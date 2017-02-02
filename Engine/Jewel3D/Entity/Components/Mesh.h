// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Entity/Entity.h"
#include "Jewel3D/Resource/Model.h"

namespace Jwl
{
	class Mesh : public Component<Mesh>
	{
		REFLECT_PRIVATE;
	public:
		Mesh(Entity& owner);
		Mesh(Entity& owner, Model::Ptr model);

		void AddData(Model::Ptr model);
		Model::Ptr GetData() const;

	private:
		Model::Ptr data;
	};
}

REFLECT(Jwl::Mesh) < Component >,
	MEMBERS<
		REF_MEMBER(data)<>
	>
REF_END;
