// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Resource.h"
#include "Jewel3D/Reflection/Reflection.h"
#include "Jewel3D/Rendering/Rendering.h"

#include <string>
#include <vector>

namespace Jwl
{
	class Texture : public Resource<Texture>
	{
		REFLECT_PRIVATE;
	public:
		Texture() = default;
		~Texture();

		void CreateTexture(unsigned width, unsigned height, TextureFormat format, TextureFilterMode filter = defaultFilterMode, unsigned numSamples = 1);
		bool Load(const std::string& filePath, TextureFilterMode filter = defaultFilterMode);
		bool LoadCubeMap(
			const std::string& filePathPosX, const std::string& filePathNegX,
			const std::string& filePathPosY, const std::string& filePathNegY,
			const std::string& filePathPosZ, const std::string& filePathNegZ,
			TextureFilterMode filter = defaultFilterMode);
		void Unload();

		void Bind(unsigned slot);
		void UnBind(unsigned slot);

		void SetWrapMode(TextureWrapMode wrap);
		void SetWrapModeS(TextureWrapMode wrap);
		void SetWrapModeT(TextureWrapMode wrap);
		void SetWrapModeR(TextureWrapMode wrap);
		void SetFilterMode(TextureFilterMode filter);
		void SetFilterModeMin(TextureFilterMode filter);
		void SetFilterModeMag(TextureFilterMode filter);
		void SetAnisotropicLevel(float level);

		unsigned GetHandle() const;
		unsigned GetWidth() const;
		unsigned GetHeight() const;
		unsigned GetNumChannels() const;
		unsigned GetNumSamples() const;
		TextureFormat GetTextureFormat() const;
		unsigned GetBindingTarget() const;
		bool IsCubeMap() const;

		void RegenerateMipmaps();

		static void SetDefaultFilterMode(TextureFilterMode filterMode);
		static void SetDefaultAnisotropicLevel(float level);
		static TextureFilterMode GetDefaultFilterMode();
		static float GetDefaultAnisotropicLevel();

	private:
		unsigned hTex		= 0;
		unsigned numSamples = 1;
		unsigned target		= 0;
		int numChannels		= 0;
		int width			= 0;
		int height			= 0;
		TextureFormat format;

		static TextureFilterMode defaultFilterMode;
		static float defaultAnisotropicLevel;
	};

	//- Used to associate a Texture with a particular binding point.
	struct TextureSlot
	{
		TextureSlot() = default;
		TextureSlot(Texture::Ptr tex, unsigned unit = 0);

		void Bind() const;
		void UnBind() const;

		Texture::Ptr tex;
		//- Unit the texture should be bound to when rendering.
		unsigned unit = 0;
	};
	
	//- A group of textures that are bound and unbound together.
	class TextureList
	{
		REFLECT_PRIVATE;
	public:
		void Bind() const;
		void UnBind() const;

		void Add(Texture::Ptr tex, unsigned unit = 0);
		void Remove(unsigned unit);
		//- Removes all TextureSlots.
		void Clear();

		//- Returns the buffer bound at the specified unit.
		Texture::Ptr& operator[](unsigned unit);

		const auto& GetAll() const { return textureSlots; }

	private:
		std::vector<TextureSlot> textureSlots;
	};
}

REFLECT_SHAREABLE(Jwl::Texture)
REFLECT(Jwl::Texture) < Resource >,
	MEMBERS<
		REF_MEMBER(numSamples)< ReadOnly >,
		REF_MEMBER(numChannels)< ReadOnly >,
		REF_MEMBER(width)< ReadOnly >,
		REF_MEMBER(height)< ReadOnly >,
		REF_MEMBER(format)< ReadOnly >
	>
REF_END;

REFLECT(Jwl::TextureSlot)<>,
	MEMBERS<
		REF_MEMBER(tex)<>,
		REF_MEMBER(unit)<>
	>
REF_END;

REFLECT_BASIC(std::vector<Jwl::TextureSlot>)
REFLECT(Jwl::TextureList)<>,
	MEMBERS<
		REF_MEMBER(textureSlots)<>
	>
REF_END;
