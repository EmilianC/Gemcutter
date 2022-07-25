// Copyright (c) 2017 Emilian Cioca
#pragma once

namespace gem
{
	// Provides global control of audio playback.
	extern class SoundSystemSingleton SoundSystem;
	class SoundSystemSingleton
	{
		friend class ApplicationSingleton; // For Update().
	public:
		bool Init();
		bool IsLoaded() const;
		void Unload();

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
		bool initialized = false;
		float globalVolume = 1.0f;
	};
}
