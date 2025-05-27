// Copyright (c) 2017 Emilian Cioca
#include "Sprite.h"
#include "gemcutter/Rendering/Primitives.h"

namespace gem
{
	Sprite::Sprite(Entity& _owner)
		: Renderable(_owner, Primitives.GetQuadArray())
	{
	}

	Sprite::Sprite(Entity& _owner, Material::Ptr material)
		: Renderable(_owner, Primitives.GetQuadArray(), std::move(material))
	{
	}

	Sprite::Sprite(Entity& _owner, Alignment pivot)
		: Renderable(_owner, Primitives.GetQuadArray())
	{
		SetAlignment(pivot);
	}

	Sprite::Sprite(Entity& _owner, Alignment pivot, bool billBoard, Material::Ptr material)
		: Renderable(_owner, Primitives.GetQuadArray(), std::move(material))
	{
		SetAlignment(pivot);
		SetBillBoarded(billBoard);
	}

	void Sprite::SetAlignment(Alignment pivot)
	{
		if (alignment == pivot)
			return;

		variants.Switch("GEM_SPRITE_CENTERED_X", pivot == Alignment::Center || pivot == Alignment::BottomCenter);
		variants.Switch("GEM_SPRITE_CENTERED_Y", pivot == Alignment::Center || pivot == Alignment::LeftCenter);

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

		variants.Switch("GEM_SPRITE_BILLBOARD", state);

		billBoarded = state;
	}

	bool Sprite::GetBillBoarded() const
	{
		return billBoarded;
	}
}

REFLECT(gem::Alignment)
	ENUM_VALUES {
		REF_VALUE(BottomLeft)
		REF_VALUE(BottomCenter)
		REF_VALUE(LeftCenter)
		REF_VALUE(Center)
	}
REF_END;

REFLECT_COMPONENT(gem::Sprite, gem::Renderable)
	MEMBERS {
		REF_PRIVATE_MEMBER_EX(alignment,   nullptr, &gem::Sprite::SetAlignment);
		REF_PRIVATE_MEMBER_EX(billBoarded, nullptr, &gem::Sprite::SetBillBoarded);
	}
REF_END;
