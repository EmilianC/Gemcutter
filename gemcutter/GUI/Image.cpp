// Copyright (c) 2021 Emilian Cioca
#include "Image.h"
#include "gemcutter/Rendering/Sprite.h"

namespace gem
{
	Image::Image(Entity& _owner)
		: Widget(_owner)
	{
		auto material = Material::MakeNew();
		material->depthMode = DepthFunc::None;
		material->shader = Load<Shader>("Shaders/Default/Sprite");

		sprite = &owner.Require<Sprite>();
		sprite->SetAlignment(Alignment::Center);
		sprite->SetMaterial(std::move(material));
	}

	Image::Image(Entity& _owner, Texture::Ptr texture)
		: Image(_owner)
	{
		sprite->GetMaterial().textures.Add(std::move(texture));
	}

	Image::Image(Entity& _owner, Material::Ptr material)
		: Widget(_owner)
	{
		sprite = &owner.Require<Sprite>();
		sprite->SetAlignment(Alignment::Center);
		sprite->SetMaterial(std::move(material));
	}

	Sprite& Image::GetSprite()
	{
		return *sprite;
	}

	const Sprite& Image::GetSprite() const
	{
		return *sprite;
	}

	void Image::SetTexture(Texture::Ptr texture)
	{
		sprite->GetMaterial().textures.Add(std::move(texture));
	}

	Texture::Ptr Image::GetTexture() const
	{
		const std::vector<TextureSlot>& textureSlots = sprite->GetMaterial().textures.GetAll();

		return textureSlots.empty() ? nullptr : textureSlots[0].tex;
	}

	void Image::SetMaterial(Material::Ptr material)
	{
		sprite->SetMaterial(std::move(material));
	}

	Material& Image::GetMaterial() const
	{
		return sprite->GetMaterial();
	}

	void Image::UpdateContent()
	{
		const Rectangle& bounds = GetAbsoluteBounds();

		owner.scale.x = bounds.width;
		owner.scale.y = bounds.height;
	}
}

REFLECT(gem::Image) BASES { REF_BASE(gem::Widget) } REF_END;
