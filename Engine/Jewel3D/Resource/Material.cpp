// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Material.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Utilities/ScopeGuard.h"
#include "Jewel3D/Utilities/String.h"

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

		// Create instance copies of any non-static uniform buffers.
		for (const BufferBinding& binding : shader->GetBufferBindings())
		{
			if (binding.templateBuff)
			{
				auto newBuffer = UniformBuffer::MakeNew();
				newBuffer->Copy(*binding.templateBuff);

				buffers.Add(std::move(newBuffer), binding.unit);
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

		BlendFunc blend;
		fread(&blend, sizeof(BlendFunc), 1, binaryFile);
		SetBlendMode(blend);

		DepthFunc depth;
		fread(&depth, sizeof(DepthFunc), 1, binaryFile);
		SetDepthMode(depth);

		CullFunc cull;
		fread(&cull, sizeof(CullFunc), 1, binaryFile);
		SetCullMode(cull);

		return true;
	}

	void Material::SetBlendMode(BlendFunc func)
	{
		blendMode = func;
	}

	void Material::SetDepthMode(DepthFunc func)
	{
		depthMode = func;
	}

	void Material::SetCullMode(CullFunc func)
	{
		cullMode = func;
	}

	BlendFunc Material::GetBlendMode() const
	{
		return blendMode;
	}

	DepthFunc Material::GetDepthMode() const
	{
		return depthMode;
	}

	CullFunc Material::GetCullMode() const
	{
		return cullMode;
	}

	void Material::Bind() const
	{
		if (shader)
		{
			shader->Bind(variantDefinitions);
		}

		BindState();
	}

	void Material::BindState() const
	{
		textures.Bind();
		buffers.Bind();

		SetBlendFunc(blendMode);
		SetDepthFunc(depthMode);
		SetCullFunc(cullMode);
	}

	void Material::UnBind() const
	{
		textures.UnBind();
		buffers.UnBind();

		if (shader)
		{
			shader->UnBind();
		}
	}
}
