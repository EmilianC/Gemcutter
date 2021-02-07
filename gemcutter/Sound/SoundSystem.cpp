// Copyright (c) 2017 Emilian Cioca
#include "SoundSystem.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Entity/Entity.h"
#include "gemcutter/Math/Matrix.h"
#include "gemcutter/Math/Vector.h"
#include "gemcutter/Resource/Sound.h"
#include "gemcutter/Sound/SoundListener.h"
#include "gemcutter/Sound/SoundSource.h"

#include <AL/al.h>
#include <AL/alc.h>

#ifdef _DEBUG
	#define AL_DEBUG_CHECK() \
		{ \
			ALenum error = alGetError(); \
			if (error != AL_NO_ERROR) \
				Jwl::Error("SoundSystem: %s", alGetString(error)); \
		}
#else
	#define AL_DEBUG_CHECK()
#endif

namespace Jwl
{
	SoundSystemSingleton SoundSystem;

	bool SoundSystemSingleton::Init()
	{
		// Init audio device.
		device = alcOpenDevice(nullptr);
		if (device == nullptr)
		{
			Error("SoundSystem: Failed to create device.");
			return false;
		}

		// Reset error stack.
		alGetError();

		// Make context.
		context = alcCreateContext(device, nullptr);
		if (context == nullptr)
		{
			Error("SoundSystem: Failed to create context.");
			device = nullptr;
			return false;
		}

		// Set context as current.
		alcMakeContextCurrent(context);
		if (alGetError() != AL_NO_ERROR)
		{
			Error("SoundSystem: Failed to make context current.");
			Unload();
			return false;
		}

		return true;
	}

	bool SoundSystemSingleton::IsLoaded() const
	{
		return context != nullptr;
	}

	void SoundSystemSingleton::Unload()
	{
		// Delete all resources that require the OpenAL context.
		UnloadAll<Sound>();

		alcMakeContextCurrent(nullptr);
		alcDestroyContext(context);
		alcCloseDevice(device);

		muted = true;
		attenuationMode = AttenuationMode::Inverse_Distance_Clamped;
		globalVolume = 1.0f;
		device = nullptr;
		context = nullptr;
	}

	void SoundSystemSingleton::SetAttenuationMode(AttenuationMode mode)
	{
		ASSERT(IsLoaded(), "SoundSystem must be initialized to call this function.");

		alDistanceModel(static_cast<ALenum>(mode));
		AL_DEBUG_CHECK();

		attenuationMode = mode;
	}

	AttenuationMode SoundSystemSingleton::GetAttenuationMode() const
	{
		return attenuationMode;
	}

	void SoundSystemSingleton::SetGlobalVolume(float volume)
	{
		ASSERT(IsLoaded(), "SoundSystem must be initialized to call this function.");
		ASSERT(volume >= 0.0f, "'volume' must be non negative.");

		if (!muted)
		{
			alListenerf(AL_GAIN, volume);
			AL_DEBUG_CHECK();
		}

		globalVolume = volume;
	}

	float SoundSystemSingleton::GetGlobalVolume() const
	{
		return globalVolume;
	}

	void SoundSystemSingleton::Mute()
	{
		ASSERT(IsLoaded(), "SoundSystem must be initialized to call this function.");

		alListenerf(AL_GAIN, 0.0f);
		AL_DEBUG_CHECK();

		muted = true;
	}

	bool SoundSystemSingleton::IsMuted() const
	{
		return muted;
	}

	void SoundSystemSingleton::Unmute()
	{
		ASSERT(IsLoaded(), "SoundSystem must be initialized to call this function.");

		alListenerf(AL_GAIN, globalVolume);
		AL_DEBUG_CHECK();

		muted = false;
	}

	void SoundSystemSingleton::Update()
	{
		ASSERT(IsLoaded(), "SoundSystem must be initialized to call this function.");

		if (muted)
		{
			return;
		}

		auto listener = SoundListener::GetListener();
		if (!listener)
		{
			return;
		}

		/* Update listener */
		ALfloat pos[3] = { 0.0f };
		ALfloat orientation[6] = { 0.0f };

		const mat4 pose = listener->GetWorldTransform();
		orientation[0] = -pose.data[mat4::ForwardX];
		orientation[1] = -pose.data[mat4::ForwardY];
		orientation[2] = -pose.data[mat4::ForwardZ];
		orientation[3] = pose.data[mat4::UpX];
		orientation[4] = pose.data[mat4::UpY];
		orientation[5] = pose.data[mat4::UpZ];
		pos[0] = pose.data[mat4::TransX];
		pos[1] = pose.data[mat4::TransY];
		pos[2] = pose.data[mat4::TransZ];

		alListenerfv(AL_POSITION, pos);
		AL_DEBUG_CHECK();

		alListenerfv(AL_ORIENTATION, orientation);
		AL_DEBUG_CHECK();

		/* Update all sounds */
		for (auto& source : All<SoundSource>())
		{
			if (!source.IsPlaying())
				continue;

			vec3 position = source.owner.GetWorldTransform().GetTranslation();
			pos[0] = position.x;
			pos[1] = position.y;
			pos[2] = position.z;

			alSourcefv(source.GetHandle(), AL_POSITION, pos);
			AL_DEBUG_CHECK();
		}
	}
}
