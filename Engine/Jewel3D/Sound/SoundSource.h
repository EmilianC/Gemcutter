// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Entity/Entity.h"
#include "Jewel3D/Resource/Sound.h"

namespace Jwl
{
	// Plays audio clips in 3D space.
	class SoundSource : public Component<SoundSource>
	{
	public:
		SoundSource(Entity& owner);
		SoundSource(Entity& owner, Sound::Ptr sound);
		SoundSource& operator=(const SoundSource&);
		~SoundSource();

		void SetData(Sound::Ptr sound);
		Sound::Ptr GetData() const;

		void Play();
		void Stop();
		void Pause();
		void Resume();
		bool IsPlaying() const;
		bool IsPaused() const;

		// Causes the sound to loop to the beginning when done. Default is off.
		void SetLooping(bool loop);
		bool isLooping() const;

		// Sets the min/max range for attenuation. Should be [0, ...] for both. Default is 1.0f for both.
		void SetDistance(float min, float max);
		float GetMinDistance() const;
		float GetMaxDistance() const;

		// Volume should be [0, ...]. Default is 1.0f.
		void SetVolume(float volume);
		float GetVolume() const;

		// Minimum volume this source can reach. Should be [0, 1]. Default is 0.0f.
		void SetMinVolume(float min);
		float GetMinVolume() const;

		// Maximum volume this source can reach. Should be [0, 1]. Default is 1.0f.
		void SetMaxVolume(float max);
		float GetMaxVolume() const;

		// Pitch should be [0, ...]. Default is 1.0f.
		void SetPitch(float pitch);
		float GetPitch() const;

		// Roll-off should be [0, ...]. Default is 1.0f. Values > 1 increase intensity of roll-off.
		void SetRolloffFactor(float rollOff);
		float GetRolloffFactor() const;

		// Returns the OpenAL sound handle. Used internally.
		unsigned GetHandle() const;

	private:
		// Stops any playing sound when disabled.
		void OnDisable() final override;

		Sound::Ptr data;

		// An OpenAL sound handle.
		unsigned hSound = 0;

		bool loop = false;
		float distanceMin = 1.0f;
		float distanceMax = 1.0f;
		float volume = 1.0f;
		float volumeMin = 0.0f;
		float volumeMax = 1.0f;
		float pitch = 1.0f;
		float rollOff = 1.0f;
	};
}
