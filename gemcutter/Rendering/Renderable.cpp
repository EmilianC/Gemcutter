// Copyright (c) 2020 Emilian Cioca
#include "Renderable.h"

namespace gem
{
	Renderable::Renderable(Entity& _owner)
		: Component(_owner)
	{
		material = Material::MakeNew();
		material->shader = Shader::MakeNewPassThrough();
	}

	Renderable::Renderable(Entity& _owner, Material::Ptr _material)
		: Component(_owner)
	{
		SetMaterial(std::move(_material));
	}

	Renderable::Renderable(Entity& _owner, VertexArray::Ptr _array)
		: Component(_owner)
		, array(std::move(_array))
	{
		material = Material::MakeNew();
		material->shader = Shader::MakeNewPassThrough();
	}

	Renderable::Renderable(Entity& _owner, VertexArray::Ptr _array, Material::Ptr _material)
		: Component(_owner)
		, array(std::move(_array))
	{
		SetMaterial(std::move(_material));
	}

	void Renderable::SetMaterial(Material::Ptr newMaterial)
	{
		ASSERT(newMaterial, "'newMaterial' cannot be null.");

		// Remove any instanced buffer bindings from the previous shader.
		if (material && material->shader)
		{
			for (const BufferBinding& binding : material->shader->GetBufferBindings())
			{
				if (binding.templateBuff)
				{
					buffers.Remove(binding.unit);
				}
			}
		}

		// Create any instanced buffer copies needed per-entity.
		for (const BufferBinding& binding : newMaterial->shader->GetBufferBindings())
		{
			if (binding.templateBuff)
			{
				auto newBuffer = UniformBuffer::MakeNew();
				newBuffer->Copy(*binding.templateBuff);

				buffers.Add(std::move(newBuffer), binding.unit);
			}
		}

		material = std::move(newMaterial);
	}

	const Material& Renderable::GetMaterial() const
	{
		return *material;
	}

	Material& Renderable::GetMaterial()
	{
		return *material;
	}
}

REFLECT_COMPONENT(gem::Renderable, gem::ComponentBase)
	MEMBERS {
		REF_PRIVATE_MEMBER(material, gem::ReadOnly)
	}
REF_END;
