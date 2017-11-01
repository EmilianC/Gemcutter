// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Utilities/Singleton.h"

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

	int ResolveFilterMag(TextureFilter filter);
	int ResolveFilterMin(TextureFilter filter);
	bool ResolveMipMapping(TextureFilter filter);
	int ResolveWrap(TextureWrap wrap);
	unsigned ResolveFormat(TextureFormat format);

	TextureFilter StringToTextureFilter(std::string_view);
	TextureWrap StringToTextureWrap(std::string_view);

	unsigned CountMipLevels(unsigned width, unsigned height, TextureFilter filter);
	unsigned CountChannels(TextureFormat format);

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

	static class GPUInfo : public Singleton<class GPUInfo>
	{
		friend class Application;
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
	} &GPUInfo = Singleton<class GPUInfo>::instanceRef;
}
