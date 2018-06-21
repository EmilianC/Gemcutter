// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Model.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Utilities/ScopeGuard.h"
#include "Jewel3D/Utilities/String.h"

namespace Jwl
{
	bool Model::Load(std::string filePath)
	{
		return Load(std::move(filePath), VertexBufferUsage::Static);
	}

	bool Model::Load(std::string filePath, VertexBufferUsage usage)
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

		int numVertices;
		fread(&minBounds, sizeof(vec3), 1, binaryFile);
		fread(&maxBounds, sizeof(vec3), 1, binaryFile);
		fread(&hasUvs, sizeof(bool), 1, binaryFile);
		fread(&hasNormals, sizeof(bool), 1, binaryFile);
		fread(&hasTangents, sizeof(bool), 1, binaryFile);
		fread(&numVertices, sizeof(int), 1, binaryFile);

		// Determine mesh properties.
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

		vertexArray = VertexArray::MakeNew();

		// Read the data buffer from the file.
		VertexBuffer& buffer = vertexArray->CreateBuffer(sizeof(float) * bufferSize, usage);
		auto* data = buffer.MapBuffer(VertexAccess::WriteOnly);
		fread(data, sizeof(float), bufferSize, binaryFile);
		buffer.UnmapBuffer();

		// Set streams...
		VertexStream stream = {};
		stream.bindingUnit = 0;
		stream.bufferSource = 0;
		stream.format = VertexFormat::Float;
		stream.numElements = 3;
		stream.startOffset = 0;
		stream.stride = stride;

		vertexArray->AddStream(stream);
		stream.startOffset += sizeof(float) * 3;

		if (hasUvs)
		{
			stream.bindingUnit = 1;
			stream.numElements = 2;

			vertexArray->AddStream(stream);
			stream.startOffset += sizeof(float) * 2;
		}
		if (hasNormals)
		{
			stream.bindingUnit = 2;
			stream.numElements = 3;

			vertexArray->AddStream(stream);
			stream.startOffset += sizeof(float) * 3;
		}
		if (hasTangents)
		{
			stream.bindingUnit = 3;
			stream.numElements = 4;

			vertexArray->AddStream(stream);
		}

		vertexArray->SetVertexCount(numVertices);

		return true;
	}

	const vec3& Model::GetMinBounds() const
	{
		return minBounds;
	}

	const vec3& Model::GetMaxBounds() const
	{
		return maxBounds;
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

	VertexArray::Ptr& Model::GetVertexArray()
	{
		return vertexArray;
	}
}
