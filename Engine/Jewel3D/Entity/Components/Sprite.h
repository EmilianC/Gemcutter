// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Reflection/Reflection.h"
#include "Jewel3D/Entity/Entity.h"

namespace Jwl
{
	class Sprite : public Component<Sprite>
	{
		REFLECT_PRIVATE;
	public:
		Sprite(Entity& owner);
		Sprite& operator=(const Sprite&);

		void SetCenteredX(bool state);
		void SetCenteredY(bool state);
		bool GetCenteredX() const;
		bool GetCenteredY() const;

		void SetBillBoarded(bool state);
		bool GetBillBoarded() const;

	private:
		bool billBoarded = false;
		bool centeredX = false;
		bool centeredY = false;
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
