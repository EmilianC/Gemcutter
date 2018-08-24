// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Material.h"
#include "Sprite.h"

namespace Jwl
{
	Sprite::Sprite(Entity& owner)
		: Component(owner)
	{
	}

	Sprite::Sprite(Entity& owner, Alignment pivot)
		: Component(owner)
	{
		SetAlignment(pivot);
	}

	Sprite::Sprite(Entity& owner, Alignment pivot, bool billBoarded)
		: Component(owner)
	{
		SetAlignment(pivot);
		SetBillBoarded(billBoarded);
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

		auto& mat = owner.Require<Material>();
		mat.variantDefinitions.Switch("JWL_SPRITE_CENTERED_X", pivot == Alignment::Center || pivot == Alignment::BottomCenter);
		mat.variantDefinitions.Switch("JWL_SPRITE_CENTERED_Y", pivot == Alignment::Center || pivot == Alignment::LeftCenter);

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

		owner.Require<Material>().variantDefinitions.Switch("JWL_SPRITE_BILLBOARD", state);

		billBoarded = state;
	}

	bool Sprite::GetBillBoarded() const
	{
		return billBoarded;
	}
}
