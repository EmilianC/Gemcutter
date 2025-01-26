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

		// The provided Entity must have a camera component if it is not null.
		void SetCamera(Entity::WeakPtr camera);
		// Provides a shader to be used with PostProcess(), or to override Entity materials.
		void SetShader(Shader::Ptr shader);
		// If no explicit target is set, the back-buffer will be targeted.
		void SetTarget(RenderTarget::Ptr target);
		// If no explicit viewport is set, the viewport will match the render target or back-buffer.
		void SetViewport(std::optional<Viewport> vp);

		auto GetCamera() const { return camera.lock(); }
		const auto& GetShader() const { return shader; }
		const auto& GetTarget() const { return target; }
		const auto& GetViewport() const { return viewport; }

		// Prepares the pass for rendering and captures the current state of the camera.
		void Bind();
		void UnBind();

		// Renders a fullscreen quad.
		void PostProcess();
		// Renders only the given Entity.
		void Render(const Entity&);
		// Renders all Entities in the list in order.
		void Render(std::span<const Entity::Ptr> entities);
		// Renders the root Entity along with all renderable descendants (depth first traversal).
		void RenderRoot(const Entity& root);

		// These textures will be bound along with the RenderPass.
		TextureList textures;
		// These buffers will be bound along with the RenderPass.
		BufferList buffers;

	private:
		void CreateUniformBuffer();

		std::optional<Viewport> viewport;
		Entity::WeakPtr camera;
		RenderTarget::Ptr target;
		Shader::Ptr shader;

		// Holds the world transformation matrices for an Entity while rendering.
		UniformBuffer transformBuffer;

		mat4 viewMatrix;
		mat4 viewProjMatrix;

		UniformHandle<mat4> MVP;
		UniformHandle<mat4> modelView;
		UniformHandle<mat4> model;
		UniformHandle<mat4> invModel;
		UniformHandle<mat3> normalMatrix;

		static inline RenderPass* boundPass = nullptr;
	};
}
