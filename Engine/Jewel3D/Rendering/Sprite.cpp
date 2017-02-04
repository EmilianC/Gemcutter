// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Material.h"
#include "Sprite.h"
#include "Jewel3D/Application/Logging.h"

namespace Jwl
{
	Sprite::Sprite(Entity& owner)
		: Component(owner)
	{
	}

	Sprite& Sprite::operator=(const Sprite& other)
	{
		SetBillBoarded(other.billBoarded);
		SetCenteredX(other.centeredX);
		SetCenteredY(other.centeredY);
		
		return *this;
	}

	void Sprite::SetCenteredX(bool state)
	{
		if (centeredX == state)
		{
			return;
		}

		owner.Get<Material>().variantDefinitions.Switch("JWL_SPRITE_CENTERED_X", state);

		centeredX = state;
	}

	void Sprite::SetCenteredY(bool state)
	{
		if (centeredY == state)
		{
			return;
		}

		owner.Get<Material>().variantDefinitions.Switch("JWL_SPRITE_CENTERED_Y", state);

		centeredY = state;
	}

	bool Sprite::GetCenteredX() const
	{
		return centeredX;
	}

	bool Sprite::GetCenteredY() const
	{
		return centeredY;
	}

	void Sprite::SetBillBoarded(bool state)
	{
		if (billBoarded == state)
		{
			return;
		}

		owner.Get<Material>().variantDefinitions.Switch("JWL_SPRITE_BILLBOARD", state);

		billBoarded = state;
	}

	bool Sprite::GetBillBoarded() const
	{
		return billBoarded;
	}
}
