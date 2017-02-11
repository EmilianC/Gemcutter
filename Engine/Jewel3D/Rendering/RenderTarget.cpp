// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "RenderTarget.h"
#include "Jewel3D/Application/Application.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Math/Vector.h"
#include "Jewel3D/Resource/Texture.h"
#include "Jewel3D/Utilities/ScopeGuard.h"

#include <GLEW/GL/glew.h>

namespace Jwl
{
	RenderTarget::~RenderTarget()
	{
		Unload();
	}

	void RenderTarget::Init(unsigned pixelWidth, unsigned pixelHeight, unsigned numColorTextures, bool hasDepth, unsigned samples)
	{
		ASSERT(FBO == GL_NONE, "RenderTarget is already initialized. Must call Unload() before initializing it again.");
		ASSERT(pixelWidth != 0, "'pixelWidth' must be greater than 0.");
		ASSERT(pixelHeight != 0, "'pixelHeight' must be greater than 0.");
		ASSERT(samples == 1 || samples == 2 || samples == 4 || samples == 8 || samples == 16, "'samples' must be a power of 2 between 1 and 16.");

		width = pixelWidth;
		height = pixelHeight;
		numColorAttachments = numColorTextures;
		numSamples = samples;

		glGenFramebuffers(1, &FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		if (hasDepth)
		{
			depthAttachment = Texture::MakeNew();
			depthAttachment->CreateTexture(
				width, height,
				TextureFormat::DEPTH_24, TextureFilterMode::Point,
				numSamples);
			
			glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				GL_DEPTH_ATTACHMENT,
				numSamples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D,
				depthAttachment->GetHandle(),
				0);
		}

		if (numColorAttachments > 0)
		{
			if (numColorAttachments > GPUInfo.GetMaxRenderTargetAttachments() ||
				numColorAttachments > GPUInfo.GetMaxDrawBuffers())
			{
				Error("RenderTarget: Number of color attachments exceeds amount supported by the OpenGL drivers.");
				glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
				return;
			}

			colorAttachments = new Texture::Ptr[numColorAttachments];

			// Enable all color attachments as write-able targets.
			GLenum* bufs = reinterpret_cast<GLenum*>(malloc(sizeof(GLenum) * numColorAttachments));
			for (unsigned i = 0; i < numColorAttachments; i++)
			{
				bufs[i] = GL_COLOR_ATTACHMENT0 + i;
				colorAttachments[i] = Texture::MakeNew();
			}

			glDrawBuffers(numColorAttachments, bufs);

			free(bufs);
		}
		else
		{
			// No color targets.
			glDrawBuffers(0, GL_NONE);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	bool RenderTarget::InitAsResolve(const RenderTarget& multisampleBuffer, TextureFilterMode filter)
	{
		ASSERT(FBO == GL_NONE, "RenderTarget is already initialized. Must call Unload() before initializing it again.");
		ASSERT(multisampleBuffer.numSamples != 1, "'multisampleBuffer' must have a sample count above 1 in order to create a resolve buffer.");

		Init(multisampleBuffer.width, multisampleBuffer.height, multisampleBuffer.numColorAttachments, multisampleBuffer.HasDepth());

		// Mirror all color attachments, but with just one sample.
		for (unsigned i = 0; i < multisampleBuffer.numColorAttachments; i++)
		{
			CreateAttachment(i, multisampleBuffer.GetColorTexture(i)->GetTextureFormat(), filter);
		}

		return Validate();
	}

	void RenderTarget::CreateAttachment(unsigned index, TextureFormat format, TextureFilterMode filter)
	{
		ASSERT(FBO != GL_NONE, "Must call Init() or InitAsResolve() before the RenderTarget can be used.");
		ASSERT(numColorAttachments > 0, "RenderTarget does not have any color attachments.");
		ASSERT(index < numColorAttachments, "'index' must specify a valid color attachment.");
		ASSERT(format != TextureFormat::DEPTH_24, "'format' cannot be DEPTH_24 for a color attachment.");

		colorAttachments[index] = Texture::MakeNew();
		colorAttachments[index]->CreateTexture(width, height, format, filter, numSamples);

		// Attach to framebuffer.
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0 + index,
			numSamples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D,
			colorAttachments[index]->GetHandle(),
			0);

		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	bool RenderTarget::Validate() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Error("RenderTarget: Failed to validate with the provided parameters.");
			return false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

		return true;
	}

	void RenderTarget::AttachDepthTexture(Texture::Ptr& tex)
	{
		ASSERT(FBO != GL_NONE, "RenderTarget must be initialized before use.");
		ASSERT(tex->GetHandle() != GL_NONE, "'tex' is not initialized.");
		ASSERT(tex->GetNumSamples() == numSamples, "'tex' must have the same per-texel sample count as the RenderTarget.");
		ASSERT(tex->GetTextureFormat() == TextureFormat::DEPTH_24, "Format of 'tex' must be a 24 bit depth texture.");
		ASSERT(tex->GetWidth() == width, "Dimensions of 'tex' must match the dimensions of the RenderTarget.");
		ASSERT(tex->GetHeight() == height, "Dimensions of 'tex' must match the dimensions of the RenderTarget.");

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, numSamples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, tex->GetHandle(), 0);
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

		depthAttachment = tex;
	}

	void RenderTarget::DetachDepthTexture()
	{
		ASSERT(FBO != GL_NONE, "RenderTarget must be initialized before use.");

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, numSamples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, GL_NONE, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

		depthAttachment.reset();
	}

	void RenderTarget::Unload()
	{
		depthAttachment.reset();

		delete[] colorAttachments;
		colorAttachments = nullptr;

		glDeleteFramebuffers(1, &FBO);
		FBO = GL_NONE;
	}

	void RenderTarget::Clear() const
	{
		ASSERT(FBO != GL_NONE, "RenderTarget must be initialized before use.");

		GLbitfield clearFlags = 0x0000;
		if (HasDepth())
		{
			clearFlags |= GL_DEPTH_BUFFER_BIT;
		}

		if (colorAttachments != nullptr)
		{
			clearFlags |= GL_COLOR_BUFFER_BIT;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClear(clearFlags);
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	void RenderTarget::ClearDepth() const
	{
		ASSERT(FBO != GL_NONE, "RenderTarget must be initialized before use.");

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	void RenderTarget::ClearDepth(float depth) const
	{
		ASSERT(FBO != GL_NONE, "RenderTarget must be initialized before use.");
		ASSERT(depth >= 0.0f && depth <= 1.0f, "'depth' must be in the range of [0, 1].");

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClearBufferfv(GL_DEPTH, 0, &depth);
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	void RenderTarget::ClearColor() const
	{
		ASSERT(FBO != GL_NONE, "RenderTarget must be initialized before use.");

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	void RenderTarget::ClearColor(unsigned index) const
	{
		ASSERT(FBO != GL_NONE, "RenderTarget must be initialized before use.");
		ASSERT(numColorAttachments > 0 , "RenderTarget does not have a color attachment to clear.");
		ASSERT(index < numColorAttachments, "'index' must specify a valid color attachment.");

		// Use the current clear color.
		float color[4];
		glGetFloatv(GL_COLOR_CLEAR_VALUE, color);

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClearBufferfv(GL_COLOR, static_cast<int>(index), color);
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	void RenderTarget::ClearColor(unsigned index, const vec4& color) const
	{
		ClearColor(index, color.x, color.y, color.z, color.w);
	}

	void RenderTarget::ClearColor(unsigned index, float red, float green, float blue, float alpha) const
	{
		ASSERT(FBO != GL_NONE, "RenderTarget must be initialized before use.");
		ASSERT(numColorAttachments > 0, "RenderTarget does not have a color attachment to clear");
		ASSERT(index < numColorAttachments, "'index' must specify a valid color attachment.");

		float color[4] = { red, green, blue, alpha };
		
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClearBufferfv(GL_COLOR, static_cast<int>(index), color);
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	void RenderTarget::Bind() const
	{
		ASSERT(FBO != GL_NONE, "RenderTarget must be initialized before use.");

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	}

	void RenderTarget::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	bool RenderTarget::Resize(unsigned pixelWidth, unsigned pixelHeight)
	{
		ASSERT(FBO != GL_NONE, "RenderTarget must be initialized before use.");
		ASSERT(pixelWidth > 0, "'width' must be greater than 0.");
		ASSERT(pixelHeight > 0, "'height' must be greater than 0.");

		if (width == pixelWidth && height == pixelHeight)
		{
			return true;
		}

		// Save information to recreate textures.
		unsigned numColorTextures = numColorAttachments;
		unsigned samples = numSamples;
		bool hasDepth = HasDepth();
		TextureFormat* formats = nullptr;
		defer {
			delete[] formats;
		};

		if (numColorTextures > 0)
		{
			formats = new TextureFormat[numColorTextures];

			for (unsigned i = 0; i < numColorTextures; i++)
			{
				formats[i] = colorAttachments[i]->GetTextureFormat();
			}
		}

		Unload();

		Init(pixelWidth, pixelHeight, numColorTextures, hasDepth, samples);
		for (unsigned i = 0; i < numColorTextures; i++)
		{
			CreateAttachment(i, formats[i], TextureFilterMode::Point);
		}

		return Validate();
	}

	void RenderTarget::ResolveMultisampling(const RenderTarget& target) const
	{
		ASSERT(FBO != GL_NONE, "RenderTarget must be initialized before use.");
		ASSERT(numColorAttachments == target.numColorAttachments, "RenderTargets must have the same number of attachments.");
		ASSERT(HasDepth() == target.HasDepth(), "One RenderTarget has a depth buffer but the other does not.");
		ASSERT(target.FBO != GL_NONE, "'target' is not an initialized RenderTarget.");
		ASSERT(target.numSamples == 1, "'target' must have a sample count of 1.");

		GLenum filter = (width == target.width) && (height == target.height) ? GL_NEAREST : GL_LINEAR;
		
		glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.FBO);

		if (HasDepth())
		{
			ASSERT(width == target.width && height == target.height, "Source and target RenderTargets must have the same dimensions to resolve a depth buffer.");
			glBlitFramebuffer(0, 0, width, height, 0, 0, target.width, target.height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		}

		for (unsigned i = 0; i < numColorAttachments; i++)
		{
			glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
			glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
			glBlitFramebuffer(0, 0, width, height, 0, 0, target.width, target.height, GL_COLOR_BUFFER_BIT, filter);
		}

		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	void RenderTarget::CopyDepth(const RenderTarget& target) const
	{
		ASSERT(FBO != GL_NONE, "RenderTarget must be initialized before use.");
		ASSERT(target.FBO != GL_NONE, "'target' is not an initialized RenderTarget.");
		ASSERT(HasDepth(), "RenderTarget must have depth texture to copy.");
		ASSERT(target.HasDepth(), "'target' must have depth texture.");
		ASSERT(numSamples == target.numSamples, "Source and target RenderTargets must have the same sample count.");
		ASSERT(width == target.width && height == target.height, "Source and target RenderTargets must have the same dimensions.");

		glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.FBO);
		
		glBlitFramebuffer(0, 0, width, height, 0, 0, target.width, target.height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	void RenderTarget::CopyColor(const RenderTarget& target, unsigned index) const
	{
		ASSERT(FBO != GL_NONE, "RenderTarget must be initialized before use.");
		ASSERT(target.FBO != GL_NONE, "'target' is not an initialized RenderTarget.");
		ASSERT(numColorAttachments > 0, "RenderTarget does not have a color attachment to copy.");
		ASSERT(target.numColorAttachments > 0, "'target' RenderTarget does not have a color attachment to copy.");
		ASSERT(index < numColorAttachments, "'index' must specify a valid color attachment.");
		ASSERT(index < target.numColorAttachments, "'target' does not have a color attachment at index %u", index);

		GLenum filter = (width == target.width) && (height == target.height) ? GL_NEAREST : GL_LINEAR;

		glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.FBO);
		glReadBuffer(GL_COLOR_ATTACHMENT0 + index);
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + index);

		glBlitFramebuffer(0, 0, width, height, 0, 0, target.width, target.height, GL_COLOR_BUFFER_BIT, filter);

		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	void RenderTarget::CopyDepthToBackBuffer() const
	{
		ASSERT(FBO != GL_NONE, "RenderTarget must be initialized before use.");
		ASSERT(HasDepth(), "RenderTarget does not have a depth texture to copy.");
		ASSERT(width == (unsigned)Application.GetScreenWidth() && height == (unsigned)Application.GetScreenHeight(), "RenderTarget and Backbuffer must have the same dimensions.");

		glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);

		glBlitFramebuffer(0, 0, width, height, 0, 0, Application.GetScreenWidth(), Application.GetScreenHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	void RenderTarget::CopyColorToBackBuffer(unsigned index) const
	{
		ASSERT(FBO != GL_NONE, "RenderTarget must be initialized before use.");
		ASSERT(numColorAttachments > 0, "RenderTarget does not have a color attachment to copy.");
		ASSERT(index < numColorAttachments, "'index' must specify a valid color attachment.");

		GLenum filter = (width == static_cast<unsigned>(Application.GetScreenWidth())) && (height == static_cast<unsigned>(Application.GetScreenHeight())) ? GL_NEAREST : GL_LINEAR;

		glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);
		glReadBuffer(GL_COLOR_ATTACHMENT0 + index);

		glBlitFramebuffer(0, 0, width, height, 0, 0, Application.GetScreenWidth(), Application.GetScreenHeight(), GL_COLOR_BUFFER_BIT, filter);

		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	vec4 RenderTarget::ReadPixel(unsigned index, const vec2& position) const
	{
		ASSERT(FBO != GL_NONE, "RenderTarget must be initialized before use.");
		ASSERT(index < numColorAttachments, "'index' must specify a valid color attachment.");
		ASSERT(numSamples == 1, "RenderTarget must not be multi-sampled.");

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glReadBuffer(GL_COLOR_ATTACHMENT0 + index);

		unsigned format = 0;
		switch (colorAttachments[index]->GetNumChannels())
		{
		case 4: format = GL_RGBA;
			break;
		case 3: format = GL_RGB;
			break;
		case 1: format = GL_RED;
			break;
		}

		vec4 result;
		switch (colorAttachments[index]->GetTextureFormat())
		{
		case TextureFormat::RGB_8:
		case TextureFormat::RGBA_8:
			{
				unsigned char pixel[4] = { 0 };
				glReadPixels(static_cast<int>(position.x), static_cast<int>(position.y), 1, 1, format, GL_UNSIGNED_BYTE, pixel);
				// Normalize color range to [0, 1].
				result = vec4(pixel[0], pixel[1], pixel[2], pixel[3]) / static_cast<float>(UCHAR_MAX);
			} break;

		case TextureFormat::RGB_16:
		case TextureFormat::RGBA_16:
			{
				unsigned short pixel[4] = { 0 };
				glReadPixels(static_cast<int>(position.x), static_cast<int>(position.y), 1, 1, format, GL_UNSIGNED_SHORT, pixel);
				// Normalize color range to [0, 1].
				result = vec4(pixel[0], pixel[1], pixel[2], pixel[3]) / static_cast<float>(USHRT_MAX);
			} break;

		case TextureFormat::RGB_32:
		case TextureFormat::RGBA_32:
			{
				unsigned pixel[4] = { 0 };
				glReadPixels(static_cast<int>(position.x), static_cast<int>(position.y), 1, 1, format, GL_UNSIGNED_INT, pixel);
				// Normalize color range to [0, 1].
				result = vec4(
					static_cast<float>(pixel[0]),
					static_cast<float>(pixel[1]),
					static_cast<float>(pixel[2]),
					static_cast<float>(pixel[3])) / static_cast<float>(UINT_MAX);
			} break;

		case TextureFormat::RGB_16F:
		case TextureFormat::RGBA_16F:
		case TextureFormat::RGBA_32F:
		case TextureFormat::RGB_32F:
			glReadPixels(static_cast<int>(position.x), static_cast<int>(position.y), 1, 1, format, GL_FLOAT, &result.x);
			break;

		default:
			ASSERT(false, "Unsupported texture format.");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

		return result;
	}

	bool RenderTarget::HasDepth() const
	{
		return depthAttachment != nullptr;
	}

	bool RenderTarget::IsMultisampled() const
	{
		return numSamples != 1;
	}

	unsigned RenderTarget::GetNumColorAttachments() const
	{
		return numColorAttachments;
	}

	unsigned RenderTarget::GetNumSamples() const
	{
		return numSamples;
	}

	Texture::Ptr RenderTarget::GetDepthTexture() const
	{
		return depthAttachment;
	}

	Texture::Ptr RenderTarget::GetColorTexture(unsigned index) const
	{
		ASSERT(numColorAttachments > 0, "RenderTarget does not have any color attachments.");
		ASSERT(index < numColorAttachments, "'index' must specify a valid color attachment.");

		return colorAttachments[index];
	}

	Viewport RenderTarget::GetViewport() const
	{
		return Viewport(0, 0, width, height);
	}

	unsigned RenderTarget::GetWidth() const
	{
		return width;
	}

	unsigned RenderTarget::GetHeight() const
	{
		return height;
	}
}
