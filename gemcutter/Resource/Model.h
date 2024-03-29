// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Math/Vector.h"
#include "gemcutter/Resource/Resource.h"
#include "gemcutter/Resource/VertexArray.h"

namespace gem
{
	// A 3D model resource. Can be attached to an Entity's Mesh component.
	//
	// Provides the following attributes and bindings:
	//	Vertex  : 0
	//	UVs     : 1
	//	Normal  : 2
	//	Tangent : 3
	class Model : public Resource<Model>, public Shareable<Model>
	{
	public:
		static constexpr std::string_view Extension = ".model";

		// Loads pre-packed *.model resources.
		bool Load(std::string_view filePath);

		VertexArray::Ptr GetArray() const;

		// Returns the extents of each axis in local-space.
		const vec3& GetMinBounds() const;
		const vec3& GetMaxBounds() const;

		bool HasUVs() const;
		bool HasNormals() const;
		bool HasTangents() const;

	private:
		VertexArray::Ptr array;

		vec3 minBounds;
		vec3 maxBounds;

		bool hasUvs = false;
		bool hasNormals = false;
		bool hasTangents = false;
	};
}
