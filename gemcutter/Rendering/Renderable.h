// Copyright (c) 2020 Emilian Cioca
#pragma once
#include "gemcutter/Entity/Entity.h"
#include "gemcutter/Resource/Material.h"

namespace Jwl
{
	// Base class for all renderable components.
	class Renderable : public Component<Renderable>
	{
	public:
		Renderable(Entity& owner);
		Renderable(Entity& owner, Material::Ptr material);

		void SetMaterial(Material::Ptr newMaterial);
		const Material::Ptr& GetMaterial() const;
		Material::Ptr& GetMaterial();

		// The set of definitions used to permute the shader.
		ShaderVariantControl variants;

		// Provide per-instance uniform data for the Material's shader.
		BufferList buffers;

	private:
		Material::Ptr material;
	};
}
