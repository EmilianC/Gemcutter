// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Entity/Entity.h"
#include "gemcutter/Rendering/RenderTarget.h"
#include "gemcutter/Rendering/Viewport.h"
#include "gemcutter/Resource/Shader.h"
#include "gemcutter/Resource/Texture.h"

#include <optional>
#include <span>

namespace gem
{
	// Ties together the three requirements for rendering: geometry, shaders, and a render target.
	class RenderPass
	{
	public:
		RenderPass();
		RenderPass(const RenderPass&);

		RenderPass& operator=(const RenderPass&);

		// Provided Entity must have a camera component.
		void SetCamera(Entity::Ptr camera);
		// Provides a shader to be used with PostProcess(), or to override Entity materials.
		void SetShader(Shader::Ptr shader);
		// If no explicit target is set, the back-buffer will be targeted.
		void SetTarget(RenderTarget::Ptr target);
		// If no explicit viewport is set, the viewport will match the render target or back-buffer.
		void SetViewport(std::optional<Viewport> vp);

		const auto& GetCamera() const { return camera; }
		const auto& GetShader() const { return shader; }
		const auto& GetTarget() const { return target; }
		const auto& GetViewport() const { return viewport; }

		void Bind();
		void UnBind();

		// Renders a fullscreen quad.
		void PostProcess();
		// Renders only the given Entity.
		void Render(const Entity&);
		// Renders all Entities in the list in order.
		void Render(std::span<Entity::Ptr> entities);
		// Renders the root Entity along with all renderable descendants (depth first traversal).
		void RenderRoot(const Entity& root);
		// Renders 'count' copies of the instance.
		// Gem_MVP, Gem_ModelView, Gem_Model, Gem_InvModel, and Gem_NormalToWorld shader uniforms will not be set.
		void RenderInstanced(const Entity& instance, unsigned count);

		// These textures will be bound along with the RenderPass.
		TextureList textures;
		// These buffers will be bound along with the RenderPass.
		BufferList buffers;

	private:
		void CreateUniformBuffer();

		std::optional<Viewport> viewport;
		Entity::Ptr camera;
		RenderTarget::Ptr target;
		Shader::Ptr shader;

		// Holds the world transformation matrices for an Entity while rendering.
		UniformBuffer transformBuffer;

		UniformHandle<mat4> MVP;
		UniformHandle<mat4> modelView;
		UniformHandle<mat4> model;
		UniformHandle<mat4> invModel;
		UniformHandle<mat3> normalMatrix;

		static inline RenderPass* boundPass = nullptr;
	};
}
