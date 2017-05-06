// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Resource.h"

namespace Jwl
{
	class Model : public Resource<Model>
	{
	public:
		~Model();

		//- Loads pre-packed *.model resources.
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
