// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Entity/Entity.h"
#include "Jewel3D/Resource/Sound.h"

namespace Jwl
{
	//- Position of a SoundSource is updated automatically based on the location of its owning SceneNode.
	class SoundSource : public Component<SoundSource>
	{
	public:
		SoundSource(Entity& owner);
		SoundSource(Entity& owner, Sound::Ptr sound);
		SoundSource& operator=(const SoundSource&);
		~SoundSource();

		void AddData(Sound::Ptr sound);
		Sound::Ptr GetData() const;

		void Play();
		void Stop();
		void Pause();
		void Resume();
		bool IsPlaying() const;
		bool IsPaused() const;

		//- Causes the sound to loop to the beginning when done.
		void SetLooping(bool loop);
		//- Sets the min/max range for attenuation. Should be [0, ...] for both. Default is 1.0f for both.
		void SetDistance(float min, float max);
		//- Volume should be [0, ...]. Default is 1.0f.
		void SetVolume(float volume);
		//- Minimum volume this source can reach. Should be [0, 1]. Default is 0.0f.
		void SetMinVolume(float volume);
		//- Maximum volume this source can reach. Should be [0, 1]. Default is 1.0f.
		void SetMaxVolume(float volume);
		//- Pitch should be [0, ...]. Default is 1.0f.
		void SetPitch(float pitch);
		//- Roll-off should be [0, ...]. Default is 1.0f. Values > 1 increase intensity of roll-off.
		void SetRolloffFactor(float factor);

		//- Returns the OpenAL sound handle. Used internally.
		unsigned GetHandle() const;

	private:
		//- Stops any playing sound when disabled.
		virtual void OnDisable() final override;

		Sound::Ptr data;

		//- An OpenAL sound handle.
		unsigned hSound = 0;
	};
}

REFLECT(Jwl::SoundSource) < Component >
REF_END;
