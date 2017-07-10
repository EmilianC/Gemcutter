// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Model.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Utilities/ScopeGuard.h"
#include "Jewel3D/Utilities/String.h"

#include <GLEW/GL/glew.h>

namespace Jwl
{
	Model::~Model()
	{
		Unload();
	}

	bool Model::Load(std::string filePath)
	{
		auto ext = ExtractFileExtension(filePath);
		if (ext.empty())
		{
			filePath += ".model";
		}
		else if (!CompareLowercase(ext, ".model"))
		{
			Error("Model: ( %s )\nAttempted to load unknown file type as a mesh.", filePath.c_str());
			return false;
		}

		// Load binary file.
		FILE* binaryFile = fopen(filePath.c_str(), "rb");
		if (binaryFile == nullptr)
		{
			Error("Model: ( %s )\nUnable to open file.", filePath.c_str());
			return false;
		}
		defer { fclose(binaryFile); };

		fread(&numFaces, sizeof(s32), 1, binaryFile);
		// Which attributes are enabled?
		fread(&hasUvs, sizeof(bool), 1, binaryFile);
		fread(&hasNormals, sizeof(bool), 1, binaryFile);

		// Determine size of file.
		numVertices = numFaces * 3;
		s32 bufferSize = numVertices * 3;
		if (hasUvs) bufferSize += numVertices * 2;
		if (hasNormals) bufferSize += numVertices * 3;

		// Read the data buffer.
		f32* data = static_cast<f32*>(malloc(sizeof(f32) * bufferSize));
		defer { free(data); };
		fread(data, sizeof(f32), bufferSize, binaryFile);

		// Send data to OpenGL.
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * bufferSize, data, GL_STATIC_DRAW);

		// Determine the stride.
		s32 stride = sizeof(f32) * 3;
		if (hasUvs) stride += sizeof(f32) * 2;
		if (hasNormals) stride += sizeof(f32) * 3;

		// Vertex attribute  : 0
		// Texture attribute : 1
		// Normal attribute  : 2
		glEnableVertexAttribArray(0);
		if (hasUvs) glEnableVertexAttribArray(1);
		if (hasNormals) glEnableVertexAttribArray(2);

		glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
		if (hasUvs) glVertexAttribPointer(1u, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(sizeof(f32) * 3));
		if (hasNormals) glVertexAttribPointer(2u, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(sizeof(f32) * (hasUvs ? 5 : 3)));

		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
		glBindVertexArray(GL_NONE);

		return true;
	}

	void Model::Unload()
	{
		if (VBO != GL_NONE)
		{
			glDeleteBuffers(1, &VBO);
			VBO = GL_NONE;
		}
		
		if (VAO != GL_NONE)
		{
			glDeleteVertexArrays(1, &VAO);
			VAO = GL_NONE;
		}

		hasUvs = false;
		hasNormals = false;

		numFaces = 0;
		numVertices = 0;
	}

	u32 Model::GetVAO() const
	{
		return VAO;
	}

	bool Model::HasUVs() const
	{
		return hasUvs;
	}

	bool Model::HasNormals() const
	{
		return hasNormals;
	}

	u32 Model::GetNumFaces() const
	{
		return numFaces;
	}

	u32 Model::GetNumVerticies() const
	{
		return numVertices;
	}
}
