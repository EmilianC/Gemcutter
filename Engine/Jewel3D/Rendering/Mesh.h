// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Entity/Entity.h"
#include "Jewel3D/Resource/Model.h"

namespace Jwl
{
	class Mesh : public Component<Mesh>
	{
	public:
		Mesh(Entity& owner);
		Mesh(Entity& owner, Model::Ptr model);

		void AddData(Model::Ptr model);
		Model::Ptr GetData() const;

	private:
		Model::Ptr data;
	};
}
