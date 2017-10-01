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

		fread(&numFaces, sizeof(int), 1, binaryFile);
		fread(&hasUvs, sizeof(bool), 1, binaryFile);
		fread(&hasNormals, sizeof(bool), 1, binaryFile);
		fread(&hasTangents, sizeof(bool), 1, binaryFile);

		// Determine mesh properties.
		numVertices = numFaces * 3;
		int bufferSize = numVertices * 3;
		int stride = sizeof(float) * 3;
		if (hasUvs)
		{
			bufferSize += numVertices * 2;
			stride += sizeof(float) * 2;
		}
		if (hasNormals)
		{
			bufferSize += numVertices * 3;
			stride += sizeof(float) * 3;
		}
		if (hasTangents)
		{
			bufferSize += numVertices * 4;
			stride += sizeof(float) * 4;
		}

		// Read the data buffer.
		float* data = static_cast<float*>(malloc(sizeof(float) * bufferSize));
		defer { free(data); };
		fread(data, sizeof(float), bufferSize, binaryFile);

		// Send data to OpenGL.
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bufferSize, data, GL_STATIC_DRAW);

		unsigned startOffset = 0;
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(startOffset));
		startOffset += sizeof(float) * 3;

		if (hasUvs)
		{
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1u, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(startOffset));
			startOffset += sizeof(float) * 2;
		}
		
		if (hasNormals)
		{
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2u, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(startOffset));
			startOffset += sizeof(float) * 3;
		}

		if (hasTangents)
		{
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3u, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(startOffset));
			startOffset += sizeof(float) * 4;
		}

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
		hasTangents = false;

		numFaces = 0;
		numVertices = 0;
	}

	unsigned Model::GetVAO() const
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

	bool Model::HasTangents() const
	{
		return hasTangents;
	}

	unsigned Model::GetNumFaces() const
	{
		return numFaces;
	}

	unsigned Model::GetNumVerticies() const
	{
		return numVertices;
	}
}
