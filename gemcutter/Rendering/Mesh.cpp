// Copyright (c) 2017 Emilian Cioca
#include "Mesh.h"

namespace gem
{
	Mesh::Mesh(Entity& _owner)
		: Renderable(_owner)
	{
	}

	Mesh::Mesh(Entity& _owner, Model::Ptr _model)
		: Renderable(_owner)
	{
		SetModel(std::move(_model));
	}

	Mesh::Mesh(Entity& _owner, Model::Ptr _model, Material::Ptr material)
		: Renderable(_owner, std::move(material))
	{
		SetModel(std::move(_model));
	}

	Mesh::Mesh(Entity& _owner, Material::Ptr material)
		: Renderable(_owner, std::move(material))
	{
	}

	Mesh::Mesh(Entity& _owner, VertexArray::Ptr _array)
		: Renderable(_owner, std::move(_array))
	{
	}

	Mesh::Mesh(Entity& _owner, VertexArray::Ptr _array, Material::Ptr material)
		: Renderable(_owner, std::move(_array), std::move(material))
	{
	}

	void Mesh::SetModel(Model::Ptr _model)
	{
		model = std::move(_model);
		array = model->GetArray();
	}

	Model* Mesh::GetModel() const
	{
		return model.get();
	}
}

REFLECT_COMPONENT(gem::Mesh, gem::Renderable)
	MEMBERS {
		REF_PRIVATE_MEMBER_EX(model, nullptr, &gem::Mesh::SetModel)
	}
REF_END;
