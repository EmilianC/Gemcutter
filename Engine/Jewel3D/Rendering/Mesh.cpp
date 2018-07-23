// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Mesh.h"

namespace Jwl
{
	Mesh::Mesh(Entity& _owner)
		: Component(_owner)
	{
	}

	Mesh::Mesh(Entity& _owner, VertexArray::Ptr _array)
		: Component(_owner)
		, array(std::move(_array))
	{
	}
}
