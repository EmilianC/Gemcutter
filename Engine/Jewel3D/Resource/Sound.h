// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Resource.h"

namespace Jwl
{
	class Sound : public Resource<Sound>
	{
	public:
		~Sound();

		//- Loads .wav (must be 8-bit u32 PCM).
		bool Load(std::string filePath);
		void Unload();

		u32 GetBufferHandle() const;

	private:
		u32 hBuffer = 0;
	};
}
