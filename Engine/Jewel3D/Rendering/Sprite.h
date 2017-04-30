// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Entity/Entity.h"
#include "Jewel3D/Reflection/Reflection.h"

namespace Jwl
{
	enum class Alignment
	{
		BottomLeft,
		BottomCenter,
		LeftCenter,
		Center
	};

	// Causes an entity to render as a 2D textured sprite.
	// The desired textures should be set on the Material component.
	// Alignment and BillBoard options are supported by the default sprite shader.
	class Sprite : public Component<Sprite>
	{
		REFLECT_PRIVATE;
	public:
		Sprite(Entity& owner);
		Sprite(Entity& owner, Alignment pivot);
		Sprite(Entity& owner, Alignment pivot, bool billBoarded);
		Sprite& operator=(const Sprite&);

		// The sprite will render with the specified point on the entity's position.
		// Defines "JWL_SPRITE_CENTERED_X" and "JWL_SPRITE_CENTERED_Y" on the Material Component if needed.
		void SetAlignment(Alignment pivot);
		Alignment GetAlignment() const;

		// Causes the Sprite to face the camera.
		// Defines "JWL_SPRITE_BILLBOARD" on the Material Component if needed.
		void SetBillBoarded(bool state);
		bool GetBillBoarded() const;

	private:
		Alignment alignment = Alignment::BottomLeft;
		bool billBoarded = false;
	};
}

REFLECT(Jwl::Sprite) < Component >,
	MEMBERS <
		REF_MEMBER(billBoarded)<
			Setter<Jwl::Sprite, bool, &Jwl::Sprite::SetBillBoarded>
		>,
		REF_MEMBER(centeredX)<
			Setter<Jwl::Sprite, bool, &Jwl::Sprite::SetCenteredX>
		>,
		REF_MEMBER(centeredY)<
			Setter<Jwl::Sprite, bool, &Jwl::Sprite::SetCenteredY>
		>
	>
REF_END;
