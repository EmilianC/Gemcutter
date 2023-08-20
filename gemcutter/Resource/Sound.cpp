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

		fread(&is3D, sizeof(bool), 1, soundFile);
		fread(&loop, sizeof(bool), 1, soundFile);
		fread(&unique, sizeof(bool), 1, soundFile);
		fread(&attenuation, sizeof(AttenuationFunc), 1, soundFile);
		fread(&rolloff, sizeof(float), 1, soundFile);
		fread(&volume, sizeof(float), 1, soundFile);
		fread(&minDistance, sizeof(float), 1, soundFile);
		fread(&maxDistance, sizeof(float), 1, soundFile);

		size_t size = 0;
		fread(&size, sizeof(size_t), 1, soundFile);
		buffer.resize(size);
		fread(buffer.data(), size, 1, soundFile);

		auto sound = std::make_unique<SoLoud::Wav>();

		SoLoud::result result = sound->loadMem(buffer.data(), size, false, false);
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
		REF_PRIVATE_MEMBER(is3D, gem::ReadOnly)
		REF_PRIVATE_MEMBER(loop, gem::ReadOnly)
		REF_PRIVATE_MEMBER(unique, gem::ReadOnly)
		REF_PRIVATE_MEMBER(attenuation, gem::ReadOnly)
		REF_PRIVATE_MEMBER(rolloff, gem::ReadOnly)
		REF_PRIVATE_MEMBER(volume, gem::ReadOnly)
		REF_PRIVATE_MEMBER(minDistance, gem::ReadOnly)
		REF_PRIVATE_MEMBER(maxDistance, gem::ReadOnly)
	}
REF_END;
