// Copyright (c) 2017 Emilian Cioca
#include "MeshEncoder.h"

#include "Jewel3D/Math/Matrix.h"
#include "Jewel3D/Math/Vector.h"
#include "Jewel3D/Resource/Encoder.h"

#include <fstream>
#include <vector>

#define CURRENT_VERSION 2
#define CHAR_BUFFER_SIZE 128

// Indices for three points; one triangle. v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
struct MeshFace
{
	unsigned vertices[3];
	unsigned textures[3];
	unsigned normals[3];

	// Calculated after indices have been read.
	// xyz = normalized tangent, w = handedness.
	Jwl::vec4 tangent[3];
};

MeshEncoder::MeshEncoder()
	: Encoder(CURRENT_VERSION)
{
}

Jwl::ConfigTable MeshEncoder::GetDefault() const
{
	Jwl::ConfigTable defaultConfig;

	defaultConfig.SetValue("version", CURRENT_VERSION);
	defaultConfig.SetValue("scale", 1);
	defaultConfig.SetValue("uvs", true);
	defaultConfig.SetValue("normals", true);
	defaultConfig.SetValue("tangents", true);

	return defaultConfig;
}

bool MeshEncoder::Validate(const Jwl::ConfigTable& metadata, unsigned loadedVersion) const
{
	if (!metadata.HasSetting("uvs"))
	{
		Jwl::Error("Missing \"uvs\" value.");
		return false;
	}

	if (!metadata.HasSetting("scale"))
	{
		Jwl::Error("Missing \"scale\" value.");
		return false;
	}

	if (!metadata.HasSetting("normals"))
	{
		Jwl::Error("Missing \"normals\" value.");
		return false;
	}

	switch (loadedVersion)
	{
	case 1:
		if (metadata.GetSize() != 4)
		{
			Jwl::Error("Incorrect number of value entries.");
			return false;
		}
		break;

	case 2:
		if (!metadata.HasSetting("tangents"))
		{
			Jwl::Error("Missing \"tangents\" value.");
			return false;
		}

		if (metadata.GetSize() != 5)
		{
			Jwl::Error("Incorrect number of value entries.");
			return false;
		}
		break;
	}

	return true;
}

bool MeshEncoder::Convert(std::string_view source, std::string_view destination, const Jwl::ConfigTable& metadata) const
{
	const std::string outputFile = std::string(destination) + Jwl::ExtractFilename(source) + ".model";
	const float scale = metadata.GetFloat("scale");
	const bool packUvs = metadata.GetBool("uvs");
	const bool packNormals = metadata.GetBool("normals");
	const bool packTangents = metadata.GetBool("tangents");

	// Load ASCII file.
	std::ifstream input;
	input.open(source.data());
	if (!input)
	{
		Jwl::Error("Input file could not be opened or processed.");
		return false;
	}

	char inputString[CHAR_BUFFER_SIZE] = { '\0' };
	bool hasUvs = false;
	bool hasNormals = false;

	// Unique data.
	std::vector<Jwl::vec3> vertexData;
	std::vector<Jwl::vec2> textureData;
	std::vector<Jwl::vec3> normalData;
	// Indexing data.
	std::vector<MeshFace> faceData;

	Jwl::vec3 minBounds{ FLT_MAX };
	Jwl::vec3 maxBounds{ FLT_MIN };

	while (!input.eof())
	{
		input.getline(inputString, CHAR_BUFFER_SIZE);

		if (inputString[0] == 'v')
		{
			if (inputString[1] == 't')
			{
				hasUvs = true;

				if (!(packUvs || packTangents))
					continue;

				// Load texture coordinates.
				char* endPtr;
				Jwl::vec2 temp;
				temp.x = std::strtof(inputString + 3, &endPtr);
				temp.y = std::strtof(endPtr + 1, nullptr);

				textureData.push_back(temp);
			}
			else if (inputString[1] == 'n')
			{
				hasNormals = true;

				if (!(packNormals || packTangents))
					continue;

				// Load normals.
				char* endPtr;
				Jwl::vec3 temp;
				temp.x = std::strtof(inputString + 3, &endPtr);
				temp.y = std::strtof(endPtr + 1, &endPtr);
				temp.z = std::strtof(endPtr + 1, nullptr);

				normalData.push_back(temp);
			}
			else
			{
				// Load vertices.
				char* endPtr;
				Jwl::vec3 temp;
				temp.x = std::strtof(inputString + 2, &endPtr);
				temp.y = std::strtof(endPtr + 1, &endPtr);
				temp.z = std::strtof(endPtr + 1, nullptr);

				if (temp.x < minBounds.x) minBounds.x = temp.x;
				if (temp.y < minBounds.y) minBounds.y = temp.y;
				if (temp.z < minBounds.z) minBounds.z = temp.z;

				if (temp.x > maxBounds.x) maxBounds.x = temp.x;
				if (temp.y > maxBounds.y) maxBounds.y = temp.y;
				if (temp.z > maxBounds.z) maxBounds.z = temp.z;

				vertexData.push_back(temp);
			}
		}
		else if (inputString[0] == 'f')
		{
			// Load face indices.
			MeshFace face;
			char* endPtr;

			// Each permutation of data formats.
			if (hasUvs && hasNormals)
			{
				// Format of "f #/#/# #/#/# #/#/#"
				face.vertices[0] = std::strtol(inputString + 2, &endPtr, 10);
				face.textures[0] = std::strtol(endPtr + 1, &endPtr, 10);
				face.normals[0] = std::strtol(endPtr + 1, &endPtr, 10);

				face.vertices[1] = std::strtol(endPtr + 1, &endPtr, 10);
				face.textures[1] = std::strtol(endPtr + 1, &endPtr, 10);
				face.normals[1] = std::strtol(endPtr + 1, &endPtr, 10);

				face.vertices[2] = std::strtol(endPtr + 1, &endPtr, 10);
				face.textures[2] = std::strtol(endPtr + 1, &endPtr, 10);
				face.normals[2] = std::strtol(endPtr + 1, nullptr, 10);
			}
			else if (hasUvs && !hasNormals)
			{
				// Format: "f #/# #/# #/#"
				face.vertices[0] = std::strtol(inputString + 2, &endPtr, 10);
				face.textures[0] = std::strtol(endPtr + 1, &endPtr, 10);

				face.vertices[1] = std::strtol(endPtr + 1, &endPtr, 10);
				face.textures[1] = std::strtol(endPtr + 1, &endPtr, 10);

				face.vertices[2] = std::strtol(endPtr + 1, &endPtr, 10);
				face.textures[2] = std::strtol(endPtr + 1, nullptr, 10);
			}
			else if (!hasUvs && hasNormals)
			{
				// Format: "f #//# #//# #//#"
				face.vertices[0] = std::strtol(inputString + 2, &endPtr, 10);
				face.normals[0] = std::strtol(endPtr + 2, &endPtr, 10);

				face.vertices[1] = std::strtol(endPtr + 1, &endPtr, 10);
				face.normals[1] = std::strtol(endPtr + 2, &endPtr, 10);

				face.vertices[2] = std::strtol(endPtr + 1, &endPtr, 10);
				face.normals[2] = std::strtol(endPtr + 2, nullptr, 10);
			}
			else
			{
				// Format: "f # # #"
				face.vertices[0] = std::strtol(inputString + 2, &endPtr, 10);
				face.vertices[1] = std::strtol(endPtr + 1, &endPtr, 10);
				face.vertices[2] = std::strtol(endPtr + 1, nullptr, 10);
			}

			faceData.push_back(face);
		}
	}

	input.close();

	// Apply scale.
	if (scale != 1.0f)
	{
		for (auto& vertex : vertexData)
		{
			vertex *= scale;
		}
	}

	const bool useUvs = packUvs && hasUvs;
	const bool useNormals = packNormals && hasNormals;
	const bool useTangents = packTangents && hasUvs && hasNormals;

	// Generate tangent vectors.
	if (useTangents)
	{
		for (unsigned i = 0; i < faceData.size(); ++i)
		{
			const Jwl::vec3 edge1 =
				vertexData[faceData[i].vertices[0] - 1] -
				vertexData[faceData[i].vertices[1] - 1];

			const Jwl::vec3 edge2 =
				vertexData[faceData[i].vertices[0] - 1] -
				vertexData[faceData[i].vertices[2] - 1];

			const Jwl::vec2 edgeUV1 =
				textureData[faceData[i].textures[0] - 1] -
				textureData[faceData[i].textures[1] - 1];

			const Jwl::vec2 edgeUV2 =
				textureData[faceData[i].textures[0] - 1] -
				textureData[faceData[i].textures[2] - 1];

			const Jwl::mat2 UVs = Jwl::mat2(
				edgeUV1.x, edgeUV1.y,
				edgeUV2.x, edgeUV2.y).GetInverse();

			const Jwl::vec3 tangent = Jwl::vec3(
				UVs[0] * edge1.x + UVs[2] * edge2.x,
				UVs[0] * edge1.y + UVs[2] * edge2.y,
				UVs[0] * edge1.z + UVs[2] * edge2.z);

			const Jwl::vec3 bitangent = Jwl::vec3(
				UVs[1] * edge1.x + UVs[3] * edge2.x,
				UVs[1] * edge1.y + UVs[3] * edge2.y,
				UVs[1] * edge1.z + UVs[3] * edge2.z);

			// Each face's tangent is further refined per-vertex by using their respective normals.
			for (unsigned v = 0; v < 3; ++v)
			{
				const Jwl::vec3 normal = normalData[faceData[i].normals[v] - 1];

				// Calculate handedness.
				const float handedness = Jwl::Dot(Jwl::Cross(normal, tangent), bitangent) < 0.0f ? -1.0f : 1.0f;

				// Orthonormalize the tangent with respect to the normal.
				const Jwl::vec3 t = Jwl::Normalize(tangent - normal * Jwl::Dot(normal, tangent));

				faceData[i].tangent[v] = Jwl::vec4(t, handedness);
			}
		}
	}

	const unsigned numVertices = faceData.size() * 3;
	unsigned bufferSize = numVertices * 3;
	if (useUvs) bufferSize += numVertices * 2;
	if (useNormals) bufferSize += numVertices * 3;
	if (useTangents) bufferSize += numVertices * 4;

	float* data = static_cast<float*>(malloc(sizeof(float) * bufferSize));
	defer{ free(data); };

	// Unpack the data.
	unsigned pen = 0;
	for (unsigned i = 0; i < faceData.size(); ++i)
	{
		for (unsigned j = 0; j < 3; ++j)
		{
			data[pen++] = vertexData[faceData[i].vertices[j] - 1].x;
			data[pen++] = vertexData[faceData[i].vertices[j] - 1].y;
			data[pen++] = vertexData[faceData[i].vertices[j] - 1].z;

			if (useUvs)
			{
				data[pen++] = textureData[faceData[i].textures[j] - 1].x;
				data[pen++] = textureData[faceData[i].textures[j] - 1].y;
			}

			if (useNormals)
			{
				data[pen++] = normalData[faceData[i].normals[j] - 1].x;
				data[pen++] = normalData[faceData[i].normals[j] - 1].y;
				data[pen++] = normalData[faceData[i].normals[j] - 1].z;
			}

			if (useTangents)
			{
				data[pen++] = faceData[i].tangent[j].x;
				data[pen++] = faceData[i].tangent[j].y;
				data[pen++] = faceData[i].tangent[j].z;
				data[pen++] = faceData[i].tangent[j].w;
			}
		}
	}

	// Save file.
	FILE* modelFile = fopen(outputFile.c_str(), "wb");
	if (modelFile == nullptr)
	{
		Jwl::Error("Output file could not be created.");
		return false;
	}

	// Write header.
	fwrite(&minBounds, sizeof(Jwl::vec3), 1, modelFile);
	fwrite(&maxBounds, sizeof(Jwl::vec3), 1, modelFile);
	fwrite(&useUvs, sizeof(bool), 1, modelFile);
	fwrite(&useNormals, sizeof(bool), 1, modelFile);
	fwrite(&useTangents, sizeof(bool), 1, modelFile);
	fwrite(&numVertices, sizeof(int), 1, modelFile);

	// Write Data.
	fwrite(data, sizeof(float), bufferSize, modelFile);
	auto result = fclose(modelFile);

	// Report results.
	if (result != 0)
	{
		Jwl::Error("Failed to generate mesh Binary\nOutput file could not be saved.");
		return false;
	}
	else
	{
		if (packUvs && !hasUvs)
		{
			Jwl::Warning("Output of uvs was enabled but no uvs were found in the mesh.");
		}

		if (packNormals && !hasNormals)
		{
			Jwl::Warning("Output of normals was enabled but no normals were found in the mesh.");
		}

		if (packTangents && !hasUvs)
		{
			Jwl::Warning("Output of tangents was enabled but the mesh has no uvs. UVs are required to compute tangents.");
		}

		if (packTangents && !hasNormals)
		{
			Jwl::Warning("Output of tangents was enabled but the mesh has no normals. Normals are required to compute tangents.");
		}

		return true;
	}
}

bool MeshEncoder::Upgrade(Jwl::ConfigTable& metadata, unsigned loadedVersion) const
{
	switch (loadedVersion)
	{
	case 1:
		// Added tangents field.
		metadata.SetValue("tangents", true);
		break;
	}

	return true;
}
