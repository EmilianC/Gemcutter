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
		bool Load(std::string filePath);
		void Unload();

		u32 GetVAO() const;
		bool HasUVs() const;
		bool HasNormals() const;
		u32 GetNumFaces() const;
		u32 GetNumVerticies() const;

	private:
		/* OpenGL buffers */
		u32 VBO = 0;
		u32 VAO = 0;

		bool hasUvs		= false;
		bool hasNormals	= false;

		u32 numFaces	 = 0;
		u32 numVertices = 0;
	};
}
