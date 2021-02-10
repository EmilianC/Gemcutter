// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Resource/Shareable.h"
#include "gemcutter/Resource/Texture.h"

namespace gem
{
	struct vec4;
	struct Viewport;

	// Contains a series of textures that can be rendered into.
	class RenderTarget : public Shareable<RenderTarget>
	{
		friend ShareableAlloc;
		~RenderTarget();

	public:
		RenderTarget(const RenderTarget&) = delete;
		RenderTarget& operator=(const RenderTarget&) = delete;

		RenderTarget(unsigned width, unsigned height, unsigned numColorTextures, bool hasDepth, unsigned numSamples = 1);
		// Allocates and initializes the texture slot specified by index. Must be called on each slot before use.
		void InitTexture(unsigned index, TextureFormat format, TextureFilter filter);

		// Creates a non multisampled version of this RenderTarget to be used with the ResolveMultisampling() functions.
		RenderTarget::Ptr MakeResolve() const;

		// Should be called once after all texture slots are created. Returns true if the RenderTarget was created successfully.
		bool Validate() const;

		// Attach an existing texture.
		void AttachDepthTexture(Texture::Ptr texture);
		void DetachDepthTexture();

		// Clears all textures to the current global clear color/depth.
		void Clear() const;
		// Clears depth texture to the current global clear depth.
		void ClearDepth() const;
		// Clears depth texture to [depth].
		void ClearDepth(float depth) const;
		// Clears all color textures to the current global clear color.
		void ClearColor() const;
		// Clears the Specified texture to the current global clear color.
		void ClearColor(unsigned index) const;
		// Clears all textures to [color].
		void ClearColor(unsigned index, const vec4& color) const;

		void Bind() const;
		static void UnBind();

		// Copies the internal multisampled textures to the target while also consolidating the samples.
		// This must be done because multisample textures cannot be used directly.
		// target should be a RenderTarget created with MakeResolve().
		void ResolveMultisampling(const RenderTarget& target) const;
		void ResolveMultisamplingDepth(const RenderTarget& target) const;
		void ResolveMultisamplingColor(const RenderTarget& target) const;

		// Resizes all internal textures to the new resolution. Texture data will be lost.
		bool Resize(unsigned newWidth, unsigned newHeight);

		void CopyDepth(const RenderTarget& target) const;
		void CopyColor(const RenderTarget& target, unsigned index) const;

		void CopyDepthToBackBuffer() const;
		void CopyColorToBackBuffer(unsigned index) const;

		// Retrieves a pixel from the specified color buffer.
		// * This is not recommended for real-time use because it can be very expensive *
		vec4 ReadPixel(unsigned index, const vec2& position) const;

		bool HasDepth() const;
		bool IsMultisampled() const;
		unsigned GetNumColorTextures() const;
		unsigned GetNumSamples() const;

		Texture::Ptr GetDepthTexture() const;
		Texture::Ptr GetColorTexture(unsigned index) const;

		// Returns a viewport matching the dimensions of the render target.
		Viewport GetViewport() const;
		unsigned GetWidth() const;
		unsigned GetHeight() const;

	private:
		unsigned FBO;
		unsigned numColorTextures;
		unsigned numSamples;
		unsigned width;
		unsigned height;

		Texture::Ptr depth;
		Texture::Ptr* colors = nullptr;
	};
}
