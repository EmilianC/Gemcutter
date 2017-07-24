// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Entity/Entity.h"

namespace Jwl
{
	// Causes an entity to render as a 2D textured sprite.
	// The desired textures should be set on the Material component.
	class Sprite : public Component<Sprite>
	{
	public:
		Sprite(Entity& owner);
		Sprite& operator=(const Sprite&);

		void SetCenteredX(bool state);
		void SetCenteredY(bool state);
		bool GetCenteredX() const;
		bool GetCenteredY() const;

		// Defines "JWL_SPRITE_BILLBOARD" on the entity's shader.
		// If the standard sprite shader is used, this causes the Sprite to face the camera.
		void SetBillBoarded(bool state);
		bool GetBillBoarded() const;

	private:
		bool billBoarded = false;
		bool centeredX = false;
		bool centeredY = false;
	};
}
