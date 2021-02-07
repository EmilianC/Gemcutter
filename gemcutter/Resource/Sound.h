// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Resource/Resource.h"
#include "gemcutter/Resource/Shareable.h"

namespace Jwl
{
	// An audio clip.
	// To be played, the clip must be set on an Entity's SoundSource component.
	// A SoundListener must also be present in the scene.
	class Sound : public Resource<Sound>, public Shareable<Sound>
	{
	public:
		~Sound();

		// Loads .wav (must be 8-bit unsigned PCM).
		bool Load(std::string filePath);
		void Unload();

		unsigned GetBufferHandle() const;

	private:
		unsigned hBuffer = 0;
	};
}
