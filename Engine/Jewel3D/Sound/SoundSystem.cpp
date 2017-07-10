// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "SoundSystem.h"
#include "SoundListener.h"
#include "SoundSource.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Entity/Entity.h"
#include "Jewel3D/Math/Matrix.h"
#include "Jewel3D/Math/Transform.h"
#include "Jewel3D/Math/Vector.h"
#include "Jewel3D/Resource/Sound.h"

#include <OpenAL_Soft/al.h>
#include <OpenAL_Soft/alc.h>

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
	bool SoundSystem::Init()
	{
		// Init audio device.
		device = alcOpenDevice(NULL);
		if (device == nullptr)
		{
			Error("SoundSystem: Failed to create device.");
			return false;
		}

		// Reset error stack.
		alGetError();

		// Make context.
		context = alcCreateContext(device, NULL);
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

	bool SoundSystem::IsLoaded() const
	{
		return context != nullptr;
	}

	void SoundSystem::Unload()
	{
		// Delete all resources that require the OpenAL context.
		UnloadAll<Sound>();

		alcMakeContextCurrent(NULL);
		alcDestroyContext(context);
		alcCloseDevice(device);

		context = nullptr;
		device = nullptr;
	}

	void SoundSystem::SetAttenuationMode(AttenuationMode mode)
	{
		ASSERT(IsLoaded(), "SoundSystem must be initialized to call this function.");

		alDistanceModel(static_cast<ALenum>(mode));
		AL_DEBUG_CHECK();
	}

	void SoundSystem::SetGlobalVolume(f32 volume)
	{
		ASSERT(IsLoaded(), "SoundSystem must be initialized to call this function.");
		ASSERT(volume >= 0.0f, "'volume' must be non negative.");

		alListenerf(AL_GAIN, volume);
		AL_DEBUG_CHECK();
	}

	void SoundSystem::Update()
	{
		ASSERT(IsLoaded(), "SoundSystem must be initialized to call this function.");

		auto listener = SoundListener::GetListener();
		if (!listener)
		{
			return;
		}

		/* Update listener */
		ALfloat pos[] =
		{
			0.0f, 0.0f, 0.0f
		};

		ALfloat orientation[] =
		{
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f
		};

		mat4 pose = listener->GetWorldTransform();

		orientation[3] = pose.data[mat4::UpX];
		orientation[4] = pose.data[mat4::UpY];
		orientation[5] = pose.data[mat4::UpZ];

		orientation[0] = pose.data[mat4::ForwardX];
		orientation[1] = pose.data[mat4::ForwardY];
		orientation[2] = pose.data[mat4::ForwardZ];

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
