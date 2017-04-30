// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Reflection/Reflection.h"

struct ALCdevice_struct;
struct ALCcontext_struct;

namespace Jwl
{
	enum class AttenuationMode
	{
		None                      = 0,
		Inverse_Distance          = 0xD001,
		Inverse_Distance_Clamped  = 0xD002,
		Linear_Distance           = 0xD003,
		Linear_Distance_Clamped   = 0xD004,
		Exponent_Distance         = 0xD005,
		Exponent_Distance_Clamped = 0xD006
	};

	// Provides global control of audio playback.
	extern class SoundSystemSingleton SoundSystem;
	class SoundSystemSingleton
	{
		friend class ApplicationSingleton;
	public:
		bool Init();
		bool IsLoaded() const;
		void Unload();

		// Sets the behaviour of volume falloff over distance. Default is Inverse_Distance_Clamped.
		void SetAttenuationMode(AttenuationMode mode);
		AttenuationMode GetAttenuationMode() const;
		// Sets the global volume level. Should be [0, ...]. Default is 1.0f.
		void SetGlobalVolume(float volume);
		float GetGlobalVolume() const;

		void Mute();
		bool IsMuted() const;
		void Unmute();

	private:
		// Called every frame in order to ensure that sounds play at the correct locations.
		void Update();

		bool muted = true;
		AttenuationMode attenuationMode = AttenuationMode::Inverse_Distance_Clamped;
		float globalVolume = 1.0f;

		ALCdevice_struct* device = nullptr;
		ALCcontext_struct* context = nullptr;
	};
}

REFLECT(Jwl::AttenuationMode)<>,
	VALUES <
		REF_VALUE(None),
		REF_VALUE(Linear_Distance),
		REF_VALUE(Linear_Distance_Clamped),
		REF_VALUE(Inverse_Distance),
		REF_VALUE(Inverse_Distance_Clamped),
		REF_VALUE(Exponent_Distance),
		REF_VALUE(Exponent_Distance_Clamped)
	>
REF_END;
