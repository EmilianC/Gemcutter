// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Resource.h"
#include "Jewel3D/Reflection/Reflection.h"

namespace Jwl
{
	class Model : public Resource<Model>
	{
		REFLECT_PRIVATE;
	public:
		~Model();

		//- Loads .model files as well as the standard .obj format.
		bool Load(const std::string& filePath);
		void Unload();

		unsigned GetVAO() const;
		bool HasUVs() const;
		bool HasNormals() const;
		unsigned GetNumFaces() const;
		unsigned GetNumVerticies() const;

	private:
		/* OpenGL buffers */
		unsigned VBO = 0;
		unsigned VAO = 0;

		bool hasUvs		= false;
		bool hasNormals	= false;

		unsigned numFaces	 = 0;
		unsigned numVertices = 0;
	};
}

REFLECT_SHAREABLE(Jwl::Model)
REFLECT(Jwl::Model) < Resource >,
	MEMBERS<
		REF_MEMBER(hasUvs)< ReadOnly >,
		REF_MEMBER(hasNormals)< ReadOnly >,
		REF_MEMBER(numFaces)< ReadOnly >,
		REF_MEMBER(numVertices)< ReadOnly >
	>
REF_END;
