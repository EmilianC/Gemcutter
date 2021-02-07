// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Rendering/RenderTarget.h"
#include "gemcutter/Rendering/Viewport.h"
#include "gemcutter/Resource/Shader.h"
#include "gemcutter/Resource/Texture.h"

#include <optional>

namespace gem
{
	class Entity;

	// Consolidates the three main components for rendering: input Geometry, shader pipeline and render target.
	class RenderPass
	{
	public:
		RenderPass();
		RenderPass(const RenderPass&);

		RenderPass& operator=(const RenderPass&);

		// Provided Entity must have a camera component.
		void SetCamera(Entity::Ptr camera);
		// The shader that will be used if the render call is a post process, or if a renderable does not have a material.
		void SetShader(Shader::Ptr shader);
		// If no explicit target is set, the render pass will target the backbuffer.
		void SetTarget(RenderTarget::Ptr target);
		// If no explicit viewport is set, the viewport will match the render target or back buffer.
		void SetViewport(std::optional<Viewport> vp);
		// Rendered after all the group objects attached. Ignored for post process passes.
		void SetSkybox(Texture::Ptr sky);

		const auto& GetCamera() const { return camera; }
		const auto& GetShader() const { return shader; }
		const auto& GetTarget() const { return target; }
		const auto& GetSkybox() const { return skybox; }
		const auto& GetViewport() const { return viewport; }

		// Renders a fullscreen quad.
		void PostProcess();
		// Traverses the root Entity and renders all renderable children.
		void Render(const Entity& root);
		// Renders all Entities in the list in order.
		void Render(const std::vector<Entity::Ptr>& entities);
		// Renders 'count' copies of the instance.
		// Gem_MVP, Gem_ModelView, Gem_Model, Gem_InvModel, and Gem_NormalToWorld shader uniforms will not be set.
		void RenderInstanced(const Entity& instance, unsigned count);

		// These textures will be bound during the execution of the render pass.
		TextureList textures;
		// These buffers will be bound during the execution of the render pass.
		BufferList buffers;

	private:
		void Bind();
		void UnBind();

		void RenderEntity(const Entity& ent);
		void RenderEntityRecursive(const Entity& ent);

		void CreateUniformBuffer();

		std::optional<Viewport> viewport;
		Entity::Ptr camera;
		RenderTarget::Ptr target;
		Shader::Ptr shader;
		Texture::Ptr skybox;

		// Holds the world transformation matrices for an entity while rendering.
		UniformBuffer transformBuffer;

		UniformHandle<mat4> MVP;
		UniformHandle<mat4> modelView;
		UniformHandle<mat4> model;
		UniformHandle<mat4> invModel;
		UniformHandle<mat3> normalMatrix;
	};
}
