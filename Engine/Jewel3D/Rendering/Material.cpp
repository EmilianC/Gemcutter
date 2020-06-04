// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Material.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Rendering/Rendering.h"
#include "Jewel3D/Resource/Texture.h"

namespace Jwl
{
	Shader::WeakPtr Material::passThroughShader;

	Material::Material(Entity& _owner)
		: Component(_owner)
	{
		if (auto lock = passThroughShader.lock())
		{
			shader = std::move(lock);
		}
		else
		{
			shader = Shader::MakeNewPassThrough();
			passThroughShader = shader;
		}
	}

	Material::Material(Entity& _owner, Shader::Ptr _shader)
		: Component(_owner)
		, shader(std::move(_shader))
	{
	}

	Material::Material(Entity& _owner, Texture::Ptr texture)
		: Material(_owner)
	{
		if (texture)
		{
			textures.Add(std::move(texture));
		}
	}

	Material::Material(
		Entity& _owner, Shader::Ptr _shader, Texture::Ptr texture,
		BlendFunc _blendMode, DepthFunc _depthMode, CullFunc _cullMode)
		: Component(_owner)
		, shader(std::move(_shader))
	{
		if (texture)
		{
			textures.Add(std::move(texture));
		}

		SetBlendMode(_blendMode);
		SetDepthMode(_depthMode);
		SetCullMode(_cullMode);
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
		for (const auto& bufferSlot : other.buffers.GetAll())
		{
			auto newBuff = UniformBuffer::MakeNew();
			newBuff->Copy(*bufferSlot.buffer);

			buffers.Add(newBuff, bufferSlot.unit);
		}

		return *this;
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
