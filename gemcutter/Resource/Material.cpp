// Copyright (c) 2017 Emilian Cioca
#include "Material.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Utilities/ScopeGuard.h"
#include "gemcutter/Utilities/String.h"

#include <Dirent/dirent.h>

namespace Jwl
{
	bool Material::Load(std::string filePath)
	{
		auto ext = ExtractFileExtension(filePath);
		if (ext.empty())
		{
			filePath += ".material";
		}
		else if (!CompareLowercase(ext, ".material"))
		{
			Error("Material: ( %s )\nAttempted to load unknown file type as a material.", filePath.c_str());
			return false;
		}

		// Load binary file.
		FILE* binaryFile = fopen(filePath.c_str(), "rb");
		if (binaryFile == nullptr)
		{
			Error("Material: ( %s )\nUnable to open file.", filePath.c_str());
			return false;
		}
		defer{ fclose(binaryFile); };

		char pathBuffer[MAX_PATH + 1];

		size_t shaderLen = 0;
		fread(&shaderLen, sizeof(size_t), 1, binaryFile);
		if (shaderLen == 0)
		{
			shader = Shader::MakeNewPassThrough();
		}
		else
		{
			if (shaderLen > MAX_PATH)
			{
				Error("Material: ( %s )\nInvalid Shader file path length.", filePath.c_str());
				return false;
			}
			fread(pathBuffer, sizeof(char), shaderLen, binaryFile);
			pathBuffer[shaderLen] = '\0';

			shader = Jwl::Load<Shader>(pathBuffer);
			if (!shader)
			{
				Error("Material: ( %s )\nFailed to load Shader ( %s ).", filePath.c_str(), pathBuffer);
				return false;
			}
		}

		size_t textureCount = 0;
		fread(&textureCount, sizeof(size_t), 1, binaryFile);
		if (textureCount > GPUInfo.GetMaxTextureSlots())
		{
			Error("Material: ( %s )\nMaterial contains more texture units than is supported ( %d ).", filePath.c_str(), GPUInfo.GetMaxTextureSlots());
			return false;
		}

		for (size_t i = 0; i < textureCount; ++i)
		{
			int unit = 0;
			fread(&unit, sizeof(int), 1, binaryFile);

			size_t textureLen = 0;
			fread(&textureLen, sizeof(size_t), 1, binaryFile);
			if (textureLen == 0 || textureLen > MAX_PATH)
			{
				Error("Material: ( %s )\nInvalid Texture file path length.", filePath.c_str());
				return false;
			}

			fread(pathBuffer, sizeof(char), textureLen, binaryFile);
			pathBuffer[textureLen] = '\0';

			auto texture = Jwl::Load<Texture>(pathBuffer);
			if (!texture)
			{
				Error("Material: ( %s )\nFailed to load Texture ( %s ).", filePath.c_str(), pathBuffer);
				return false;
			}

			textures.Add(std::move(texture), unit);
		}

		fread(&blendMode, sizeof(BlendFunc), 1, binaryFile);
		fread(&depthMode, sizeof(DepthFunc), 1, binaryFile);
		fread(&cullMode, sizeof(CullFunc), 1, binaryFile);

		return true;
	}
}
