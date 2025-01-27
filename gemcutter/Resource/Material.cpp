// Copyright (c) 2017 Emilian Cioca
#include "Material.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Utilities/ScopeGuard.h"

#include <dirent/dirent.h>

namespace gem
{
	bool Material::Load(std::string_view filePath)
	{
		// Load binary file.
		FILE* binaryFile = fopen(filePath.data(), "rb");
		if (binaryFile == nullptr)
		{
			Error("Material: ( %s )\nUnable to open file.", filePath.data());
			return false;
		}
		defer{ fclose(binaryFile); };

		char pathBuffer[MAX_PATH + 1];

		size_t shaderLen = 0;
		fread(&shaderLen, sizeof(shaderLen), 1, binaryFile);
		if (shaderLen == 0)
		{
			shader = Shader::MakeNewPassThrough();
		}
		else
		{
			if (shaderLen > MAX_PATH)
			{
				Error("Material: ( %s )\nInvalid Shader file path length.", filePath.data());
				return false;
			}
			fread(pathBuffer, sizeof(char), shaderLen, binaryFile);
			pathBuffer[shaderLen] = '\0';

			shader = gem::Load<Shader>(pathBuffer);
			if (!shader)
			{
				Error("Material: ( %s )\nFailed to load Shader ( %s ).", filePath.data(), pathBuffer);
				return false;
			}
		}

		size_t textureCount = 0;
		fread(&textureCount, sizeof(textureCount), 1, binaryFile);
		if (textureCount > GPUInfo.GetMaxTextureSlots())
		{
			Error("Material: ( %s )\nMaterial contains more texture units than is supported ( %d ).", filePath.data(), GPUInfo.GetMaxTextureSlots());
			return false;
		}

		for (size_t i = 0; i < textureCount; ++i)
		{
			int unit = 0;
			fread(&unit, sizeof(unit), 1, binaryFile);

			size_t textureLen = 0;
			fread(&textureLen, sizeof(textureLen), 1, binaryFile);
			if (textureLen == 0 || textureLen > MAX_PATH)
			{
				Error("Material: ( %s )\nInvalid Texture file path length.", filePath.data());
				return false;
			}

			fread(pathBuffer, sizeof(char), textureLen, binaryFile);
			pathBuffer[textureLen] = '\0';

			auto texture = gem::Load<Texture>(pathBuffer);
			if (!texture)
			{
				Error("Material: ( %s )\nFailed to load Texture ( %s ).", filePath.data(), pathBuffer);
				return false;
			}

			textures.Add(std::move(texture), unit);
		}

		fread(&blendMode, sizeof(blendMode), 1, binaryFile);
		fread(&depthMode, sizeof(depthMode), 1, binaryFile);
		fread(&cullMode, sizeof(cullMode), 1, binaryFile);

		return true;
	}
}

REFLECT(gem::Material) BASES { REF_BASE(gem::ResourceBase) }
	MEMBERS {
		REF_MEMBER(blendMode)
		REF_MEMBER(depthMode)
		REF_MEMBER(cullMode)
		REF_MEMBER(shader)
		REF_MEMBER(textures)
	}
REF_END;
