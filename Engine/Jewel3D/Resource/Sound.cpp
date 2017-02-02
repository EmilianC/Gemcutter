// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Sound.h"
#include "Jewel3D/Math/Vector.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Sound/SoundSystem.h"

#include <OpenAL_Soft/al.h>
#include <OpenAL_Soft/alc.h>
#include <stdio.h>

#ifdef _DEBUG
	#define AL_DEBUG_CHECK() \
		{ \
			ALenum error = alGetError(); \
			if (error != AL_NO_ERROR) \
				Jwl::Error("Sound: %s", alGetString(error)); \
		}
#else
	#define AL_DEBUG_CHECK()
#endif

enum class WaveFormat: unsigned short
{
	WAVE_FORMAT_PCM			= 0x0001,	// PCM
	WAVE_FORMAT_IEEE_FLOAT	= 0x0003,	// IEEE float
	WAVE_FORMAT_ALAW		= 0x0006,	// 8 - bit ITU - T G.711 A - law
	WAVE_FORMAT_MULAW		= 0x0007,	// 8 - bit ITU - T G.711 µ - law
	WAVE_FORMAT_EXTENSIBLE	= 0xFFFE,	// Determined by SubFormat
};

struct WaveHeader
{
	WaveFormat FormatTag				= WaveFormat::WAVE_FORMAT_PCM;
	unsigned short Channels				= 0;
	unsigned SamplesPerSec				= 0;
	unsigned AvgBytesPerSec				= 0;
	unsigned short BlockAlign			= 0;
	unsigned short BitsPerSample		= 0;
	unsigned short ExtensionSize		= 0;
	unsigned short ValidBitsPerSample	= 0;
	unsigned ChannelMask				= 0;
};

namespace Jwl
{
	Sound::~Sound()
	{
		Unload();
	}

	bool Sound::Load(const std::string& InputFile)
	{
		FILE* file = fopen(InputFile.c_str(), "rb");
		if (file == nullptr)
		{
			Error("Sound: ( %s )\nUnable to open file.", InputFile.c_str());
			return false;
		}

		// Variables to store info about the WAVE file
		char chunkID[5] = { '\0' };
		unsigned chunkSize;
		WaveHeader header;
		unsigned char* soundData = nullptr;
		
		// Check that the WAVE file is OK
		fread(chunkID, sizeof(char), 4, file);
		if (strcmp(chunkID, "RIFF"))
		{
			fclose(file);
			Error("Sound: ( %s )\nIncorrect file type.", InputFile.c_str());
			return false;
		}

		fread(&chunkSize, sizeof(unsigned), 1, file);
		fread(chunkID, sizeof(char), 4, file);
		if (strcmp(chunkID, "WAVE"))
		{
			fclose(file);
			Error("Sound: ( %s )\nIncorrect file type.", InputFile.c_str());
			return false;
		}

		fread(chunkID, sizeof(char), 4, file);
		if (strcmp(chunkID, "fmt "))
		{
			fclose(file);
			Error("Sound: ( %s )\nIncorrect file type.", InputFile.c_str());
			return false;
		}

		// read sound format
		fread(&chunkSize, sizeof(unsigned), 1, file);
		fread(&header.FormatTag, sizeof(short), 1, file);
		fread(&header.Channels, sizeof(short), 1, file);
		fread(&header.SamplesPerSec, sizeof(unsigned), 1, file);
		fread(&header.AvgBytesPerSec, sizeof(unsigned), 1, file);
		fread(&header.BlockAlign, sizeof(short), 1, file);
		fread(&header.BitsPerSample, sizeof(short), 1, file);

		// skip any extra header information
		if (header.FormatTag == WaveFormat::WAVE_FORMAT_EXTENSIBLE)
		{
			fseek(file, 24, SEEK_CUR);
		}
		else if (header.FormatTag != WaveFormat::WAVE_FORMAT_PCM)
		{
			fseek(file, 2, SEEK_CUR);
		}

		// search for data chunk
		while (fread(&chunkID, sizeof(char), 4, file) != 0)
		{
			if (strcmp(chunkID, "data") == 0)
			{
				// read data
				fread(&chunkSize, sizeof(unsigned), 1, file);
				soundData = (unsigned char*)malloc(chunkSize * sizeof(unsigned char));
				fread(soundData, sizeof(unsigned char), chunkSize, file);

				break;
			}
			else
			{
				// skip chunk
				int size;
				fread(&size, sizeof(int), 1, file);
				fseek(file, size, SEEK_CUR);
			}
		}

		fclose(file);

		if (soundData == nullptr)
		{
			// we didn't find any data to load.
			Error("Sound: ( %s )\nNo data found in file.", InputFile.c_str());
			return false;
		}

		// resolve the format of the WAVE file
		ALenum format = AL_NONE;
		if (header.BitsPerSample == 8)
		{
			if (header.Channels == 1)
			{
				format = AL_FORMAT_MONO8;
			}
			else if (header.Channels == 2)
			{
				format = AL_FORMAT_STEREO8;
			}
		}
		else if (header.BitsPerSample == 16)
		{
			if (header.Channels == 1)
			{
				format = AL_FORMAT_MONO16;
			}
			else if (header.Channels == 2)
			{
				format = AL_FORMAT_STEREO16;
			}
		}

		if (format == AL_NONE)
		{
			Error("Sound: ( %s )\nUnsupported audio format.", InputFile.c_str());
			return false;
		}

		// create OpenAL buffer
		alGenBuffers(1, &hBuffer);
		ALenum error = alGetError();
		if (error != AL_NO_ERROR)
		{
			Unload();
			Error("Sound: ( %s )\n%s", InputFile.c_str(), alGetString(error));
			return false;
		}

		// send data to OpenAL
		alBufferData(hBuffer, format, soundData, chunkSize, header.SamplesPerSec);
		error = alGetError();
		if (error != AL_NO_ERROR)
		{
			Unload();
			Error("Sound: ( %s )\n%s", InputFile.c_str(), alGetString(error));
			return false;
		}

		free(soundData);

		return true;
	}

	void Sound::Unload()
	{
		if (hBuffer != AL_NONE)
		{
			alDeleteBuffers(1, &hBuffer);
			AL_DEBUG_CHECK();
			hBuffer = 0;
		}
	}

	unsigned Sound::GetBufferHandle() const
	{
		return hBuffer;
	}
}
