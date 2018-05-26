// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Rendering.h"
#include "Jewel3D/Application/FileSystem.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Math/Math.h"
#include "Jewel3D/Math/Vector.h"
#include "Jewel3D/Utilities/String.h"

#include <GLEW/GL/glew.h>
#include <GLEW/GL/wglew.h>
#include <SOIL/SOIL.h>

namespace
{
	const int filterMin_Resolve[] = {
		GL_NEAREST,
		GL_LINEAR,
		GL_LINEAR_MIPMAP_NEAREST,
		GL_LINEAR_MIPMAP_LINEAR
	};

	const int wrap_Resolve[] = {
		GL_CLAMP_TO_EDGE,
		GL_CLAMP_TO_BORDER,
		GL_REPEAT,
		GL_MIRRORED_REPEAT,
		GL_MIRROR_CLAMP_TO_EDGE
	};

	const unsigned format_Resolve[] = {
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
		GL_DEPTH_COMPONENT24,
		GL_SRGB8,
		GL_SRGB8_ALPHA8
	};
}

namespace Jwl
{
	TextureWraps::TextureWraps(TextureWrap xy)
		: x(xy), y(xy)
	{
	}

	TextureWraps::TextureWraps(TextureWrap _x, TextureWrap _y)
		: x(_x), y(_y)
	{
	}

	int ResolveFilterMag(TextureFilter filter)
	{
		if (filter == TextureFilter::Point)
		{
			return GL_NEAREST;
		}
		else
		{
			return GL_LINEAR;
		}
	}

	int ResolveFilterMin(TextureFilter filter)
	{
		return filterMin_Resolve[static_cast<unsigned>(filter)];
	}

	bool ResolveMipMapping(TextureFilter filter)
	{
		return filter == TextureFilter::Trilinear || filter == TextureFilter::Bilinear;
	}

	int ResolveWrap(TextureWrap wrap)
	{
		return wrap_Resolve[static_cast<unsigned>(wrap)];
	}

	unsigned ResolveFormat(TextureFormat format)
	{
		return format_Resolve[static_cast<unsigned>(format)];
	}

	TextureFilter StringToTextureFilter(std::string_view str)
	{
		if (CompareLowercase(str, "linear"))
			return TextureFilter::Linear;
		else if (CompareLowercase(str, "bilinear"))
			return TextureFilter::Bilinear;
		else if (CompareLowercase(str, "trilinear"))
			return TextureFilter::Trilinear;
		else
			return TextureFilter::Point;
	}

	TextureWrap StringToTextureWrap(std::string_view str)
	{
		if (CompareLowercase(str, "clampwithborder"))
			return TextureWrap::ClampWithBorder;
		else if (CompareLowercase(str, "repeat"))
			return TextureWrap::Repeat;
		else if (CompareLowercase(str, "repeatmirrored"))
			return TextureWrap::RepeatMirrored;
		else if (CompareLowercase(str, "repeatmirroredonce"))
			return TextureWrap::RepeatMirroredOnce;
		else
			return TextureWrap::Clamp;
	}

	unsigned CountMipLevels(unsigned width, unsigned height, TextureFilter filter)
	{
		unsigned numLevels = 1;
		if (ResolveMipMapping(filter))
		{
			float max = static_cast<float>(Max(width, height));
			numLevels = static_cast<unsigned>(std::floor(std::log2(max))) + 1;
		}

		return numLevels;
	}

	unsigned CountChannels(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::RGB_8:
		case TextureFormat::RGB_16:
		case TextureFormat::RGB_16F:
		case TextureFormat::RGB_32:
		case TextureFormat::RGB_32F:
		case TextureFormat::sRGB_8:
			return 3;
		case TextureFormat::RGBA_8:
		case TextureFormat::RGBA_16:
		case TextureFormat::RGBA_16F:
		case TextureFormat::RGBA_32:
		case TextureFormat::RGBA_32F:
		case TextureFormat::sRGBA_8:
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
		glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL);
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

		auto wglSwapIntervalEXT = reinterpret_cast<bool (APIENTRY *)(int)>(wglGetProcAddress("wglSwapIntervalEXT"));
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

	bool SaveScreenshot(std::string_view filePath, unsigned x, unsigned y, unsigned width, unsigned height)
	{
		ASSERT(!filePath.empty(), "Must provide a valid path and filename.");
		ASSERT(width != 0, "Cannot take a screenshot with a 'width' of zero.");
		ASSERT(height != 0, "Cannot take a screenshot with a 'height' of zero.");

		if (!MakeDirectory(ExtractPath(filePath)))
		{
			Error("Failed to create path for screenshot ( %s ).", filePath.data());
			return false;
		}

		if (!SOIL_save_screenshot(filePath.data(), SOIL_SAVE_TYPE_BMP, x, y, width, height))
		{
			Error("Failed to save screenshot ( %s ).", filePath.data());
			return false;
		}

		return true;
	}

	GPUInfoSingleton GPUInfo;

	void GPUInfoSingleton::ScanDevice()
	{
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, reinterpret_cast<int*>(&maxTextureSlots));
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, reinterpret_cast<int*>(&maxUniformBufferSlots));
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, reinterpret_cast<int*>(&maxRenderTargetTextures));
		glGetIntegerv(GL_MAX_DRAW_BUFFERS, reinterpret_cast<int*>(&maxDrawBuffers));
	}

	unsigned GPUInfoSingleton::GetMaxTextureSlots() const
	{
		return maxTextureSlots;
	}

	unsigned GPUInfoSingleton::GetMaxUniformBufferSlots() const
	{
		return maxUniformBufferSlots;
	}

	unsigned GPUInfoSingleton::GetMaxRenderTargetTextures() const
	{
		return maxRenderTargetTextures;
	}

	unsigned GPUInfoSingleton::GetMaxDrawBuffers() const
	{
		return maxDrawBuffers;
	}
}
