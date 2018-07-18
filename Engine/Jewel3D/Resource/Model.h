// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Resource.h"
#include "Shareable.h"
#include "Jewel3D\Math\Vector.h"

namespace Jwl
{
	// A 3D model resource.
	// To be rendered, a model must be set on an Entity's Mesh component.
	//
	// Provides the following attributes and bindings:
	//	Vertex  : 0
	//	UVs     : 1
	//	Normal  : 2
	//	Tangent : 3
	class Model : public Resource<Model>, public Shareable<Model>
	{
	public:
		~Model();

		// Loads pre-packed *.model resources.
		bool Load(std::string filePath);
		void Unload();

		unsigned GetVAO() const;

		// Returns the extents of each axis in local-space.
		const vec3& GetMinBounds() const;
		const vec3& GetMaxBounds() const;

		bool HasUVs() const;
		bool HasNormals() const;
		bool HasTangents() const;

		unsigned GetVertexCount() const;

	private:
		// OpenGL buffers.
		unsigned VBO = 0;
		unsigned VAO = 0;

		vec3 minBounds;
		vec3 maxBounds;

		bool hasUvs		 = false;
		bool hasNormals	 = false;
		bool hasTangents = false;

		unsigned numVertices = 0;
	};
}
