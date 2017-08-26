// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "RenderPass.h"
#include "Camera.h"
#include "Material.h"
#include "Primitives.h"
#include "RenderTarget.h"
#include "Rendering.h"
#include "Viewport.h"
#include "Jewel3D/Application/Application.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Entity/Entity.h"
#include "Jewel3D/Entity/EntityGroup.h"
#include "Jewel3D/Math/Transform.h"
#include "Jewel3D/Resource/Font.h"
#include "Jewel3D/Resource/Model.h"
#include "Jewel3D/Resource/Shader.h"
#include "Jewel3D/Resource/Texture.h"
#include "Jewel3D/Resource/UniformBuffer.h"
// Renderables
#include "Mesh.h"
#include "ParticleEmitter.h"
#include "Sprite.h"
#include "Text.h"

#include <GLEW/GL/glew.h>

namespace Jwl
{
	RenderPass::RenderPass()
	{
		CreateUniformBuffer();
		CreateUniformHandles();
	}

	RenderPass::RenderPass(const RenderPass& other)
	{
		CreateUniformBuffer();
		CreateUniformHandles();

		*this = other;
	}

	RenderPass& RenderPass::operator=(const RenderPass& other)
	{
		viewport = other.viewport;
		camera = other.camera;
		target = other.target;
		shader = other.shader;
		skybox = other.skybox;

		return *this;
	}

	void RenderPass::SetCamera(Entity::Ptr cam)
	{
		ASSERT(cam, "'cam' cannot be a nullptr.");
		ASSERT(cam->Has<Camera>(), "'cam' must have a camera component.");

		camera = cam;
	}

	void RenderPass::SetShader(Shader::Ptr program)
	{
		shader = program;
	}

	void RenderPass::SetTarget(RenderTarget::Ptr _target)
	{
		target = _target;
	}

	void RenderPass::SetViewport(const Viewport& vp)
	{
		viewport = &vp;
	}

	void RenderPass::SetSkybox(Texture::Ptr sky)
	{
		ASSERT(sky->IsCubeMap(), "'sky' must be a cubemap texture.");

		skybox = sky;
	}

	Entity::Ptr RenderPass::GetCamera() const
	{
		return camera;
	}

	Shader::Ptr RenderPass::GetShader() const
	{
		return shader;
	}

	RenderTarget::Ptr RenderPass::GetTarget() const
	{
		return target;
	}

	const Viewport* RenderPass::GetViewport() const
	{
		return viewport;
	}

	Texture::Ptr RenderPass::GetSkybox() const
	{
		return skybox;
	}

	void RenderPass::Bind()
	{
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

		if (camera)
		{
			camera->Get<Camera>().Bind();
		}

		textures.Bind();
		buffers.Bind();
	}

	void RenderPass::UnBind()
	{
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

		if (camera != nullptr)
		{
			Camera::UnBind();
		}
	}

	void RenderPass::PostProcess()
	{
		ASSERT(shader != nullptr, "RenderPass must have a Shader attached if it is a post-processing pass.");
		ASSERT(Primitives.IsLoaded(), "Primitives system must be initialized in order to render a post-processing pass.");

		Bind();

		SetBlendFunc(BlendFunc::None);
		SetDepthFunc(DepthFunc::None);

		MVP.Set(mat4::Identity);
		modelView.Set(mat4::Identity);
		model.Set(mat4::Identity);
		invModel.Set(mat4::Identity);
		transformBuffer.Bind(static_cast<unsigned>(UniformBufferSlot::Model));

		Primitives.DrawFullScreenQuad(*shader);

		if (skybox)
		{
			Primitives.DrawSkyBox(*skybox);
		}

		UnBind();
	}

	void RenderPass::Render(const Entity& root)
	{
		Bind();

		RenderEntityRecursive(root);

		if (skybox)
		{
			Primitives.DrawSkyBox(*skybox);
		}

		UnBind();
	}

	void RenderPass::Render(const EntityGroup& group)
	{
		Bind();

		for (auto& entity : group.GetEntities())
		{
			RenderEntity(*entity);
		}

		if (skybox)
		{
			Primitives.DrawSkyBox(*skybox);
		}

		UnBind();
	}

	void RenderPass::RenderEntity(const Entity& ent)
	{
		if (!ent.IsEnabled())
		{
			return;
		}

		auto material = ent.Try<Material>();
		if (!material || !material->IsEnabled())
		{
			return;
		}

		auto mesh = ent.Try<Mesh>();
		auto text = ent.Try<Text>();
		auto emitter = ent.Try<ParticleEmitter>();
		auto sprite = ent.Try<Sprite>();
		ASSERT(mesh || text || emitter || sprite, "Entity must have a renderable component.");

		// Bind override shader.
		if (shader)
		{
			material->BindState();
			shader->Bind();
		}
		else
		{
			ASSERT(material->shader != nullptr, "Renderable Entity does not have a Shader and the RenderPass does not have an override attached.");
			material->Bind();
		}

		// Update transform uniforms.
		mat4 worldTransform = ent.GetWorldTransform();

		if (camera)
		{
			auto& cameraComponent = camera->Get<Camera>();

			MVP.Set(cameraComponent.GetViewProjMatrix() * worldTransform);
			modelView.Set(cameraComponent.GetViewMatrix() * worldTransform);
		}
		else
		{
			MVP.Set(mat4::Identity);
			modelView.Set(mat4::Identity);
		}

		model.Set(worldTransform);
		invModel.Set(worldTransform.GetFastInverse());
		transformBuffer.Bind(static_cast<unsigned>(UniformBufferSlot::Model));

#pragma region Render Model
		if (mesh && mesh->IsComponentEnabled())
		{
			// Capture pointer to current mesh data.
			auto modelData = mesh->model;
			ASSERT(modelData, "Entity has a Mesh component but does not have a Model to render.");

			glBindVertexArray(modelData->GetVAO());
			glDrawArrays(GL_TRIANGLES, 0, modelData->GetNumVerticies());
		}
#pragma endregion

#pragma region Render Text
		if (text && text->IsComponentEnabled())
		{
			auto font = text->font;
			ASSERT(font != nullptr, "Entity has a Text component but does not have a Font to render with.");

			auto dimensions = font->GetDimensions();
			auto positions = font->GetPositions();
			auto advances = font->GetAdvances();
			auto masks = font->GetMasks();

			// We have to send the vertices of each character we render. We'll store them here.
			float points[18] =
			{
				0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f,

				0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f,
			};

			const vec3 advanceDirection = text->owner.GetWorldTransform().GetRight();
			const vec3 upDirection = text->owner.GetWorldTransform().GetUp();
			const vec3 initialPosition = text->owner.position;
			vec3 linePosition = text->owner.position;
			unsigned currentLine = 1;

			if (text->centeredX)
			{
				text->owner.position -= advanceDirection * (((text->GetLineWidth(currentLine) + text->kernel * text->text.size())) / 2.0f);
			}

			if (text->centeredY)
			{
				text->owner.position -= upDirection * ((font->GetStringHeight() * static_cast<float>(text->GetNumLines())) / 2.0f);
			}

			glBindVertexArray(Font::GetVAO());
			glBindBuffer(GL_ARRAY_BUFFER, Font::GetVBO());

			for (unsigned i = 0; i < text->text.size(); i++)
			{
				char character = text->text[i];
				unsigned charIndex = static_cast<unsigned>(character) - '!';

				// Handle whitespace.
				if (character == ' ')
				{
					text->owner.position += advanceDirection * (font->GetStringWidth("Z") + text->kernel);
					continue;
				}
				else if (character == '\n')
				{
					linePosition += -upDirection * static_cast<float>(font->GetStringHeight()) * 1.33f;
					text->owner.position = linePosition;
					currentLine++;

					if (text->centeredX)
					{
						text->owner.position -= advanceDirection * (((text->GetLineWidth(currentLine) + text->kernel * text->text.size())) / 2.0f);
					}

					continue;
				}
				else if (character == '\t')
				{
					text->owner.position += advanceDirection * (font->GetStringWidth("Z") + text->kernel) * 4;
					continue;
				}

				if (!masks[charIndex])
				{
					// Character does not exist in this font. Advance to next character.
					text->owner.position += advanceDirection * ((advances[charIndex].x + text->kernel));
					continue;
				}

				/* Adjusts the node's position based on the character. */
				vec3 characterPosition;
				characterPosition += advanceDirection * static_cast<float>(positions[charIndex].x);
				characterPosition += upDirection * static_cast<float>(positions[charIndex].y);
				text->owner.position += characterPosition;

				/* Construct a polygon based on the current character's dimensions. */
				points[3] = static_cast<float>(dimensions[charIndex].x);
				points[7] = static_cast<float>(dimensions[charIndex].y);
				points[9] = static_cast<float>(dimensions[charIndex].x);
				points[12] = static_cast<float>(dimensions[charIndex].x);
				points[13] = static_cast<float>(dimensions[charIndex].y);
				points[16] = static_cast<float>(dimensions[charIndex].y);

				/* Update buffers with the new polygon. */
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 18, points);

				/* Render */
				// TODO: these sends' are duplicated. get rid of them!
				if (camera)
				{
					auto& cameraComponent = camera->Get<Camera>();

					MVP.Set(cameraComponent.GetViewProjMatrix() * text->owner.GetWorldTransform());
					modelView.Set(cameraComponent.GetViewMatrix() * text->owner.GetWorldTransform());
				}
				else
				{
					MVP.Set(mat4::Identity);
					modelView.Set(mat4::Identity);
				}
				model.Set(text->owner.GetWorldTransform());
				invModel.Set(text->owner.GetWorldTransform().GetFastInverse());
				transformBuffer.Bind(static_cast<unsigned>(UniformBufferSlot::Model));

				glBindTexture(GL_TEXTURE_2D, font->GetTextures()[charIndex]);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				/* Adjust position for the next node. */
				// Undo character translate.
				text->owner.position -= characterPosition;
				// Advance to next character.
				text->owner.position += advanceDirection * ((advances[charIndex].x + text->kernel));
			}

			glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

			text->owner.position = initialPosition;
		}
#pragma endregion

#pragma region Render Particles
		if (emitter && emitter->IsComponentEnabled() && emitter->GetNumAliveParticles() > 0)
		{
			emitter->GetBuffer().Bind(static_cast<unsigned>(UniformBufferSlot::Particle));

			glBindVertexArray(emitter->GetVAO());
			glDrawArrays(GL_POINTS, 0, emitter->GetNumAliveParticles());
		}
#pragma endregion

#pragma region Render Sprite
		if (sprite && sprite->IsComponentEnabled())
		{
			ASSERT(Primitives.IsLoaded(), "Primitives system must be initialized in order to render sprites.");

			Primitives.DrawUnitRectangle();
		}
#pragma endregion

		material->UnBind();
	}

	void RenderPass::RenderEntityRecursive(const Entity& ent)
	{
		RenderEntity(ent);

		for (auto& child : ent.GetChildren())
		{
			RenderEntityRecursive(*child);
		}
	}

	void RenderPass::CreateUniformBuffer()
	{
		transformBuffer.AddUniform("MVP", sizeof(mat4));
		transformBuffer.AddUniform("ModelView", sizeof(mat4));
		transformBuffer.AddUniform("Model", sizeof(mat4));
		transformBuffer.AddUniform("InvModel", sizeof(mat4));
		transformBuffer.InitBuffer();
	}

	void RenderPass::CreateUniformHandles()
	{
		MVP = transformBuffer.MakeHandle<mat4>("MVP");
		modelView = transformBuffer.MakeHandle<mat4>("ModelView");
		model = transformBuffer.MakeHandle<mat4>("Model");
		invModel = transformBuffer.MakeHandle<mat4>("InvModel");
	}
}
