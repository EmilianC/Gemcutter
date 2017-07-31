// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Material.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Rendering/Rendering.h"
#include "Jewel3D/Resource/Texture.h"

namespace Jwl
{
	Material::Material(Entity& _owner)
		: Component(_owner)
	{
	}

	Material::Material(Entity& _owner, Shader::Ptr _shader)
		: Component(_owner)
		, shader(_shader)
	{
	}

	Material::Material(Entity& _owner, Texture::Ptr texture)
		: Component(_owner)
	{
		textures.Add(texture);
	}

	Material::Material(Entity& _owner, Shader::Ptr _shader, Texture::Ptr texture)
		: Component(_owner)
		, shader(_shader)
	{
		textures.Add(texture);
	}

	Material& Material::operator=(const Material& other)
	{
		shader = other.shader;
		variantDefinitions = other.variantDefinitions;
		SetDepthMode(other.GetDepthMode());
		SetBlendMode(other.GetBlendMode());
		SetCullMode(other.GetCullMode());
		textures = other.textures;

		// Deep copy the buffers.
		buffers.Clear();
		for (auto bufferSlot : other.buffers.GetAll())
		{
			auto newBuff = UniformBuffer::MakeNew();
			newBuff->Copy(*bufferSlot.buffer);

			buffers.Add(newBuff, bufferSlot.unit);
		}

		return *this;
	}

	void Material::Bind()
	{
		if (shader)
		{
			shader->Bind(variantDefinitions);
		}

		BindState();
	}

	void Material::BindState()
	{
		textures.Bind();
		buffers.Bind();
		
		SetBlendFunc(blendMode);
		SetDepthFunc(depthMode);
		SetCullFunc(cullMode);
	}

	void Material::UnBind()
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

		const auto& bindings = shader->GetBufferBindings();

		for (unsigned i = 0; i < bindings.size(); i++)
		{
			if (bindings[i].templateBuff)
			{
				CreateUniformBuffer(bindings[i].unit);
			}
		}
	}

	void Material::SetBlendMode(BlendFunc func)
	{
		if (func == BlendFunc::CutOut && blendMode != BlendFunc::CutOut)
		{
			variantDefinitions.Define("JWL_CUTOUT");
		}
		else if (func != BlendFunc::CutOut && blendMode == BlendFunc::CutOut)
		{
			variantDefinitions.Undefine("JWL_CUTOUT");
		}

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
