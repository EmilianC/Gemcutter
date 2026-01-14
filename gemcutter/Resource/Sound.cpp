// Copyright (c) 2017 Emilian Cioca
#include "Sound.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Utilities/ScopeGuard.h"

#include <soloud.h>
#include <soloud_wav.h>

namespace gem
{
	extern SoLoud::Soloud audioEngine;

	Sound::~Sound()
	{
		Unload();
	}

	bool Sound::Load(std::string_view filePath)
	{
		FILE* soundFile = fopen(filePath.data(), "rb");
		if (soundFile == nullptr)
		{
			Error("Sound: ( %s )\nUnable to open file.", filePath.data());
			return false;
		}
		defer { fclose(soundFile); };

		fread(&is3D,        sizeof(is3D),        1, soundFile);
		fread(&loop,        sizeof(loop),        1, soundFile);
		fread(&unique,      sizeof(unique),      1, soundFile);
		fread(&attenuation, sizeof(attenuation), 1, soundFile);
		fread(&rolloff,     sizeof(rolloff),     1, soundFile);
		fread(&volume,      sizeof(volume),      1, soundFile);
		fread(&minDistance, sizeof(minDistance), 1, soundFile);
		fread(&maxDistance, sizeof(maxDistance), 1, soundFile);

		size_t size = 0;
		fread(&size, sizeof(size), 1, soundFile);
		buffer.resize(size);
		fread(buffer.data(), size, 1, soundFile);

		auto sound = std::make_unique<SoLoud::Wav>();

		SoLoud::result result = sound->loadMem(reinterpret_cast<unsigned char*>(buffer.data()), static_cast<unsigned>(size), false, false);
		if (result != SoLoud::SOLOUD_ERRORS::SO_NO_ERROR)
		{
			Error("Sound: %s", audioEngine.getErrorString(result));
			return false;
		}

		source = sound.release();

		if (is3D)
		{
			source->mFlags |= SoLoud::AudioSource::PROCESS_3D;
		}
		source->setLooping(loop);
		source->setSingleInstance(unique);
		source->set3dAttenuation(static_cast<unsigned>(attenuation), rolloff);
		source->set3dMinMaxDistance(minDistance, maxDistance);
		source->setVolume(volume);

		return true;
	}

	void Sound::Unload()
	{
		delete source;
		source = nullptr;

		buffer.clear();
	}

	void Sound::SetIs3D(bool _is3D)
	{
		if (_is3D)
		{
			source->mFlags |= SoLoud::AudioSource::PROCESS_3D;
		}
		else
		{
			source->mFlags &= ~SoLoud::AudioSource::PROCESS_3D;
		}

		is3D = _is3D;
	}

	bool Sound::Is3D() const
	{
		return is3D;
	}

	void Sound::SetLooping(bool _loop)
	{
		source->setLooping(_loop);

		loop = _loop;
	}

	bool Sound::IsLooping() const
	{
		return loop;
	}

	void Sound::SetIsUnique(bool _unique)
	{
		source->setSingleInstance(_unique);

		unique = _unique;
	}

	bool Sound::IsUnique() const
	{
		return unique;
	}

	void Sound::SetAttenuation(AttenuationFunc func, float _rolloff)
	{
		ASSERT(_rolloff >= 0.0f, "'_rolloff' must be non negative.");
		ASSERT(func != AttenuationFunc::Linear || _rolloff <= 1.0f, "'_rolloff' cannot be greater than 1 when using linear attenuation.");

		source->set3dAttenuation(static_cast<unsigned>(func), _rolloff);

		attenuation = func;
		rolloff = _rolloff;
	}

	AttenuationFunc Sound::GetAttenuationFunc() const
	{
		return attenuation;
	}

	float Sound::GetAttenuationRolloff() const
	{
		return rolloff;
	}

	void Sound::SetDistances(float min, float max)
	{
		ASSERT(min > 0.0f, "'min' must be greater than zero.");
		ASSERT(max >= min, "'max' must be greater than or equal to 'min'");

		source->set3dMinMaxDistance(min, max);

		minDistance = min;
		maxDistance = max;
	}

	float Sound::GetMinDistance() const
	{
		return minDistance;
	}

	float Sound::GetMaxDistance() const
	{
		return maxDistance;
	}

	void Sound::SetVolume(float _volume)
	{
		ASSERT(_volume >= 0.0f, "'volume' must be non negative.");

		source->setVolume(_volume);

		volume = _volume;
	}

	float Sound::GetVolume() const
	{
		return volume;
	}
}

REFLECT(gem::AttenuationFunc)
	ENUM_VALUES {
		REF_VALUE(None)
		REF_VALUE(InverseDistance)
		REF_VALUE(Linear)
		REF_VALUE(Exponential)
	}
REF_END;

REFLECT(gem::Sound) BASES { REF_BASE(gem::ResourceBase) }
	MEMBERS {
		REF_MEMBER_EX(is3D,     nullptr, &gem::Sound::SetIs3D)
		REF_MEMBER_EX(loop,     nullptr, &gem::Sound::SetLooping)
		REF_MEMBER_EX(unique,   nullptr, &gem::Sound::SetIsUnique)
		REF_MEMBER(attenuation, readonly())
		REF_MEMBER(rolloff,     readonly(), min(0.0f))
		REF_MEMBER_EX(volume,   nullptr, &gem::Sound::SetVolume, min(0.0f))
		REF_MEMBER(minDistance, readonly(), display_name("Minimum distance"), min(0.0f))
		REF_MEMBER(maxDistance, readonly(), display_name("Maximum distance"), min(0.0f))
}
REF_END;
