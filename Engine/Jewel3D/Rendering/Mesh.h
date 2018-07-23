// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Entity/Entity.h"
#include "Jewel3D/Resource/VertexArray.h"

namespace Jwl
{
	// Causes an entity to render as a 3D mesh.
	class Mesh : public Component<Mesh>
	{
	public:
		Mesh(Entity& owner);
		Mesh(Entity& owner, VertexArray::Ptr array);

		VertexArray::Ptr array;
	};
}
