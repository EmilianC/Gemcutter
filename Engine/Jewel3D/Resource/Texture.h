// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Resource.h"
#include "Jewel3D/Rendering/Rendering.h"

#include <string>
#include <vector>

namespace Jwl
{
	class Texture : public Resource<Texture>
	{
	public:
		Texture() = default;
		~Texture();

		// Creates an empty texture.
		void CreateTexture(
			u32 width, u32 height,
			TextureFormat format,
			TextureFilterMode filter = TextureFilterMode::Point,
			TextureWrapModes wrapModes = TextureWrapMode::Clamp,
			f32 anisotropicLevel = 1.0f,
			u32 numSamples = 1);

		// Loads packed *.texture resources, as well as *.png, *.jpg, *.tga, *.bmp.
		bool Load(std::string filePath);
		void Unload();

		void Bind(u32 slot);
		void UnBind(u32 slot);

		void SetFilterMode(TextureFilterMode filter);
		void SetWrapModes(TextureWrapModes wrapModes);
		// Must be in the range of [1, 16].
		void SetAnisotropicLevel(f32 level);

		u32 GetHandle() const;
		u32 GetNumSamples() const;
		u32 GetBindingTarget() const;
		u32 GetWidth() const;
		u32 GetHeight() const;
		TextureFormat GetTextureFormat() const;
		TextureFilterMode GetFilterMode() const;
		TextureWrapModes GetWrapModes() const;
		f32 GetAnisotropicLevel() const;

		bool IsCubeMap() const;

		void RegenerateMipmaps();

	private:
		u32 hTex		= 0;
		u32 numSamples = 1;
		u32 target		= 0;
		s32 width			= 0;
		s32 height			= 0;
		TextureFormat format = TextureFormat::RGB_8;
		TextureFilterMode filter = TextureFilterMode::Point;
		TextureWrapModes wrapModes = TextureWrapMode::Clamp;
		f32 anisotropicLevel = 1.0f;
	};

	//- Used to associate a Texture with a particular binding point.
	struct TextureSlot
	{
		TextureSlot() = default;
		TextureSlot(Texture::Ptr tex, u32 unit = 0);

		void Bind() const;
		void UnBind() const;

		Texture::Ptr tex;
		//- Unit the texture should be bound to when rendering.
		u32 unit = 0;
	};
	
	//- A group of textures that are bound and unbound together.
	class TextureList
	{
	public:
		void Bind() const;
		void UnBind() const;

		void Add(Texture::Ptr tex, u32 unit = 0);
		void Remove(u32 unit);
		//- Removes all TextureSlots.
		void Clear();

		//- Returns the buffer bound at the specified unit.
		Texture::Ptr& operator[](u32 unit);

		const auto& GetAll() const { return textureSlots; }

	private:
		std::vector<TextureSlot> textureSlots;
	};

	//- Loads raw image data from the disk.
	//- Can be used when an image is needed for non-rendering use.
	class Image
	{
	public:
		Image() = default;
		Image(s32 width, s32 height, TextureFormat format, u8* data);
		~Image();

		// Loads *.png, *.jpg, *.tga, and *.bmp.
		static Image Load(const std::string& file, bool flipY = true);

		const s32 width = 0;
		const s32 height = 0;
		const TextureFormat format = TextureFormat::RGB_8;
		const u8* data = nullptr;
	};
}
