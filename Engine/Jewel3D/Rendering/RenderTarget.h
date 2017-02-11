// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Viewport.h"
#include "Jewel3D/Resource/Shareable.h"
#include "Jewel3D/Resource/Texture.h"

namespace Jwl
{
	class vec4;

	class RenderTarget : public Shareable<RenderTarget>
	{
		friend ShareableAlloc;
		RenderTarget() = default;
		RenderTarget(const RenderTarget&) = delete;
		~RenderTarget();

	public:
		RenderTarget& operator=(const RenderTarget&) = delete;

		//- Must be called first. Implicitly calls Unload().
		void Init(unsigned pixelWidth, unsigned pixelHeight, unsigned numColorTextures, bool hasDepth, unsigned samples = 1);
		//- Initialize this RenderTarget as a non multi-sampled version of the provided RenderTarget. Implicitly calls Unload() and Validate().
		bool InitAsResolve(const RenderTarget& multisampleBuffer, TextureFilterMode filter);
		//- Allocates and initializes the texture slot specified by index. Must be called on each slot before use.
		void CreateAttachment(unsigned index, TextureFormat format, TextureFilterMode filter);
		//- Should be called once after all texture slots are created. Returns true if the RenderTarget was created successfully.
		bool Validate() const;

		//- Attach an existing texture.
		void AttachDepthTexture(Texture::Ptr& tex);
		void DetachDepthTexture();
		//- Releases all VRAM.
		void Unload();

		//- Clears all textures to the current global clear color/depth.
		void Clear() const;
		//- Clears depth texture to the current global clear depth.
		void ClearDepth() const;
		//- Clears depth texture to [depth].
		void ClearDepth(float depth) const;
		//- Clears all color textures to the current global clear color.
		void ClearColor() const;
		//- Clears the Specified texture to the current global clear color.
		void ClearColor(unsigned index) const;
		//- Clears all textures to [color].
		void ClearColor(unsigned index, const vec4& color) const;
		void ClearColor(unsigned index, float red, float green, float blue, float alpha) const;

		void Bind() const;
		static void UnBind();

		//- Recreates the render target with different sized textures. Textures data will be lost and all filters will be set to point.
		bool Resize(unsigned pixelWidth, unsigned pixelHeight);
		void ResolveMultisampling(const RenderTarget& target) const;

		void CopyDepth(const RenderTarget& target) const;
		void CopyColor(const RenderTarget& target, unsigned index) const;

		void CopyDepthToBackBuffer() const;
		void CopyColorToBackBuffer(unsigned index) const;

		//- Retrieves a pixel from the specified color buffer.
		//- This is not recommended for real-time use because it can be very expensive.
		vec4 ReadPixel(unsigned index, const vec2& position) const;

		bool HasDepth() const;
		bool IsMultisampled() const;
		unsigned GetNumColorAttachments() const;
		unsigned GetNumSamples() const;

		Texture::Ptr GetDepthTexture() const;
		Texture::Ptr GetColorTexture(unsigned index) const;

		//- Returns a viewport matching the dimensions of the render target.
		Viewport GetViewport() const;
		unsigned GetWidth() const;
		unsigned GetHeight() const;

	private:
		unsigned FBO				 = 0;
		unsigned numColorAttachments = 0;
		unsigned numSamples			 = 0;
		unsigned width				 = 0;
		unsigned height				 = 0;

		Texture::Ptr depthAttachment;
		Texture::Ptr* colorAttachments = nullptr;
	};
}
