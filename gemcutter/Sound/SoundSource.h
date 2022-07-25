// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Entity/Entity.h"
#include "gemcutter/Resource/Sound.h"

namespace gem
{
	// Plays 2D or 3D audio.
	class SoundSource : public Component<SoundSource>
	{
		friend class SoundSystemSingleton; // For hSound.
	public:
		SoundSource(Entity& owner);
		SoundSource(Entity& owner, Sound::Ptr sound);
		~SoundSource();

		void SetData(Sound::Ptr sound);
		Sound* GetData() const;

		void Play();
		void Stop();
		void Pause();
		void Resume();
		bool IsPlaying() const;
		bool IsPaused() const;

		// Sets the volume multiplier of this SoundSource instance.
		void SetSourceVolume(float volume);
		float GetSourceVolume() const;

	private:
		// Stops any playing sound when disabled.
		void OnDisable() final override;

		Sound::Ptr data;

		// A handle for the sound instance.
		unsigned hSound = 0;
		float volume = 1.0f;
	};
}
