// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Rendering/Renderable.h"

namespace gem
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
	class Sprite : public Renderable
	{
	public:
		Sprite(Entity& owner);
		Sprite(Entity& owner, Material::Ptr material);
		Sprite(Entity& owner, Alignment pivot);
		Sprite(Entity& owner, Alignment pivot, bool billBoard, Material::Ptr material);
		Sprite& operator=(const Sprite&);

		// The sprite will render with the specified point on the entity's position.
		// Defines "GEM_SPRITE_CENTERED_X" and "GEM_SPRITE_CENTERED_Y" on the Material Component if needed.
		void SetAlignment(Alignment pivot);
		Alignment GetAlignment() const;

		// Causes the Sprite to face the camera.
		// Defines "GEM_SPRITE_BILLBOARD" on the Material Component if needed.
		void SetBillBoarded(bool state);
		bool GetBillBoarded() const;

	private:
		Alignment alignment = Alignment::BottomLeft;
		bool billBoarded = false;
	};
}
