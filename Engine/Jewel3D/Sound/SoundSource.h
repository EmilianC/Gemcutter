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

		//- Causes the sound to loop to the beginning when done. Default is off.
		void SetLooping(bool loop);
		bool isLooping() const;

		//- Sets the min/max range for attenuation. Should be [0, ...] for both. Default is 1.0f for both.
		void SetDistance(f32 min, f32 max);
		f32 GetMinDistance() const;
		f32 GetMaxDistance() const;

		//- Volume should be [0, ...]. Default is 1.0f.
		void SetVolume(f32 volume);
		f32 GetVolume() const;

		//- Minimum volume this source can reach. Should be [0, 1]. Default is 0.0f.
		void SetMinVolume(f32 min);
		f32 GetMinVolume() const;

		//- Maximum volume this source can reach. Should be [0, 1]. Default is 1.0f.
		void SetMaxVolume(f32 max);
		f32 GetMaxVolume() const;

		//- Pitch should be [0, ...]. Default is 1.0f.
		void SetPitch(f32 pitch);
		f32 GetPitch() const;

		//- Roll-off should be [0, ...]. Default is 1.0f. Values > 1 increase intensity of roll-off.
		void SetRolloffFactor(f32 rollOff);
		f32 GetRolloffFactor() const;

		//- Returns the OpenAL sound handle. Used internally.
		u32 GetHandle() const;

	private:
		//- Stops any playing sound when disabled.
		virtual void OnDisable() final override;

		Sound::Ptr data;

		//- An OpenAL sound handle.
		u32 hSound = 0;

		bool loop = false;
		f32 distanceMin = 1.0f;
		f32 distanceMax = 1.0f;
		f32 volume = 1.0f;
		f32 volumeMin = 0.0f;
		f32 volumeMax = 1.0f;
		f32 pitch = 1.0f;
		f32 rollOff = 1.0f;
	};
}
