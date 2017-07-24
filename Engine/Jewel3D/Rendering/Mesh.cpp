// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Mesh.h"

namespace Jwl
{
	Mesh::Mesh(Entity& _owner)
		: Component(_owner)
	{
	}

	Mesh::Mesh(Entity& _owner, Model::Ptr _model)
		: Component(_owner)
		, model(_model)
	{
	}
}
