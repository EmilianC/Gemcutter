// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Entity/Entity.h"

namespace Jwl
{
	class Sprite : public Component<Sprite>
	{
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
