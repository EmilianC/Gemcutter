// Copyright (c) 2017 Emilian Cioca
#include "RenderPass.h"
#include "gemcutter/Application/Application.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Entity/Entity.h"
#include "gemcutter/Entity/Hierarchy.h"
#include "gemcutter/Math/Transform.h"
#include "gemcutter/Rendering/Camera.h"
#include "gemcutter/Rendering/Primitives.h"
#include "gemcutter/Rendering/Renderable.h"
#include "gemcutter/Rendering/Rendering.h"
#include "gemcutter/Rendering/RenderTarget.h"
#include "gemcutter/Rendering/Viewport.h"
#include "gemcutter/Resource/Font.h"
#include "gemcutter/Resource/Material.h"
#include "gemcutter/Resource/Shader.h"
#include "gemcutter/Resource/Texture.h"
#include "gemcutter/Resource/UniformBuffer.h"
#include "gemcutter/Resource/VertexArray.h"
#include "gemcutter/Utilities/StdExt.h"

#include <GL/glew.h>

namespace
{
	void BindRenderable(const gem::Renderable& renderable, gem::Shader* overrideShader)
	{
		const gem::Material& material = renderable.GetMaterial();

		if (overrideShader)
		{
			overrideShader->Bind();
		}
		else
		{
			ASSERT(material.shader, "Renderable Entity does not have a Shader and the RenderPass does not have an override attached.");

			material.shader->Bind(renderable.variants);
		}

		if (renderable.array)
		{
			renderable.array->Bind();
		}

		material.textures.Bind();
		renderable.buffers.Bind();
		renderable.textures.Bind();
		gem::SetBlendFunc(material.blendMode);
		gem::SetDepthFunc(material.depthMode);
		gem::SetCullFunc(material.cullMode);
	}

	void UnBindRenderable(const gem::Renderable& renderable, gem::Shader* overrideShader)
	{
		const gem::Material& material = renderable.GetMaterial();

		if (overrideShader)
		{
			overrideShader->UnBind();
		}
		else
		{
			material.shader->UnBind();
		}

		if (renderable.array)
		{
			renderable.array->UnBind();
		}

		renderable.textures.UnBind();
		renderable.buffers.UnBind();
		material.textures.UnBind();
	}
}

namespace gem
{
	RenderPass::RenderPass()
	{
		CreateUniformBuffer();
	}

	RenderPass::RenderPass(const RenderPass& other)
	{
		CreateUniformBuffer();
		*this = other;
	}

	RenderPass& RenderPass::operator=(const RenderPass& other)
	{
		if (this == &other)
			return *this;

		viewport = other.viewport;
		camera = other.camera;
		target = other.target;
		shader = other.shader;

		return *this;
	}

	void RenderPass::SetCamera(Entity::WeakPtr _camera)
	{
		ASSERT(_camera.expired() || _camera.lock()->Has<Camera>(), "'camera' must have a camera component.");

		camera = std::move(_camera);
	}

	void RenderPass::SetShader(Shader::Ptr _shader)
	{
		shader = std::move(_shader);
	}

	void RenderPass::SetTarget(RenderTarget::Ptr _target)
	{
		target = std::move(_target);
	}

	void RenderPass::SetViewport(std::optional<Viewport> vp)
	{
		viewport = vp;
	}

	void RenderPass::Bind()
	{
		if (boundPass && boundPass != this)
		{
			boundPass->UnBind();
		}

		if (target)
		{
			target->Bind();
		}

		if (viewport)
		{
			viewport->bind();
		}
		else if (target)
		{
			target->GetViewport().bind();
		}
		else
		{
			Application.GetScreenViewport().bind();
		}

		if (Entity::Ptr cameraLock = camera.lock())
		{
			auto& cameraComponent = cameraLock->Get<Camera>();
			cameraComponent.Bind();

			viewMatrix = cameraComponent.GetViewMatrix();
			viewProjMatrix = cameraComponent.GetProjMatrix();
		}

		boundPass = this;
	}

	void RenderPass::UnBind()
	{
		ASSERT(boundPass == this, "RenderPass cannot be unbound if it is not bound.");

		glBindVertexArray(GL_NONE);

		// UnBind override shader.
		if (shader)
		{
			shader->UnBind();
		}

		textures.UnBind();
		buffers.UnBind();

		if (target)
		{
			RenderTarget::UnBind();
		}

		if (!IsPtrNull(camera))
		{
			Camera::UnBind();
		}

		boundPass = nullptr;
	}

	void RenderPass::PostProcess()
	{
		ASSERT(boundPass == this, "RenderPass must be bound to render.");
		ASSERT(shader != nullptr, "RenderPass must have a Shader attached if it is a post-processing pass.");
		ASSERT(Primitives.IsLoaded(), "Primitives system must be initialized in order to render a post-processing pass.");

		textures.Bind();
		buffers.Bind();

		SetBlendFunc(BlendFunc::None);
		SetDepthFunc(DepthFunc::None);

		MVP.Set(mat4::Identity);
		modelView.Set(mat4::Identity);
		model.Set(mat4::Identity);
		invModel.Set(mat4::Identity);
		normalMatrix.Set(mat3::Identity);
		transformBuffer.Bind(static_cast<unsigned>(UniformBufferSlot::Model));

		Primitives.DrawFullScreenQuad(*shader);
	}

	void RenderPass::Render(const Entity& ent)
	{
		ASSERT(boundPass == this, "RenderPass must be bound to render.");

		if (!ent.IsEnabled())
		{
			return;
		}

		auto* renderable = ent.Try<Renderable>();
		if (!renderable || !renderable->IsEnabled())
		{
			return;
		}

		// These must be re-bound for each renderable in case
		// they had been overridden by a previous entity.
		textures.Bind();
		buffers.Bind();
		BindRenderable(*renderable, shader.get());

		// Update transform uniforms.
		const mat4 worldTransform = ent.GetWorldTransform();

		if (!IsPtrNull(camera))
		{
			const mat4 mv = viewMatrix * worldTransform;

			MVP.Set(viewProjMatrix * mv);
			modelView.Set(mv);
		}
		else
		{
			MVP.Set(mat4::Identity);
			modelView.Set(mat4::Identity);
		}

		model.Set(worldTransform);
		invModel.Set(worldTransform.GetFastInverse());
		normalMatrix.Set(mat3(worldTransform).GetInverse().GetTranspose());

		transformBuffer.Bind(static_cast<unsigned>(UniformBufferSlot::Model));

		auto* vertexArray = renderable->array.get();
		ASSERT(vertexArray, "Renderable Entity does not have a valid VertexArray to render.");

		vertexArray->Draw();

		UnBindRenderable(*renderable, shader.get());
	}

	void RenderPass::Render(std::span<const Entity::Ptr> entities)
	{
		ASSERT(boundPass == this, "RenderPass must be bound to render.");

		for (auto& entity : entities)
		{
			ASSERT(entity, "Entity pointer cannot be null.");
			Render(*entity);
		}
	}

	void RenderPass::RenderRoot(const Entity& root)
	{
		ASSERT(boundPass == this, "RenderPass must be bound to render.");

		Render(root);

		if (auto* hierarchy = root.Try<Hierarchy>())
		{
			for (auto& child : hierarchy->GetChildren())
			{
				RenderRoot(*child);
			}
		}
	}

	void RenderPass::CreateUniformBuffer()
	{
		MVP = transformBuffer.AddUniform<mat4>("MVP");
		modelView = transformBuffer.AddUniform<mat4>("ModelView");
		model = transformBuffer.AddUniform<mat4>("Model");
		invModel = transformBuffer.AddUniform<mat4>("InvModel");
		normalMatrix = transformBuffer.AddUniform<mat3>("Normal");

		transformBuffer.InitBuffer(BufferUsage::Dynamic);
	}
}
