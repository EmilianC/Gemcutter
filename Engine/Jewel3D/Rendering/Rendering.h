// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Utilities/Singleton.h"

#include <string>

namespace Jwl
{
	class vec4;

	enum class UniformBufferSlot : u32
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
		DEPTH_24
	};

	enum class TextureWrapMode
	{
		Clamp,
		ClampWithBorder,
		Repeat,
		RepeatMirrored,
		RepeatMirroredOnce
	};

	struct TextureWrapModes
	{
		TextureWrapModes() = default;
		TextureWrapModes(TextureWrapMode xy);
		TextureWrapModes(TextureWrapMode x, TextureWrapMode y);

		TextureWrapMode x = TextureWrapMode::Clamp;
		TextureWrapMode y = TextureWrapMode::Clamp;
	};

	enum class TextureFilterMode
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

	s32 ResolveFilterMagMode(TextureFilterMode filter);
	s32 ResolveFilterMinMode(TextureFilterMode filter);
	bool ResolveMipMapping(TextureFilterMode filter);
	s32 ResolveWrapMode(TextureWrapMode wrapMode);
	u32 ResolveFormat(TextureFormat format);

	TextureFilterMode StringToFilterMode(const std::string&);
	TextureWrapMode StringToWrapMode(const std::string&);

	u32 CountMipLevels(u32 width, u32 height, TextureFilterMode filter);
	u32 CountChannels(TextureFormat format);

	void ClearBackBuffer();
	void ClearBackBufferDepth();
	void ClearBackBufferColor();

	void SetActiveTextureUnit(u32 unit);
	void SetClearColor(const vec4& color);
	void SetClearColor(f32 r, f32 g, f32 b, f32 a);
	void SetWireframe(bool enabled);
	void SetCullFunc(CullFunc func);
	void SetBlendFunc(BlendFunc func);
	void SetDepthFunc(DepthFunc func);
	void SetViewport(u32 x, u32 y, u32 width, u32 height);
	bool SetVSync(VSyncMode mode);

	static class GPUInfo : public Singleton<class GPUInfo>
	{
	public:
		//- Queries the limits from the device and provides them to the application.
		//- Called by Application on window creation.
		void ScanDevice();

		u32 GetMaxTextureSlots() const;
		u32 GetMaxUniformBufferSlots() const;
		u32 GetMaxRenderTargetAttachments() const;
		u32 GetMaxDrawBuffers() const;

	private:
		u32 MaxTextureSlots = 0;
		u32 MaxUniformBufferSlots = 0;
		u32 MaxRenderTargetAttachments = 0;
		u32 MaxDrawBuffers = 0;
	} &GPUInfo = Singleton<class GPUInfo>::instanceRef;
}
