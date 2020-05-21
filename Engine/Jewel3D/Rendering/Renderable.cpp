// Copyright (c) 2020 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Renderable.h"

namespace Jwl
{
	Renderable::Renderable(Entity& _owner)
		: Component(_owner)
	{
		material = Material::MakeNew();
		material->shader = Shader::MakeNewPassThrough();
	}

	Renderable::Renderable(Entity& _owner, Material::Ptr _material)
		: Component(_owner)
		, material(std::move(_material))
	{
	}
}
