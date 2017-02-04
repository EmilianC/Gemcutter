// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "RenderTarget.h"
#include "Jewel3D/Resource/Shader.h"
#include "Jewel3D/Resource/Texture.h"

namespace Jwl
{
	class Camera;
	class Entity;
	class EntityGroup;
	class Viewport;

	//- Consolidates the three main components for rendering: input Geometry, shader pipeline and render target.
	class RenderPass
	{
	public:
		RenderPass();
		RenderPass(const RenderPass&);

		RenderPass& operator=(const RenderPass&);

		//- Camera is optional. Provided SceneNode must have a camera component.
		void SetCamera(Entity::Ptr cam);
		//- The program that will be used if the render call is a post process, or if a renderable does not have a material.
		void SetShader(Shader::Ptr program);
		//- If no explicit target is set, the render pass will target the backbuffer.
		void SetTarget(RenderTarget::Ptr target);
		//- If no explicit viewport is set, the viewport will match the render target or back buffer.
		void SetViewport(const Viewport& vp);
		//- Rendered after all the group objects attached. Ignored for post process passes.
		void SetSkybox(Texture::Ptr sky);

		Entity::Ptr GetCamera() const;
		Shader::Ptr GetShader() const;
		RenderTarget::Ptr GetTarget() const;
		const Viewport* GetViewport() const;
		Texture::Ptr GetSkybox() const;

		//- Renders a fullscreen quad.
		void PostProcess();
		//- Traverses the root Entity and renders all renderable children.
		void Render(const Entity& root);
		//- Renders every Entity included in the group.
		void Render(const EntityGroup& group);

		//- These textures will be bound during the execution of the render pass.
		TextureList textures;
		//- These buffers will be bound during the execution of the render pass.
		BufferList buffers;

	private:
		void Bind();
		void UnBind();

		void RenderEntity(const Entity& ent);
		void RenderEntityRecursive(const Entity& ent);

		void CreateUniformBuffer();
		void CreateUniformHandles();

		const Viewport* viewport = nullptr;
		Entity::Ptr camera;
		RenderTarget::Ptr target;
		Shader::Ptr shader;
		Texture::Ptr skybox;

		//- Holds the world transformation matrices for an entity while rendering.
		UniformBuffer transformBuffer;

		UniformHandle<mat4> MVP;
		UniformHandle<mat4> modelView;
		UniformHandle<mat4> model;
		UniformHandle<mat4> invModel;
	};
}
