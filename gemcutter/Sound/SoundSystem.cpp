// Copyright (c) 2017 Emilian Cioca
#include "SoundSystem.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Entity/Entity.h"
#include "gemcutter/Math/Matrix.h"
#include "gemcutter/Math/Vector.h"
#include "gemcutter/Resource/Sound.h"
#include "gemcutter/Sound/SoundListener.h"
#include "gemcutter/Sound/SoundSource.h"

#include <soloud.h>

namespace gem
{
	SoundSystemSingleton SoundSystem;

	SoLoud::Soloud audioEngine;

	bool SoundSystemSingleton::Init()
	{
		SoLoud::result result = audioEngine.init();
		if (result != SoLoud::SOLOUD_ERRORS::SO_NO_ERROR)
		{
			Error("SoundSystem: %s", audioEngine.getErrorString(result));
			return false;
		}

		initialized = true;
		return true;
	}

	bool SoundSystemSingleton::IsLoaded() const
	{
		return initialized;
	}

	void SoundSystemSingleton::Unload()
	{
		// Delete all resources.
		UnloadAll<Sound>();

		audioEngine.deinit();

		muted = true;
		initialized = false;
		globalVolume = 1.0f;
	}

	void SoundSystemSingleton::SetGlobalVolume(float volume)
	{
		ASSERT(IsLoaded(), "SoundSystem must be initialized to call this function.");
		ASSERT(volume >= 0.0f, "'volume' must be non negative.");

		if (!muted)
		{
			audioEngine.setGlobalVolume(volume);
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

		audioEngine.setGlobalVolume(0.0f);
		muted = true;
	}

	bool SoundSystemSingleton::IsMuted() const
	{
		return muted;
	}

	void SoundSystemSingleton::Unmute()
	{
		ASSERT(IsLoaded(), "SoundSystem must be initialized to call this function.");

		audioEngine.setGlobalVolume(globalVolume);
		muted = false;
	}

	void SoundSystemSingleton::Update()
	{
		ASSERT(IsLoaded(), "SoundSystem must be initialized to call this function.");

		if (muted)
		{
			return;
		}

		Entity::Ptr listener = SoundListener::GetListener();
		if (!listener)
		{
			return;
		}

		/* Update listener */
		const mat4 pose = listener->GetWorldTransform();
		audioEngine.set3dListenerParameters(
			pose.data[mat4::TransX],
			pose.data[mat4::TransY],
			pose.data[mat4::TransZ],
			-pose.data[mat4::ForwardX],
			-pose.data[mat4::ForwardY],
			-pose.data[mat4::ForwardZ],
			pose.data[mat4::UpX],
			pose.data[mat4::UpY],
			pose.data[mat4::UpZ],
			0.0f,
			0.0f,
			0.0f
		);

		/* Update all sounds */
		for (auto& source : All<SoundSource>())
		{
			if (!source.IsPlaying())
				continue;

			vec3 position = source.owner.GetWorldPosition();
			audioEngine.set3dSourceParameters(
				source.hSound,
				position.x,
				position.y,
				position.z
			);
		}

		audioEngine.update3dAudio();
	}
}
