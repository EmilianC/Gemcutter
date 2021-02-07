// Copyright (c) 2017 Emilian Cioca
#include "Sound.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Utilities/ScopeGuard.h"
#include "gemcutter/Utilities/String.h"

#include <AL/al.h>
#include <cstdio>

#ifdef _DEBUG
	#define AL_DEBUG_CHECK() \
		{ \
			ALenum error = alGetError(); \
			if (error != AL_NO_ERROR) \
				gem::Error("Sound: %s", alGetString(error)); \
		}
#else
	#define AL_DEBUG_CHECK()
#endif

enum class WaveFormat : unsigned short
{
	WAVE_FORMAT_PCM        = 0x0001, // PCM
	WAVE_FORMAT_IEEE_FLOAT = 0x0003, // IEEE float
	WAVE_FORMAT_ALAW       = 0x0006, // 8 - bit ITU - T G.711 A - law
	WAVE_FORMAT_MULAW      = 0x0007, // 8 - bit ITU - T G.711 µ - law
	WAVE_FORMAT_EXTENSIBLE = 0xFFFE, // Determined by SubFormat
};

struct WaveHeader
{
	WaveFormat FormatTag              = WaveFormat::WAVE_FORMAT_PCM;
	unsigned short Channels           = 0;
	unsigned SamplesPerSec            = 0;
	unsigned AvgBytesPerSec           = 0;
	unsigned short BlockAlign         = 0;
	unsigned short BitsPerSample      = 0;
	unsigned short ExtensionSize      = 0;
	unsigned short ValidBitsPerSample = 0;
	unsigned ChannelMask              = 0;
};

namespace gem
{
	Sound::~Sound()
	{
		Unload();
	}

	bool Sound::Load(std::string filePath)
	{
		auto ext = ExtractFileExtension(filePath);
		if (ext.empty())
		{
			filePath += ".wav";
		}
		else if (!CompareLowercase(ext, ".wav"))
		{
			Error("Sound: ( %s )\nAttempted to load unknown file type as a sound.", filePath.c_str());
			return false;
		}

		FILE* file = fopen(filePath.c_str(), "rb");
		if (file == nullptr)
		{
			Error("Sound: ( %s )\nUnable to open file.", filePath.c_str());
			return false;
		}
		defer { fclose(file); };

		// Variables to store info about the WAVE file.
		char chunkID[5] = { '\0' };
		unsigned chunkSize;
		WaveHeader header;
		unsigned char* soundData = nullptr;
		defer{ free(soundData); };

		// Check that the WAVE file is OK.
		fread(chunkID, sizeof(char), 4, file);
		if (strcmp(chunkID, "RIFF") != 0)
		{
			Error("Sound: ( %s )\nIncorrect file type.", filePath.c_str());
			return false;
		}

		fread(&chunkSize, sizeof(unsigned), 1, file);
		fread(chunkID, sizeof(char), 4, file);
		if (strcmp(chunkID, "WAVE") != 0)
		{
			Error("Sound: ( %s )\nIncorrect file type.", filePath.c_str());
			return false;
		}

		fread(chunkID, sizeof(char), 4, file);
		if (strcmp(chunkID, "fmt ") != 0)
		{
			Error("Sound: ( %s )\nIncorrect file type.", filePath.c_str());
			return false;
		}

		// Read sound format.
		fread(&chunkSize, sizeof(unsigned), 1, file);
		fread(&header.FormatTag, sizeof(short), 1, file);
		fread(&header.Channels, sizeof(short), 1, file);
		fread(&header.SamplesPerSec, sizeof(unsigned), 1, file);
		fread(&header.AvgBytesPerSec, sizeof(unsigned), 1, file);
		fread(&header.BlockAlign, sizeof(short), 1, file);
		fread(&header.BitsPerSample, sizeof(short), 1, file);

		// Skip any extra header information.
		if (header.FormatTag == WaveFormat::WAVE_FORMAT_EXTENSIBLE)
		{
			fseek(file, 24, SEEK_CUR);
		}
		else if (header.FormatTag != WaveFormat::WAVE_FORMAT_PCM)
		{
			fseek(file, 2, SEEK_CUR);
		}

		// Search for data chunk.
		while (fread(&chunkID, sizeof(char), 4, file) != 0)
		{
			if (strcmp(chunkID, "data") == 0)
			{
				// Read data.
				fread(&chunkSize, sizeof(unsigned), 1, file);
				soundData = static_cast<unsigned char*>(malloc(chunkSize * sizeof(unsigned char)));
				fread(soundData, sizeof(unsigned char), chunkSize, file);

				break;
			}
			else
			{
				// Skip chunk.
				int size;
				fread(&size, sizeof(int), 1, file);
				fseek(file, size, SEEK_CUR);
			}
		}

		if (soundData == nullptr)
		{
			// We didn't find any data to load.
			Error("Sound: ( %s )\nNo data found in file.", filePath.c_str());
			return false;
		}

		// Resolve the format of the WAVE file.
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
			Error("Sound: ( %s )\nUnsupported audio format.", filePath.c_str());
			return false;
		}

		// Create OpenAL buffer.
		alGenBuffers(1, &hBuffer);
		ALenum error = alGetError();
		if (error != AL_NO_ERROR)
		{
			Unload();
			Error("Sound: ( %s )\n%s", filePath.c_str(), alGetString(error));
			return false;
		}

		// Send data to OpenAL.
		alBufferData(hBuffer, format, soundData, chunkSize, header.SamplesPerSec);
		error = alGetError();
		if (error != AL_NO_ERROR)
		{
			Unload();
			Error("Sound: ( %s )\n%s", filePath.c_str(), alGetString(error));
			return false;
		}

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
