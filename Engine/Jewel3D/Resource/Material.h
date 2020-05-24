// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Rendering/Rendering.h"
#include "Jewel3D/Resource/Resource.h"
#include "Jewel3D/Resource/Shader.h"
#include "Jewel3D/Resource/Texture.h"
#include "Jewel3D/Resource/UniformBuffer.h"

namespace Jwl
{
	// When rendering an Entity, the Material specifies:
	// - Textures
	// - Blend / Cull / Depth modes
	// - The shader
	// - Shader uniform buffers
	// - Shader definitions and permutations
	class Material : public Resource<Material>, public Shareable<Material>
	{
		friend class RenderPass;
	public:
		Material() = default;

		bool Load(std::string filePath);

		void SetBlendMode(BlendFunc func);
		void SetDepthMode(DepthFunc func);
		void SetCullMode(CullFunc func);
		BlendFunc GetBlendMode() const;
		DepthFunc GetDepthMode() const;
		CullFunc GetCullMode() const;

		// The set of definitions used to permute the shader.
		ShaderVariantControl variantDefinitions;

		// Mirror the current shader's UniformBuffer bound to the specified index.
		// The specified buffer must be marked as a 'template' in the shader.
		void CreateUniformBuffer(unsigned unit);
		// Enumerates and mirrors all the current shader's 'template' UniformBuffers.
		void CreateUniformBuffers();

		// The shader used to render the entity.
		Shader::Ptr shader;
		// These textures will be bound whenever the material is used in rendering.
		TextureList textures;
		// These buffers provide the shader's uniform data.
		BufferList buffers;

	private:
		// Bind all textures, buffers, state, and the shader for rendering.
		void Bind() const;
		// Bind all textures, buffers, and state.
		void BindState() const;
		// Unbinds all textures, buffers, and the shader.
		void UnBind() const;

		BlendFunc blendMode = BlendFunc::None;
		DepthFunc depthMode = DepthFunc::Normal;
		CullFunc cullMode = CullFunc::Clockwise;
	};
}
