// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Reflection/Reflection.h"

#include <string_view>

namespace Jwl
{
	struct vec4;

	// Binding locations for Jewel3D uniform buffers.
	enum class UniformBufferSlot : unsigned
	{
		Camera = 10,
		Model = 11,
		Engine = 12,
		Time = 13,
		Particle = 14
	};

	enum class VertexFormat
	{
		Float,
		Double,
		Vec2,
		Vec3,
		Vec4,
		Mat2,
		Mat3,
		Mat4,
		Int,
		uInt,
		Short,
		uShort,
		Byte,
		uByte
	};

	enum class VertexAccess
	{
		ReadOnly,
		WriteOnly,
		ReadWrite
	};

	enum class VertexBufferUsage
	{
		// The buffer is not expected to change and will be kept on the GPU only.
		Static,
		// The buffer is expected to be occasionally updated. It will be optimized for editing.
		Dynamic,
		// The buffer is expected to be updated before each render. It will be optimized for streaming to the GPU.
		Stream
	};

	enum class TextureFormat
	{
		RGB_8,
		RGB_16,
		RGB_16F,
		RGB_32,
		RGB_32F,
		RGBA_8,
		RGBA_16,
		RGBA_16F,
		RGBA_32,
		RGBA_32F,
		DEPTH_24,
		sRGB_8,
		sRGBA_8
	};

	enum class TextureWrap
	{
		Clamp,
		ClampWithBorder,
		Repeat,
		RepeatMirrored,
		RepeatMirroredOnce
	};

	struct TextureWraps
	{
		TextureWraps() = default;
		TextureWraps(TextureWrap xy);
		TextureWraps(TextureWrap x, TextureWrap y);

		TextureWrap x = TextureWrap::Clamp;
		TextureWrap y = TextureWrap::Clamp;
	};

	enum class TextureFilter
	{
		Point,
		Linear,
		Bilinear,
		Trilinear
	};

	enum class CullFunc
	{
		// Both back and front faces are rendered.
		None,
		// Clockwise face is culled from rendering. This is the default behaviour.
		Clockwise,
		// Counter-clockwise face is culled from rendering.
		CounterClockwise
	};

	enum class BlendFunc
	{
		// Blending is disabled.
		None,
		// Color is interpolated with the pixels in the renderTarget based on alpha.
		Linear,
		// Color is added onto the pixels in the renderTarget.
		Additive,
		// Color is multiplied with the pixels in the renderTarget.
		Multiplicative,
		// Blending is disabled, but pixels with less than 1.0 alpha are discarded.
		CutOut
	};

	enum class DepthFunc
	{
		None,
		WriteOnly,
		TestOnly,
		Normal
	};

	enum class VSyncMode
	{
		// Frames are sent to display device as soon as they are ready.
		// Screen tearing can occur but input is most responsive.
		Off = 0,
		// Framerate is locked to multiples of the display's refresh rate to eliminate tearing.
		// Can introduce input lag or stuttering.
		On = 1,
		// Vsync is on when the framerate exceeds the display's refresh rate to eliminate tearing.
		// When the framerate drops, Vsync turns off to eliminate stuttering and input lag.
		Adaptive = -1
	};

	// Used internally to convert enum values to OpenGL values.
	int ResolveVertexFormat(VertexFormat);
	int ResolveVertexAccess(VertexAccess);
	int ResolveVertexBufferUsage(VertexBufferUsage);
	int ResolveFilterMag(TextureFilter);
	int ResolveFilterMin(TextureFilter);
	bool ResolveMipMapping(TextureFilter);
	int ResolveWrap(TextureWrap);
	unsigned ResolveFormat(TextureFormat);

	TextureFilter StringToTextureFilter(std::string_view);
	TextureWrap StringToTextureWrap(std::string_view);

	unsigned CountBytes(VertexFormat);
	unsigned CountMipLevels(unsigned width, unsigned height, TextureFilter);
	unsigned CountChannels(TextureFormat);

	void ClearBackBuffer();
	void ClearBackBufferDepth();
	void ClearBackBufferColor();

	void SetClearColor(const vec4& color);
	void SetClearColor(float r, float g, float b, float a);
	void SetWireframe(bool enabled);
	void SetCullFunc(CullFunc func);
	void SetBlendFunc(BlendFunc func);
	void SetDepthFunc(DepthFunc func);
	void SetViewport(unsigned x, unsigned y, unsigned width, unsigned height);
	bool SetVSync(VSyncMode mode);

	// Saves a screenshot of the currently bound RenderTarget or the backbuffer.
	// This is a very slow function and should only be used for debugging or when absolutely necessary.
	bool SaveScreenshot(std::string_view filePath, unsigned x, unsigned y, unsigned width, unsigned height);

	extern class GPUInfoSingleton GPUInfo;
	class GPUInfoSingleton
	{
		friend class ApplicationSingleton;
	public:
		unsigned GetMaxTextureSlots() const;
		unsigned GetMaxUniformBufferSlots() const;
		unsigned GetMaxRenderTargetTextures() const;
		unsigned GetMaxDrawBuffers() const;

	private:
		void ScanDevice();

		unsigned maxTextureSlots = 0;
		unsigned maxUniformBufferSlots = 0;
		unsigned maxRenderTargetTextures = 0;
		unsigned maxDrawBuffers = 0;
	};
}

REFLECT(Jwl::TextureFormat)<>,
	VALUES <
		REF_VALUE(RGB_8),
		REF_VALUE(RGB_16),
		REF_VALUE(RGB_16F),
		REF_VALUE(RGB_32),
		REF_VALUE(RGB_32F),
		REF_VALUE(RGBA_8),
		REF_VALUE(RGBA_16),
		REF_VALUE(RGBA_16F),
		REF_VALUE(RGBA_32),
		REF_VALUE(RGBA_32F),
		REF_VALUE(DEPTH_24)
	>
REF_END;

REFLECT(Jwl::TextureWrap)<>,
	VALUES <
		REF_VALUE(Clamp),
		REF_VALUE(ClampWithBorder),
		REF_VALUE(Repeat),
		REF_VALUE(RepeatMirrored),
		REF_VALUE(RepeatMirroredOnce)
	>
REF_END;

REFLECT(Jwl::TextureFilter)<>,
	VALUES <
		REF_VALUE(Point),
		REF_VALUE(Linear),
		REF_VALUE(Bilinear),
		REF_VALUE(Trilinear)
	>
REF_END;

REFLECT(Jwl::CullFunc)<>,
	VALUES <
		REF_VALUE(None),
		REF_VALUE(Clockwise),
		REF_VALUE(CounterClockwise)
	>
REF_END;

REFLECT(Jwl::BlendFunc)<>,
	VALUES <
		REF_VALUE(None),
		REF_VALUE(Linear),
		REF_VALUE(Additive),
		REF_VALUE(Multiplicative),
		REF_VALUE(CutOut)
	>
REF_END;

REFLECT(Jwl::DepthFunc)<>,
	VALUES <
		REF_VALUE(None),
		REF_VALUE(WriteOnly),
		REF_VALUE(TestOnly),
		REF_VALUE(Normal)
	>
REF_END;
