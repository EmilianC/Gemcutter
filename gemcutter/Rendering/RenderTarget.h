// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Resource/Shareable.h"
#include "gemcutter/Resource/Texture.h"

namespace gem
{
	struct vec4;
	struct Viewport;

	// Contains a series of textures that can be rendered onto.
	// The initialization process is:
	// {
	//    renderTarget = RenderTarget::MakeNew();
	//    renderTarget->Init(width, height, numColorTextures, hasDepth);
	//
	//    renderTarget->InitTexture(0, format0, filter0);
	//    renderTarget->InitTexture(1, format1, filter1);
	//    // ... once for each color texture requested.
	//
	//    bool success = renderTarget->Validate();
	// }
	// Although each intermediate function returns a bool state, it is safe to
	// reduce code bloat and simply check the final state returned by Validate().
	class RenderTarget : public Shareable<RenderTarget>
	{
	public:
		RenderTarget() = default;
		~RenderTarget();

		RenderTarget(const RenderTarget&) = delete;
		RenderTarget(RenderTarget&&) = delete;
		RenderTarget& operator=(const RenderTarget&) = delete;
		RenderTarget& operator=(RenderTarget&&) = delete;

		// Needs to be called first to set the initial state, including the expected number of color textures.
		bool Init(unsigned width, unsigned height, unsigned numColorTextures, bool hasDepth, unsigned numSamples = 1);
		// Allocates and initializes the texture slot specified by index. Must be called on each slot before use.
		bool InitTexture(unsigned index, TextureFormat format, TextureFilter filter);
		// Should be called after all initialization is done to ensure the RenderTarget was created successfully.
		bool Validate() const;

		// Creates a non multisampled version of this RenderTarget to be used with the ResolveMultisampling() functions.
		RenderTarget::Ptr MakeResolve() const;

		void Unload();

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
		// Target should be a RenderTarget created previously with MakeResolve().
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
		// * This is very expensive and not recommended for real-time use *
		void ReadPixel(unsigned index, int x, int y, std::byte& outR, std::byte& outG, std::byte& outB, std::byte& outA) const;
		void ReadPixel(unsigned index, int x, int y, unsigned short& outR, unsigned short& outG, unsigned short& outB, unsigned short& outA) const;
		void ReadPixel(unsigned index, int x, int y, unsigned& outR, unsigned& outG, unsigned& outB, unsigned& outA) const;
		void ReadPixel(unsigned index, int x, int y, vec4& out) const;

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
		friend class ApplicationSingleton; // For InitDrawFlags().
		static void InitDrawFlags();

		unsigned FBO = 0;
		unsigned numColorTextures = 0;
		unsigned numSamples = 0;
		unsigned width = 0;
		unsigned height = 0;

		Texture::Ptr depth;
		Texture::Ptr* colors = nullptr;
	};
}
