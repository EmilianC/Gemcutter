// Copyright (c) 2021 Emilian Cioca
#pragma once
#include "gemcutter/GUI/Widget.h"
#include "gemcutter/Resource/Material.h"
#include "gemcutter/Resource/Texture.h"

namespace gem
{
	class Sprite;

	// A widget containing a sprite scaled to fit within it.
	class Image : public Widget
	{
	public:
		Image(Entity& owner);
		Image(Entity& owner, Texture::Ptr texture);
		Image(Entity& owner, Material::Ptr material);

		Sprite& GetSprite();
		const Sprite& GetSprite() const;

		void SetTexture(Texture::Ptr texture);
		Texture::Ptr GetTexture() const;

		void SetMaterial(Material::Ptr material);
		Material& GetMaterial() const;

	private:
		void UpdateContent() override;

		Sprite* sprite = nullptr;
	};
}
