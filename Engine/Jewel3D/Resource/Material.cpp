// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Material.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Rendering/Rendering.h"
#include "Jewel3D/Resource/Texture.h"

namespace Jwl
{
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

	void Material::CreateUniformBuffer(unsigned unit)
	{
		ASSERT(shader, "Must have a Shader attached.");

		buffers.Add(shader->CreateBufferFromTemplate(unit), unit);
	}

	void Material::CreateUniformBuffers()
	{
		ASSERT(shader, "Must have a Shader attached.");

		for (const auto& binding : shader->GetBufferBindings())
		{
			if (binding.templateBuff)
			{
				CreateUniformBuffer(binding.unit);
			}
		}
	}

	bool Material::Load(std::string filePath)
	{

		return true;
	}

	void Material::Unload()
	{

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
}
