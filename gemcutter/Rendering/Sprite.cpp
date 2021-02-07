// Copyright (c) 2017 Emilian Cioca
#include "Sprite.h"

namespace Jwl
{
	Sprite::Sprite(Entity& owner)
		: Renderable(owner)
	{
	}

	Sprite::Sprite(Entity& owner, Material::Ptr material)
		: Renderable(owner, std::move(material))
	{
	}

	Sprite::Sprite(Entity& owner, Alignment pivot)
		: Renderable(owner)
	{
		SetAlignment(pivot);
	}

	Sprite::Sprite(Entity& owner, Alignment pivot, bool billBoard, Material::Ptr material)
		: Renderable(owner, std::move(material))
	{
		SetAlignment(pivot);
		SetBillBoarded(billBoard);
	}

	Sprite& Sprite::operator=(const Sprite& other)
	{
		SetAlignment(other.alignment);
		SetBillBoarded(other.billBoarded);

		return *this;
	}

	void Sprite::SetAlignment(Alignment pivot)
	{
		if (alignment == pivot)
			return;

		variants.Switch("JWL_SPRITE_CENTERED_X", pivot == Alignment::Center || pivot == Alignment::BottomCenter);
		variants.Switch("JWL_SPRITE_CENTERED_Y", pivot == Alignment::Center || pivot == Alignment::LeftCenter);

		alignment = pivot;
	}

	Alignment Sprite::GetAlignment() const
	{
		return alignment;
	}

	void Sprite::SetBillBoarded(bool state)
	{
		if (billBoarded == state)
			return;

		variants.Switch("JWL_SPRITE_BILLBOARD", state);

		billBoarded = state;
	}

	bool Sprite::GetBillBoarded() const
	{
		return billBoarded;
	}
}
