// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Resource.h"
#include "Jewel3D/Reflection/Reflection.h"

namespace Jwl
{
	class Sound : public Resource<Sound>
	{
	public:
		~Sound();

		//- Loads .wav (must be 8-bit unsigned PCM).
		bool Load(const std::string& filePath);
		void Unload();

		unsigned GetBufferHandle() const;

	private:
		unsigned hBuffer = 0;
	};
}

REFLECT_SHAREABLE(Jwl::Sound)
REFLECT(Jwl::Sound) < Resource >
REF_END;
