// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "SoundSource.h"
#include "Jewel3D/Math/Transform.h"
#include "Jewel3D/Sound/SoundSystem.h"
#include "Jewel3D/Application/Logging.h"

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

	void SoundSource::SetLooping(bool loop)
	{
		alSourcef(hSound, AL_LOOPING, loop);
		AL_DEBUG_CHECK();
	}

	void SoundSource::SetDistance(float min, float max)
	{
		ASSERT(min >= 0.0f, "'min' must be non negative.");
		ASSERT(max >= 0.0f && max >= min, "'min' must be less than or equal to 'max' and 'max' must be non negative.");

		alSourcef(hSound, AL_REFERENCE_DISTANCE, min);
		AL_DEBUG_CHECK();

		alSourcef(hSound, AL_MAX_DISTANCE, max);
		AL_DEBUG_CHECK();
	}

	void SoundSource::SetVolume(float volume)
	{
		ASSERT(volume >= 0.0f, "'volume' must be non negative.");

		alSourcef(hSound, AL_GAIN, volume);
		AL_DEBUG_CHECK();
	}

	void SoundSource::SetMinVolume(float volume)
	{
		ASSERT(volume >= 0.0f && volume <= 1.0f, "'volume' must be in the range of [0, 1].");

		alSourcef(hSound, AL_MIN_GAIN, volume);
		AL_DEBUG_CHECK();
	}

	void SoundSource::SetMaxVolume(float volume)
	{
		ASSERT(volume >= 0.0f && volume <= 1.0f, "'volume' must be in the range of [0, 1].");

		alSourcef(hSound, AL_MAX_GAIN, volume);
		AL_DEBUG_CHECK();
	}

	void SoundSource::SetPitch(float pitch)
	{
		ASSERT(pitch >= 0.0f, "'pitch' must be non negative.");

		alSourcef(hSound, AL_PITCH, pitch);
		AL_DEBUG_CHECK();
	}

	void SoundSource::SetRolloffFactor(float factor)
	{
		ASSERT(factor >= 0.0f, "'factor' must be non negative.");

		alSourcef(hSound, AL_ROLLOFF_FACTOR, factor);
		AL_DEBUG_CHECK();
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
