// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Entity/Entity.h"
#include "Jewel3D/Rendering/Rendering.h"
#include "Jewel3D/Resource/Shader.h"

namespace Jwl
{
	class Material : public Component<Material>
	{
	public:
		Material(Entity& owner);
		Material(Entity& owner, Shader::Ptr shader);
		Material(Entity& owner, Texture::Ptr texture);
		Material(Entity& owner, Shader::Ptr shader, Texture::Ptr texture);
		Material& operator=(const Material&);

		void SetBlendMode(BlendFunc func);
		void SetDepthMode(DepthFunc func);
		void SetCullMode(CullFunc func);
		BlendFunc GetBlendMode() const;
		DepthFunc GetDepthMode() const;
		CullFunc GetCullMode() const;

		//- The set of definitions used to permute the shader.
		ShaderVariantControl variantDefinitions;

		//- Bind all textures, buffers, state, and the shader for rendering. Used internally.
		void Bind();
		//- Bind all textures, buffers, and state. Used internally.
		void BindState();
		//- Unbinds all textures, buffers, and the shader. Used internally.
		void UnBind();

		//- Mirror the current shader's UniformBuffer bound to the specified index.
		//- The specified buffer must be marked as a 'template'.
		void CreateUniformBuffer(unsigned unit);
		//- Enumerates and mirrors all the current shader's 'template' UniformBuffers.
		void CreateUniformBuffers();

		//- The shader used to render the entity.
		Shader::Ptr shader;
		//- These textures will be bound whenever the material is used in rendering.
		TextureList textures;
		//- These buffers will be bound whenever the material is used in rendering.
		BufferList buffers;

	private:
		BlendFunc blendMode = BlendFunc::None;
		DepthFunc depthMode = DepthFunc::Normal;
		CullFunc cullMode = CullFunc::Clockwise;
	};
}
