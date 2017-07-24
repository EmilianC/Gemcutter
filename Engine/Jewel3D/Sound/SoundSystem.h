// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Utilities/Singleton.h"

struct ALCdevice_struct;
struct ALCcontext_struct;

namespace Jwl
{
	enum class AttenuationMode
	{
		None						= 0,
		Inverse_Distance			= 0xD001,
		Inverse_Distance_Clamped	= 0xD002,
		Linear_Distance				= 0xD003,
		Linear_Distance_Clamped		= 0xD004,
		Exponent_Distance			= 0xD005,
		Exponent_Distance_Clamped	= 0xD006
	};

	// Provides global control of audio playback.
	static class SoundSystem : public Singleton<class SoundSystem>
	{
		friend class Application;
	public:
		bool Init();
		bool IsLoaded() const;
		void Unload();

		// Sets the behaviour of volume falloff over distance.
		void SetAttenuationMode(AttenuationMode mode);
		// Sets the global volume level. Should be [0, ...]. Default is 1.0f.
		void SetGlobalVolume(float volume);

	private:
		// Called every frame in order to ensure that sounds play at the correct locations.
		void Update();

		ALCdevice_struct* device = nullptr;
		ALCcontext_struct* context = nullptr;
	} &SoundSystem = Singleton<class SoundSystem>::instanceRef;
}
