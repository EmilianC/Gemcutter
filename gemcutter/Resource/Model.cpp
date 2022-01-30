// Copyright (c) 2017 Emilian Cioca
#include "Model.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Utilities/ScopeGuard.h"

namespace gem
{
	bool Model::Load(std::string_view filePath)
	{
		// Load binary file.
		FILE* binaryFile = fopen(filePath.data(), "rb");
		if (binaryFile == nullptr)
		{
			Error("Model: ( %s )\nUnable to open file.", filePath.data());
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

		array = VertexArray::MakeNew();
		auto buffer = VertexBuffer::MakeNew(sizeof(float) * bufferSize, VertexBufferUsage::Static, VertexBufferType::Data);

		{ // Read the data buffer from the file.
			auto mapping = buffer->MapBuffer(VertexAccess::WriteOnly);
			fread(mapping.GetPtr(), sizeof(float), bufferSize, binaryFile);
		}

		// Enable vertex attribute streams.
		VertexStream stream {
			.buffer       = std::move(buffer),
			.bindingUnit  = 0,
			.format       = VertexFormat::Vec3,
			.normalized   = false,
			.startOffset  = 0,
			.stride       = static_cast<unsigned>(stride)
		};

		array->AddStream(stream);
		stream.startOffset += sizeof(float) * 3;

		if (hasUvs)
		{
			stream.bindingUnit = 1;
			stream.format = VertexFormat::Vec2;

			array->AddStream(stream);
			stream.startOffset += sizeof(float) * 2;
		}
		if (hasNormals)
		{
			stream.bindingUnit = 2;
			stream.format = VertexFormat::Vec3;

			array->AddStream(stream);
			stream.startOffset += sizeof(float) * 3;
		}
		if (hasTangents)
		{
			stream.bindingUnit = 3;
			stream.format = VertexFormat::Vec4;

			array->AddStream(std::move(stream));
		}

		array->SetVertexCount(numVertices);

		return true;
	}

	VertexArray::Ptr Model::GetArray() const
	{
		return array;
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
}
