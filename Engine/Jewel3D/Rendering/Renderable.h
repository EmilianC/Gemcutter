// Copyright (c) 2020 Emilian Cioca
#pragma once
#include "Jewel3D/Entity/Entity.h"
#include "Jewel3D/Resource/Material.h"

namespace Jwl
{
	// Base class for all renderable components.
	class Renderable : public Component<Renderable>
	{
	public:
		Renderable(Entity& owner);

		Material::Ptr material;
	};
}
