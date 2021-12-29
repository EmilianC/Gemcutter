// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Rendering/Renderable.h"
#include "gemcutter/Resource/Model.h"

namespace gem
{
	// Causes an entity to render as a 3D mesh.
	// Can use loaded *.models as well as custom VertexArrays.
	class Mesh : public Renderable
	{
	public:
		Mesh(Entity& owner);
		Mesh(Entity& owner, Model::Ptr model);
		Mesh(Entity& owner, Model::Ptr model, Material::Ptr material);
		Mesh(Entity& owner, Material::Ptr material);
		Mesh(Entity& owner, VertexArray::Ptr array);
		Mesh(Entity& owner, VertexArray::Ptr array, Material::Ptr material);

		void SetModel(Model::Ptr model);
		Model* GetModel() const;

	private:
		Model::Ptr model;
	};
}
