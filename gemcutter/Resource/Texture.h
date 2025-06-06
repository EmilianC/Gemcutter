// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Rendering/Rendering.h"
#include "gemcutter/Resource/Resource.h"
#include "gemcutter/Resource/Shareable.h"

#include <span>
#include <string_view>
#include <vector>

namespace gem
{
	// A 2D texture, renderTarget, or cubemap.
	class Texture : public Resource<Texture>, public Shareable<Texture>
	{
	public:
		static constexpr std::string_view Extension = ".texture";

		~Texture();

		// Creates an empty 2D texture for render targets or custom data.
		bool Create(
			unsigned width, unsigned height,
			TextureFormat format,
			TextureFilter filter = TextureFilter::Point,
			TextureWraps wraps = TextureWrap::Clamp,
			float anisotropicLevel = 1.0f,
			unsigned numSamples = 1);

		// Replaces the texture with the provided raw image data.
		void SetData(const std::byte* data, TextureFormat sourceFormat);

		// Loads packed *.texture resources, as well as *.png, *.jpg, *.tga, *.bmp.
		bool Load(std::string_view filePath);
		void Unload();

		void Bind(unsigned slot);
		void UnBind(unsigned slot);

		void SetFilter(TextureFilter filter);
		void SetWrap(TextureWraps wraps);
		// Must be in the range of [1, 16].
		void SetAnisotropicLevel(float level);
		// Sets whether Percentage Closer Filtering is enabled for depth textures.
		void SetPCF(bool enabled);

		unsigned GetHandle() const;
		unsigned GetNumSamples() const;
		unsigned GetBindingTarget() const;
		unsigned GetWidth() const;
		unsigned GetHeight() const;
		TextureFormat GetFormat() const;
		TextureFilter GetFilter() const;
		TextureWraps GetWrap() const;
		float GetAnisotropicLevel() const;
		float GetAspectRatio() const;

		bool IsCubeMap() const;

		void RegenerateMipmaps();

	private:
		unsigned hTex       = 0;
		unsigned numSamples = 1;
		unsigned target     = 0;
		int width           = 0;
		int height          = 0;
		TextureFormat format = TextureFormat::RGB_8;
		TextureFilter filter = TextureFilter::Point;
		TextureWraps wraps = TextureWrap::Clamp;
		float anisotropicLevel = 1.0f;

	public:
		PRIVATE_MEMBER(Texture, numSamples);
		PRIVATE_MEMBER(Texture, width);
		PRIVATE_MEMBER(Texture, height);
		PRIVATE_MEMBER(Texture, format);
		PRIVATE_MEMBER(Texture, filter);
		PRIVATE_MEMBER(Texture, wraps);
		PRIVATE_MEMBER(Texture, anisotropicLevel);
	};

	// Used to associate a Texture with a particular binding point.
	struct TextureSlot
	{
		void Bind() const;
		void UnBind() const;

		Texture::Ptr tex;
		// Unit the texture should be bound to when rendering.
		unsigned unit = 0;
	};

	// A group of textures that are bound and unbound together.
	class TextureList
	{
	public:
		void Bind() const;
		void UnBind() const;

		void Add(Texture::Ptr tex, unsigned unit = 0);
		void Remove(unsigned unit);
		void Clear();

		size_t Size() const;

		// Returns the buffer bound at the specified unit.
		Texture& operator[](unsigned unit);

		std::span<const TextureSlot> GetAll() const { return textureSlots; }

	private:
		std::vector<TextureSlot> textureSlots;

	public:
		PRIVATE_MEMBER(TextureList, textureSlots);
	};

	// Loads raw image data from the disk.
	// Can be used when an image is needed for non-rendering use.
	class RawImage
	{
	public:
		~RawImage();

		// Loads *.png, *.jpg, *.tga, and *.bmp.
		[[nodiscard]] static RawImage Load(std::string_view file, bool flipY, bool sRGB);

		int width;
		int height;
		TextureFormat format;
		std::byte* data;

	private:
		RawImage(int width, int height, TextureFormat format, std::byte* data);
	};

	// Flips the image contained in `data` on the Y axis.
	void FlipImage(std::byte* data, int width, int height, int numChannels);
}
