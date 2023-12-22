// Copyright (c) 2017 Emilian Cioca
#include "Rendering.h"
#include "gemcutter/Application/FileSystem.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Math/Math.h"
#include "gemcutter/Math/Vector.h"
#include "gemcutter/Utilities/String.h"

#include <glew/glew.h>
#include <glew/wglew.h>
#include <loupe/loupe.h>
#include <SOIL/SOIL.h>

namespace
{
	constexpr int vertexFormat_Resolve[] = {
		GL_FLOAT,
		GL_DOUBLE,
		GL_FLOAT,
		GL_FLOAT,
		GL_FLOAT,
		GL_FLOAT,
		GL_FLOAT,
		GL_FLOAT,
		GL_INT,
		GL_UNSIGNED_INT,
		GL_SHORT,
		GL_UNSIGNED_SHORT,
		GL_BYTE,
		GL_UNSIGNED_BYTE
	};

	constexpr int vertexArrayFormat_Resolve[] = {
		GL_POINTS,
		GL_LINES,
		GL_LINES_ADJACENCY,
		GL_LINE_LOOP,
		GL_LINE_STRIP,
		GL_LINE_STRIP_ADJACENCY,
		GL_TRIANGLES,
		GL_TRIANGLES_ADJACENCY,
		GL_TRIANGLE_FAN,
		GL_TRIANGLE_STRIP,
		GL_TRIANGLE_STRIP_ADJACENCY
	};

	constexpr int vertexFormatSize_Resolve[] = {
		4,  // float
		8,  // double
		8,  // vec2
		12, // vec3
		16, // vec4
		16, // mat2
		36, // mat3
		64, // mat4
		4,  // int
		4,  // unsigned int
		2,  // short
		2,  // unsigned short
		1,  // char
		1   // unsigned char
	};

	constexpr int textureFormatChannelCount_Resolve[] = {
		1, // R_8
		1, // R_16
		1, // R_16F
		1, // R_32
		1, // R_32F
		3, // RGB_8
		3, // RGB_16
		3, // RGB_16F
		3, // RGB_32
		3, // RGB_32F
		4, // RGBA_8
		4, // RGBA_16
		4, // RGBA_16F
		4, // RGBA_32
		4, // RGBA_32F
		1, // DEPTH_24
		3, // sRGB_8
		4  // sRGBA_8
	};

	constexpr int vertexAccess_Resolve[] = {
		GL_READ_ONLY,
		GL_WRITE_ONLY,
		GL_READ_WRITE
	};

	constexpr int bufferUsage_Resolve[] = {
		GL_STATIC_DRAW,
		GL_DYNAMIC_DRAW,
		GL_STREAM_DRAW
	};

	constexpr int filterMin_Resolve[] = {
		GL_NEAREST,
		GL_LINEAR,
		GL_LINEAR_MIPMAP_NEAREST,
		GL_LINEAR_MIPMAP_LINEAR
	};

	constexpr int wrap_Resolve[] = {
		GL_CLAMP_TO_EDGE,
		GL_CLAMP_TO_BORDER,
		GL_REPEAT,
		GL_MIRRORED_REPEAT,
		GL_MIRROR_CLAMP_TO_EDGE
	};

	constexpr unsigned format_Resolve[] = {
		GL_R8,                // R_8
		GL_R16,               // R_16
		GL_R16F,              // R_16F
		GL_R32UI,             // R_32
		GL_R32F,              // R_32F
		GL_RGB8,              // RGB_8
		GL_RGB16,             // RGB_16
		GL_RGB16F,            // RGB_16F
		GL_RGB32UI,           // RGB_32
		GL_RGB32F,            // RGB_32F
		GL_RGBA8,             // RGBA_8
		GL_RGBA16,            // RGBA_16
		GL_RGBA16F,           // RGBA_16F
		GL_RGBA32UI,          // RGBA_32
		GL_RGBA32F,           // RGBA_32F
		GL_DEPTH_COMPONENT24, // DEPTH_24
		GL_SRGB8,             // sRGB_8
		GL_SRGB8_ALPHA8       // sRGBA_8
	};

	constexpr int dataFormat_resolve[] = {
		GL_RED,               // R_8
		GL_RED,               // R_16
		GL_RED,               // R_16F
		GL_RED,               // R_32
		GL_RED,               // R_32F
		GL_RGB,               // RGB_8
		GL_RGB,               // RGB_16
		GL_RGB,               // RGB_16F
		GL_RGB,               // RGB_32
		GL_RGB,               // RGB_32F
		GL_RGBA,              // RGBA_8
		GL_RGBA,              // RGBA_16
		GL_RGBA,              // RGBA_16F
		GL_RGBA,              // RGBA_32
		GL_RGBA,              // RGBA_32F
		GL_DEPTH_COMPONENT,   // DEPTH_24
		GL_RGB,               // sRGB_8
		GL_RGBA               // sRGBA_8
	};
}

namespace gem
{
	TextureWraps::TextureWraps(TextureWrap xy)
		: x(xy), y(xy)
	{
	}

	TextureWraps::TextureWraps(TextureWrap _x, TextureWrap _y)
		: x(_x), y(_y)
	{
	}

	int ResolveVertexFormat(VertexFormat format)
	{
		return vertexFormat_Resolve[static_cast<unsigned>(format)];
	}

	int ResolveVertexArrayFormat(VertexArrayFormat format)
	{
		return vertexArrayFormat_Resolve[static_cast<unsigned>(format)];
	}

	int ResolveVertexAccess(VertexAccess access)
	{
		return vertexAccess_Resolve[static_cast<unsigned>(access)];
	}

	int ResolveBufferUsage(BufferUsage usage)
	{
		return bufferUsage_Resolve[static_cast<unsigned>(usage)];
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

	unsigned ResolveDataFormat(TextureFormat format)
	{
		return dataFormat_resolve[static_cast<unsigned>(format)];
	}

	unsigned CountBytes(VertexFormat format)
	{
		return vertexFormatSize_Resolve[static_cast<unsigned>(format)];
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
		return textureFormatChannelCount_Resolve[static_cast<unsigned>(format)];
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

	void SetScissor(unsigned x, unsigned y, unsigned width, unsigned height)
	{
		glScissor(x, y, width, height);
	}

	void SetScissor(bool enabled)
	{
		if (enabled)
		{
			glEnable(GL_SCISSOR_TEST);
		}
		else
		{
			glDisable(GL_SCISSOR_TEST);
		}
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

REFLECT(gem::UniformBufferSlot)
	ENUM_VALUES {
		REF_VALUE(Camera)
		REF_VALUE(Model)
		REF_VALUE(Engine)
		REF_VALUE(Time)
		REF_VALUE(Particle)
	}
REF_END;

REFLECT(gem::VertexFormat)
	ENUM_VALUES {
		REF_VALUE(Float)
		REF_VALUE(Double)
		REF_VALUE(Vec2)
		REF_VALUE(Vec3)
		REF_VALUE(Vec4)
		REF_VALUE(Mat2)
		REF_VALUE(Mat3)
		REF_VALUE(Mat4)
		REF_VALUE(Int)
		REF_VALUE(uInt)
		REF_VALUE(Short)
		REF_VALUE(uShort)
		REF_VALUE(Byte)
		REF_VALUE(uByte)
	}
REF_END;

REFLECT(gem::VertexArrayFormat)
	ENUM_VALUES {
		REF_VALUE(Point)
		REF_VALUE(Line)
		REF_VALUE(LineAdjacency)
		REF_VALUE(LineLoop)
		REF_VALUE(LineStrip)
		REF_VALUE(LineStripAdjacency)
		REF_VALUE(Triangle)
		REF_VALUE(TriangleAdjacency)
		REF_VALUE(TriangleFan)
		REF_VALUE(TriangleStrip)
		REF_VALUE(TriangleStripAdjacency)
	}
REF_END;

REFLECT(gem::VertexAccess)
	ENUM_VALUES {
		REF_VALUE(ReadOnly)
		REF_VALUE(WriteOnly)
		REF_VALUE(ReadWrite)
	}
REF_END;

REFLECT(gem::VertexBufferType)
	ENUM_VALUES {
		REF_VALUE(Data)
		REF_VALUE(Index)
	}
REF_END;

REFLECT(gem::BufferUsage)
	ENUM_VALUES {
		REF_VALUE(Static)
		REF_VALUE(Dynamic)
		REF_VALUE(Stream)
	}
REF_END;

REFLECT(gem::TextureFormat)
	ENUM_VALUES {
		REF_VALUE(R_8)
		REF_VALUE(R_16)
		REF_VALUE(R_16F)
		REF_VALUE(R_32)
		REF_VALUE(R_32F)
		REF_VALUE(RGB_8)
		REF_VALUE(RGB_16)
		REF_VALUE(RGB_16F)
		REF_VALUE(RGB_32)
		REF_VALUE(RGB_32F)
		REF_VALUE(RGBA_8)
		REF_VALUE(RGBA_16)
		REF_VALUE(RGBA_16F)
		REF_VALUE(RGBA_32)
		REF_VALUE(RGBA_32F)
		REF_VALUE(DEPTH_24)
		REF_VALUE(sRGB_8)
		REF_VALUE(sRGBA_8)
	}
REF_END;

REFLECT(gem::TextureWrap)
	ENUM_VALUES {
		REF_VALUE(Clamp)
		REF_VALUE(ClampWithBorder)
		REF_VALUE(Repeat)
		REF_VALUE(RepeatMirrored)
		REF_VALUE(RepeatMirroredOnce)
	}
REF_END;

REFLECT(gem::TextureWraps)
	MEMBERS {
		REF_MEMBER(x)
		REF_MEMBER(y)
	}
REF_END;

REFLECT(gem::TextureFilter)
	ENUM_VALUES {
		REF_VALUE(Point)
		REF_VALUE(Linear)
		REF_VALUE(Bilinear)
		REF_VALUE(Trilinear)
	}
REF_END;

REFLECT(gem::CullFunc)
	ENUM_VALUES {
		REF_VALUE(None)
		REF_VALUE(Clockwise)
		REF_VALUE(CounterClockwise)
	}
REF_END;

REFLECT(gem::BlendFunc)
	ENUM_VALUES {
		REF_VALUE(None)
		REF_VALUE(Linear)
		REF_VALUE(Additive)
		REF_VALUE(Multiplicative)
	}
REF_END;

REFLECT(gem::DepthFunc)
	ENUM_VALUES {
		REF_VALUE(None)
		REF_VALUE(WriteOnly)
		REF_VALUE(TestOnly)
		REF_VALUE(Normal)
	}
REF_END;
