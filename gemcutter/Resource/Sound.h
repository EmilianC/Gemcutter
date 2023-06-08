// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Resource/Resource.h"
#include "gemcutter/Resource/Shareable.h"

namespace SoLoud
{
	class AudioSource;
}

namespace gem
{
	enum class AttenuationFunc
	{
		None = 0,
		InverseDistance = 1,
		Linear = 2,
		Exponential = 3
	};

	// An audio clip which can be attached to an Entity's SoundSource component.
	// A SoundListener must also be present in the scene for 3d audio effects.
	class Sound : public Resource<Sound>, public Shareable<Sound>
	{
		friend class SoundSource; // For source.
	public:
		static constexpr std::string_view Extension = ".sound";

		~Sound();

		// Loads pre-packed *.sound resources.
		bool Load(std::string_view filePath);
		void Unload();

		void SetIs3D(bool is3D);
		bool Is3D() const;

		// Causes the sound to loop to the beginning when done.
		void SetLooping(bool loop);
		bool IsLooping() const;

		// Playing a unique sound will stop any other currently playing instance of the sound.
		void SetIsUnique(bool unique);
		bool IsUnique() const;

		// Sets the behaviour of volume roll-off over distance.
		// Roll-off should be [0, ...], or [0, 1] for linear attenuation. A larger value speeds up roll-off.
		void SetAttenuation(AttenuationFunc func, float rolloff);
		AttenuationFunc GetAttenuationFunc() const;
		float GetAttenuationRolloff() const;

		// Sets the min/max range for attenuation. Should be (0, ...] for both.
		void SetDistances(float min, float max);
		float GetMinDistance() const;
		float GetMaxDistance() const;

		// Volume should be [0, ...].
		void SetVolume(float volume);
		float GetVolume() const;

	private:
		SoLoud::AudioSource* source = nullptr;
		std::vector<unsigned char> buffer;

		bool is3D = false;
		bool loop = false;
		bool unique = false;
		AttenuationFunc attenuation = AttenuationFunc::None;
		float rolloff = 1.0f;
		float volume = 1.0f;
		float minDistance = 0.0f;
		float maxDistance = 1.0f;
	};

}
