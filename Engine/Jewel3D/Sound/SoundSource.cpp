// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "SoundSource.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Math/Transform.h"
#include "Jewel3D/Sound/SoundSystem.h"

#include <OpenAL_Soft/al.h>
#include <OpenAL_Soft/alc.h>

#ifdef _DEBUG
	#define AL_DEBUG_CHECK() \
		{ \
			ALenum error = alGetError(); \
			if (error != AL_NO_ERROR) \
				Jwl::Error("SoundSource: %s", alGetString(error)); \
		}
#else
	#define AL_DEBUG_CHECK()
#endif

namespace Jwl
{
	SoundSource::SoundSource(Entity& _owner)
		: Component(_owner)
	{
		alGenSources(1, &hSound);
		AL_DEBUG_CHECK();
	}

	SoundSource::SoundSource(Entity& _owner, Sound::Ptr sound)
		: Component(_owner)
	{
		alGenSources(1, &hSound);
		AL_DEBUG_CHECK();

		AddData(sound);
	}

	SoundSource& SoundSource::operator=(const SoundSource& other)
	{
		if (other.data)
		{
			AddData(other.data);
		}

		return *this;
	}

	SoundSource::~SoundSource()
	{
		Stop();

		alSourcei(hSound, AL_BUFFER, AL_NONE);
		AL_DEBUG_CHECK();

		alDeleteSources(1, &hSound);
		AL_DEBUG_CHECK();
	}

	void SoundSource::AddData(Sound::Ptr sound)
	{
		ASSERT(sound, "SoundHandle is not valid.");

		data = sound;

		alSourcei(hSound, AL_BUFFER, sound->GetBufferHandle());
		AL_DEBUG_CHECK();
	}

	Sound::Ptr SoundSource::GetData() const
	{
		return data;
	}

	void SoundSource::Play()
	{
		alSourceRewind(hSound);
		AL_DEBUG_CHECK();

		alSourcePlay(hSound);
		AL_DEBUG_CHECK();
	}

	void SoundSource::Stop()
	{
		alSourceStop(hSound);
		AL_DEBUG_CHECK();
	}

	void SoundSource::Pause()
	{
		alSourcePause(hSound);
		AL_DEBUG_CHECK();
	}

	void SoundSource::Resume()
	{
		alSourcePlay(hSound);
		AL_DEBUG_CHECK();
	}

	bool SoundSource::IsPlaying() const
	{
		ALenum state;
		alGetSourcei(hSound, AL_SOURCE_STATE, &state);

		return state == AL_PLAYING;
	}

	bool SoundSource::IsPaused() const
	{
		ALenum state;
		alGetSourcei(hSound, AL_SOURCE_STATE, &state);

		return state == AL_PAUSED;
	}

	void SoundSource::SetLooping(bool _loop)
	{
		loop = _loop;
		alSourcef(hSound, AL_LOOPING, _loop);
		AL_DEBUG_CHECK();
	}

	bool SoundSource::isLooping() const
	{
		return loop;
	}

	void SoundSource::SetDistance(f32 min, f32 max)
	{
		ASSERT(min >= 0.0f, "'min' must be non negative.");
		ASSERT(max >= 0.0f && max >= min, "'min' must be less than or equal to 'max' and 'max' must be non negative.");

		distanceMin = min;
		alSourcef(hSound, AL_REFERENCE_DISTANCE, min);
		AL_DEBUG_CHECK();

		distanceMin = max;
		alSourcef(hSound, AL_MAX_DISTANCE, max);
		AL_DEBUG_CHECK();
	}

	f32 SoundSource::GetMinDistance() const
	{
		return distanceMin;
	}

	f32 SoundSource::GetMaxDistance() const
	{
		return distanceMax;
	}

	void SoundSource::SetVolume(f32 _volume)
	{
		ASSERT(_volume >= 0.0f, "'volume' must be non negative.");

		volume = _volume;
		alSourcef(hSound, AL_GAIN, _volume);
		AL_DEBUG_CHECK();
	}

	f32 SoundSource::GetVolume() const
	{
		return volume;
	}

	void SoundSource::SetMinVolume(f32 min)
	{
		ASSERT(min >= 0.0f && min <= 1.0f, "'volume' must be in the range of [0, 1].");

		volumeMin = min;
		alSourcef(hSound, AL_MIN_GAIN, min);
		AL_DEBUG_CHECK();
	}

	f32 SoundSource::GetMinVolume() const
	{
		return volumeMin;
	}

	void SoundSource::SetMaxVolume(f32 max)
	{
		ASSERT(max >= 0.0f && max <= 1.0f, "'volume' must be in the range of [0, 1].");

		volumeMax = max;
		alSourcef(hSound, AL_MAX_GAIN, max);
		AL_DEBUG_CHECK();
	}

	f32 SoundSource::GetMaxVolume() const
	{
		return volumeMax;
	}

	void SoundSource::SetPitch(f32 _pitch)
	{
		ASSERT(_pitch >= 0.0f, "'pitch' must be non negative.");

		pitch = _pitch;
		alSourcef(hSound, AL_PITCH, _pitch);
		AL_DEBUG_CHECK();
	}

	f32 SoundSource::GetPitch() const
	{
		return pitch;
	}

	void SoundSource::SetRolloffFactor(f32 _rollOff)
	{
		ASSERT(_rollOff >= 0.0f, "'factor' must be non negative.");

		rollOff = _rollOff;
		alSourcef(hSound, AL_ROLLOFF_FACTOR, _rollOff);
		AL_DEBUG_CHECK();
	}

	f32 SoundSource::GetRolloffFactor() const
	{
		return rollOff;
	}

	u32 SoundSource::GetHandle() const
	{
		return hSound;
	}

	void SoundSource::OnDisable()
	{
		Stop();
	}
}
