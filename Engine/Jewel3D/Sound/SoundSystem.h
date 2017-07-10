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
		Linear_Distance				= 0xD003,
		Linear_Distance_Clamped		= 0xD004,
		Inverse_Distance			= 0xD001,
		Inverse_Distance_Clamped	= 0xD002,
		Exponent_Distance			= 0xD005,
		Exponent_Distance_Clamped	= 0xD006
	};

	static class SoundSystem : public Singleton<class SoundSystem>
	{
	public:
		bool Init();
		bool IsLoaded() const;
		void Unload();

		//- Sets the behaviour of volume falloff over distance.
		void SetAttenuationMode(AttenuationMode mode);
		//- Sets the global volume level. Should be [0, ...]. Default is 1.0f.
		void SetGlobalVolume(f32 volume);

		//- This must be called every frame in order to ensure that sounds play at the correct locations.
		void Update();

	private:
		ALCdevice_struct* device = nullptr;
		ALCcontext_struct* context = nullptr;
	} &SoundSystem = Singleton<class SoundSystem>::instanceRef;
}
