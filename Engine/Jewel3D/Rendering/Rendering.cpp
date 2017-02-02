// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Rendering.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Application/Application.h"
#include "Jewel3D/Math/Vector.h"

#include <GLEW/GL/glew.h>
#include <GLEW/GL/wglew.h>

namespace
{
	static const int filterMinMode_Resolve[] = {
		GL_NEAREST,
		GL_LINEAR,
		GL_LINEAR_MIPMAP_NEAREST,
		GL_LINEAR_MIPMAP_LINEAR
	};

	static const int wrapMode_Resolve[] = {
		GL_CLAMP_TO_EDGE,
		GL_CLAMP_TO_BORDER,
		GL_REPEAT,
		GL_MIRRORED_REPEAT,
		GL_MIRROR_CLAMP_TO_EDGE
	};

	static const unsigned format_Resolve[] = {
		GL_RGB8,
		GL_RGB16,
		GL_RGB16F,
		GL_RGB32UI,
		GL_RGB32F,
		GL_RGBA8,
		GL_RGBA16,
		GL_RGBA16F,
		GL_RGBA32UI,
		GL_RGBA32F,
		GL_DEPTH_COMPONENT24
	};
}

namespace Jwl
{
	int ResolveFilterMagMode(TextureFilterMode filter)
	{
		if (filter == TextureFilterMode::Point)
		{
			return GL_NEAREST;
		}
		else
		{
			return GL_LINEAR;
		}
	}

	int ResolveFilterMinMode(TextureFilterMode filter)
	{
		return filterMinMode_Resolve[static_cast<unsigned>(filter)];
	}

	bool ResolveMipMapping(TextureFilterMode filter)
	{
		return filter == TextureFilterMode::Trilinear || filter == TextureFilterMode::Bilinear;
	}

	int ResolveWrapMode(TextureWrapMode wrapMode)
	{
		return wrapMode_Resolve[static_cast<unsigned>(wrapMode)];
	}

	unsigned ResolveFormat(TextureFormat format)
	{
		return format_Resolve[static_cast<unsigned>(format)];
	}

	int ResolveChannels(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::RGB_8:
		case TextureFormat::RGB_16:
		case TextureFormat::RGB_16F:
		case TextureFormat::RGB_32:
		case TextureFormat::RGB_32F:
			return 3;
		case TextureFormat::RGBA_8:
		case TextureFormat::RGBA_16:
		case TextureFormat::RGBA_16F:
		case TextureFormat::RGBA_32:
		case TextureFormat::RGBA_32F:
			return 4;
		case TextureFormat::DEPTH_24:
			return 1;
		default:
			return 0;
		}
	}

	void ClearBackBuffer()
	{
		SetDepthFunc(DepthFunc::Normal);
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void ClearBackBufferDepth()
	{
		SetDepthFunc(DepthFunc::Normal);
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void ClearBackBufferColor()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void SetActiveTextureUnit(unsigned unit)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
	}

	void SetClearColor(const vec4& color)
	{
		glClearColor(color.x, color.y, color.z, color.w);
	}

	void SetClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void SetWireframe(bool enabled)
	{
		if (enabled)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	void SetCullFunc(CullFunc func)
	{
		switch (func)
		{
		case CullFunc::None:
			glDisable(GL_CULL_FACE);
			break;

		case CullFunc::Clockwise:
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);
			break;

		case CullFunc::CounterClockwise:
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CW);
			break;
		}
	}

	void SetBlendFunc(BlendFunc func)
	{
		switch (func)
		{
		default:
		case BlendFunc::CutOut:
		case BlendFunc::None:
			glDisable(GL_BLEND);
			break;

		case BlendFunc::Linear:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;

		case BlendFunc::Additive:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;

		case BlendFunc::Multiplicative:
			glEnable(GL_BLEND);
			glBlendFunc(GL_DST_COLOR, GL_ZERO);
			break;
		}
	}

	void SetDepthFunc(DepthFunc func)
	{
		switch (func)
		{
		default:
		case DepthFunc::Normal:
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			break;

		case DepthFunc::TestOnly:
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			break;

		case DepthFunc::WriteOnly:
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			break;

		case DepthFunc::None:
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			break;
		}
	}

	void SetViewport(unsigned x, unsigned y, unsigned width, unsigned height)
	{
		glViewport(x, y, width, height);
	}
	
	bool SetVSync(VSyncMode mode)
	{
		if (!wglewIsSupported("WGL_EXT_swap_control"))
		{
			Error("VSync is not supported on this system.");
			return false;
		}

		typedef bool (APIENTRY *FUNC)(int);
		FUNC wglSwapIntervalEXT = (FUNC)wglGetProcAddress("wglSwapIntervalEXT");

		if (!wglSwapIntervalEXT)
		{
			Error("Could not retrieve \"wglSwapIntervalEXT\" function.");
			return false;
		}

		if (mode == VSyncMode::Adaptive && !wglewIsSupported("WGL_EXT_swap_control_tear"))
		{
			Error("Adaptive VSync is not supported on this system. VSync mode left unchanged.");
			return false;
		}

		bool result = wglSwapIntervalEXT(static_cast<int>(mode));

		if (!result)
		{
			Error("Failed to change VSync mode.");
		}

		return result;
	}

	void GPUInfo::ScanDevice()
	{
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, reinterpret_cast<int*>(&MaxTextureSlots));
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, reinterpret_cast<int*>(&MaxUniformBufferSlots));
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, reinterpret_cast<int*>(&MaxRenderTargetAttachments));
		glGetIntegerv(GL_MAX_DRAW_BUFFERS, reinterpret_cast<int*>(&MaxDrawBuffers));
	}

	unsigned GPUInfo::GetMaxTextureSlots() const
	{
		return MaxTextureSlots;
	}

	unsigned GPUInfo::GetMaxUniformBufferSlots() const
	{
		return MaxUniformBufferSlots;
	}

	unsigned GPUInfo::GetMaxRenderTargetAttachments() const
	{
		return MaxRenderTargetAttachments;
	}

	unsigned GPUInfo::GetMaxDrawBuffers() const
	{
		return MaxDrawBuffers;
	}
}
