// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Entity/Entity.h"
#include "Jewel3D/Resource/Model.h"

namespace Jwl
{
	// Causes an entity to render as a 3D mesh.
	class Mesh : public Component<Mesh>
	{
	public:
		Mesh(Entity& owner);
		Mesh(Entity& owner, Model::Ptr model);

		Model::Ptr model;
	};
}
