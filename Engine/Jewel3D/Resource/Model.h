// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Resource.h"
#include "Shareable.h"
#include "VertexArray.h"
#include "Jewel3D/Math/Vector.h"
#include "Jewel3D/Reflection/Reflection.h"

namespace Jwl
{
	// A 3D model resource. Can be attached to an Entity's Mesh component.
	//
	// Provides the following attributes and bindings:
	//	Vertex  : 0
	//	UVs     : 1
	//	Normal  : 2
	//	Tangent : 3
	class Model : public VertexArray, public Resource<Model>
	{
		REFLECT_PRIVATE;
	public:
		// Loads pre-packed *.model resources.
		bool Load(std::string filePath);
		bool Load(std::string filePath, VertexBufferUsage usage);

		// Returns the extents of each axis in local-space.
		const vec3& GetMinBounds() const;
		const vec3& GetMaxBounds() const;

		bool HasUVs() const;
		bool HasNormals() const;
		bool HasTangents() const;

	private:
		vec3 minBounds;
		vec3 maxBounds;

		bool hasUvs = false;
		bool hasNormals = false;
		bool hasTangents = false;
	};
}

REFLECT(Jwl::Model) < Resource >,
	BASES< Jwl::VertexArray >,
	MEMBERS<
		REF_MEMBER(hasUvs)< ReadOnly >,
		REF_MEMBER(hasNormals)< ReadOnly >,
		REF_MEMBER(hasTangents)< ReadOnly >
	>
REF_END;
