// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Rendering/Renderable.h"
#include "gemcutter/Resource/VertexArray.h"

namespace gem
{
	// Causes an entity to render as a 3D mesh.
	class Mesh : public Renderable
	{
	public:
		Mesh(Entity& owner);
		Mesh(Entity& owner, VertexArray::Ptr array);
		Mesh(Entity& owner, Material::Ptr material);
		Mesh(Entity& owner, VertexArray::Ptr array, Material::Ptr material);

		VertexArray::Ptr array;
	};
}
