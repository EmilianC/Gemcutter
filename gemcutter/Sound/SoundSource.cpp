// Copyright (c) 2017 Emilian Cioca
#include "SoundSource.h"
#include "gemcutter/Application/Logging.h"

#include <soloud.h>

namespace gem
{
	extern SoLoud::Soloud audioEngine;

	SoundSource::SoundSource(Entity& _owner)
		: Component(_owner)
	{
	}

	SoundSource::SoundSource(Entity& _owner, Sound::Ptr sound)
		: Component(_owner)
	{
		SetData(std::move(sound));
	}

	SoundSource::~SoundSource()
	{
		Stop();
	}

	void SoundSource::SetData(Sound::Ptr sound)
	{
		Stop();

		data = std::move(sound);
	}

	Sound* SoundSource::GetData() const
	{
		return data.get();
	}

	void SoundSource::Play()
	{
		ASSERT(data && data->source, "The SoundSource does not have a valid sound to play.");

		if (data->Is3D())
		{
			const vec3 pos = owner.GetWorldPosition();
			hSound = audioEngine.play3d(*data->source, pos.x, pos.y, pos.z, 0.0f, 0.0f, 0.0f, data->GetVolume() * volume);
		}
		else
		{
			hSound = audioEngine.play(*data->source, data->GetVolume() * volume);
		}
	}

	void SoundSource::Stop()
	{
		audioEngine.stop(hSound);
	}

	void SoundSource::Pause()
	{
		audioEngine.setPause(hSound, true);
	}

	void SoundSource::Resume()
	{
		audioEngine.setPause(hSound, false);
	}

	bool SoundSource::IsPlaying() const
	{
		return audioEngine.isValidVoiceHandle(hSound);
	}

	bool SoundSource::IsPaused() const
	{
		return audioEngine.getPause(hSound);
	}

	void SoundSource::SetSourceVolume(float _volume)
	{
		ASSERT(_volume >= 0.0f, "'volume' must be non negative.");

		volume = _volume;
		if (IsPlaying())
		{
			audioEngine.setVolume(hSound, data->GetVolume() * volume);
		}
	}

	float SoundSource::GetSourceVolume() const
	{
		return volume;
	}

	void SoundSource::OnDisable()
	{
		Stop();
	}
}

REFLECT_COMPONENT(gem::SoundSource, gem::ComponentBase)
	MEMBERS {
		REF_PRIVATE_MEMBER_GET_SET(data,   nullptr, &gem::SoundSource::SetData)
		REF_PRIVATE_MEMBER_GET_SET(volume, nullptr, &gem::SoundSource::SetSourceVolume, min(0.0f))
	}
REF_END;
