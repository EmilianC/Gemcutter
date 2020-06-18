// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Rendering/Rendering.h"
#include "Jewel3D/Resource/Resource.h"
#include "Jewel3D/Resource/Shader.h"
#include "Jewel3D/Resource/Texture.h"

namespace Jwl
{
	// When rendering an Entity, the Material specifies:
	// - Textures
	// - Blend / Cull / Depth modes
	// - The shader
	// - Static uniform buffers
	class Material : public Resource<Material>, public Shareable<Material>
	{
	public:
		Material() = default;

		bool Load(std::string filePath);

		BlendFunc blendMode = BlendFunc::None;
		DepthFunc depthMode = DepthFunc::Normal;
		CullFunc cullMode = CullFunc::Clockwise;

		// The shader used to render the entity.
		Shader::Ptr shader;
		// These textures will be bound whenever the material is used in rendering.
		TextureList textures;
	};
}
