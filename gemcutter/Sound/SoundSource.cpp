// Copyright (c) 2017 Emilian Cioca
#include "SoundSource.h"
#include "gemcutter/Application/Logging.h"

#include <AL/al.h>

#ifdef _DEBUG
	#define AL_DEBUG_CHECK() \
		{ \
			ALenum error = alGetError(); \
			if (error != AL_NO_ERROR) \
				gem::Error("SoundSource: %s", alGetString(error)); \
		}
#else
	#define AL_DEBUG_CHECK()
#endif

namespace gem
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

		SetData(std::move(sound));
	}

	SoundSource& SoundSource::operator=(const SoundSource& other)
	{
		if (other.data)
		{
			SetData(other.data);
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

	void SoundSource::SetData(Sound::Ptr sound)
	{
		ASSERT(sound, "SoundHandle is not valid.");

		Stop();

		data = std::move(sound);
		alSourcei(hSound, AL_BUFFER, data->GetBufferHandle());
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
		alSourcei(hSound, AL_LOOPING, _loop);
		AL_DEBUG_CHECK();
	}

	bool SoundSource::isLooping() const
	{
		return loop;
	}

	void SoundSource::SetDistance(float min, float max)
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

	float SoundSource::GetMinDistance() const
	{
		return distanceMin;
	}

	float SoundSource::GetMaxDistance() const
	{
		return distanceMax;
	}

	void SoundSource::SetVolume(float _volume)
	{
		ASSERT(_volume >= 0.0f, "'volume' must be non negative.");

		volume = _volume;
		alSourcef(hSound, AL_GAIN, _volume);
		AL_DEBUG_CHECK();
	}

	float SoundSource::GetVolume() const
	{
		return volume;
	}

	void SoundSource::SetMinVolume(float min)
	{
		ASSERT(min >= 0.0f && min <= 1.0f, "'volume' must be in the range of [0, 1].");

		volumeMin = min;
		alSourcef(hSound, AL_MIN_GAIN, min);
		AL_DEBUG_CHECK();
	}

	float SoundSource::GetMinVolume() const
	{
		return volumeMin;
	}

	void SoundSource::SetMaxVolume(float max)
	{
		ASSERT(max >= 0.0f && max <= 1.0f, "'volume' must be in the range of [0, 1].");

		volumeMax = max;
		alSourcef(hSound, AL_MAX_GAIN, max);
		AL_DEBUG_CHECK();
	}

	float SoundSource::GetMaxVolume() const
	{
		return volumeMax;
	}

	void SoundSource::SetPitch(float _pitch)
	{
		ASSERT(_pitch >= 0.0f, "'pitch' must be non negative.");

		pitch = _pitch;
		alSourcef(hSound, AL_PITCH, _pitch);
		AL_DEBUG_CHECK();
	}

	float SoundSource::GetPitch() const
	{
		return pitch;
	}

	void SoundSource::SetRolloffFactor(float _rollOff)
	{
		ASSERT(_rollOff >= 0.0f, "'factor' must be non negative.");

		rollOff = _rollOff;
		alSourcef(hSound, AL_ROLLOFF_FACTOR, _rollOff);
		AL_DEBUG_CHECK();
	}

	float SoundSource::GetRolloffFactor() const
	{
		return rollOff;
	}

	unsigned SoundSource::GetHandle() const
	{
		return hSound;
	}

	void SoundSource::OnDisable()
	{
		Stop();
	}
}
