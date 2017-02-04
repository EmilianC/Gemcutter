// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Mesh.h"

namespace Jwl
{
	Mesh::Mesh(Entity& _owner)
		: Component(_owner)
	{
	}

	Mesh::Mesh(Entity& _owner, Model::Ptr model)
		: Component(_owner)
	{
		AddData(model);
	}

	void Mesh::AddData(Model::Ptr model)
	{
		data = model;
	}

	Model::Ptr Mesh::GetData() const
	{
		return data;
	}
}
