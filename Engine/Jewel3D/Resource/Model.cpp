// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Model.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Math/Vector.h"
#include "Jewel3D/Utilities/ScopeGuard.h"

#include <GLEW/GL/glew.h>
#include <fstream>

#define CHAR_BUFFER_SIZE 128

//- Indices for three points; one triangle. v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
struct MeshFace
{
	MeshFace()
	{
		vertices[0] = 0;
		vertices[1] = 0;
		vertices[2] = 0;

		textures[0] = 0;
		textures[1] = 0;
		textures[2] = 0;

		normals[0] = 0;
		normals[1] = 0;
		normals[2] = 0;
	}

	MeshFace(unsigned v1, unsigned v2, unsigned v3, 
		unsigned t1, unsigned t2, unsigned t3, 
		unsigned n1, unsigned n2, unsigned n3)
	{
		vertices[0] = v1;
		vertices[1] = v2;
		vertices[2] = v3;

		textures[0] = t1;
		textures[1] = t2;
		textures[2] = t3;

		normals[0] = n1;
		normals[1] = n2;
		normals[2] = n3;
	}

	unsigned vertices[3];
	unsigned textures[3];
	unsigned normals[3];
};

namespace Jwl
{
	Model::~Model()
	{
		Unload();
	}

	bool Model::Load(const std::string& InputFile)
	{
		// We support both .obj and our proprietary .model.
		if (InputFile.find(".model") != std::string::npos)
		{
			// Load binary file.
			FILE* binaryFile = nullptr;
			fopen_s(&binaryFile, InputFile.c_str(), "rb");

			if (binaryFile == nullptr)
			{
				Error("Model: ( %s )\nUnable to open file.", InputFile.c_str());
				return false;
			}

			fread(&numFaces, sizeof(int), 1, binaryFile);
			// Which attributes are enabled?
			fread(&hasUvs, sizeof(bool), 1, binaryFile);
			fread(&hasNormals, sizeof(bool), 1, binaryFile);

			numVertices = numFaces * 3;

			// Determine size of file.
			int bufferSize = numVertices * 3;
			if (hasUvs)
			{
				bufferSize += numVertices * 2;
			}
			if (hasNormals)
			{
				bufferSize += numVertices * 3;
			}

			// Read the data buffer.
			float* data = (float*)malloc(sizeof(float) * bufferSize);
			defer { free(data); };

			fread(data, sizeof(float), bufferSize, binaryFile);
			fclose(binaryFile);

			// Send data to OpenGL.
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bufferSize, data, GL_STATIC_DRAW);
		}
		else if (InputFile.find(".obj") != std::string::npos)
		{
			// load ASCII file
			std::ifstream input;
			input.open(InputFile);

			if (!input)
			{
				Error("Model: ( %s )\nUnable to open file.", InputFile.c_str());
				return false;
			}

			char inputString[CHAR_BUFFER_SIZE];

			// Unique data.
			std::vector<vec3> vertexData;
			std::vector<vec2> textureData;
			std::vector<vec3> normalData;
			// Indexing data.
			std::vector<MeshFace> faceData;
			// OpenGL ready data.
			std::vector<float> unPackedData;

			while (!input.eof())
			{
				input.getline(inputString, CHAR_BUFFER_SIZE);

				if (std::strstr(inputString, "#") != nullptr)
				{
					continue;
				}
				else if (std::strstr(inputString, "vt") != nullptr)
				{
					hasUvs = true;

					// Load texture coordinates.
					vec2 temp;
					std::sscanf(inputString, "vt %f %f", &temp.x, &temp.y);
					textureData.push_back(temp);
				}
				else if (std::strstr(inputString, "vn") != nullptr)
				{
					hasNormals = true;

					// Load normals.
					vec3 temp;
					std::sscanf(inputString, "vn %f %f %f", &temp.x, &temp.y, &temp.z);
					normalData.push_back(temp);
				}
				else if (std::strstr(inputString, "v") != nullptr)
				{
					// Load vertices.
					vec3 temp;
					std::sscanf(inputString, "v %f %f %f", &temp.x, &temp.y, &temp.z);
					vertexData.push_back(temp);
				}
				else if (std::strstr(inputString, "f") != nullptr)
				{
					// load face indices.
					MeshFace temp;

					// Each permutation of data formats.
					if (hasUvs && hasNormals)
					{
						std::sscanf(inputString, "f %u/%u/%u %u/%u/%u %u/%u/%u",
							&temp.vertices[0], &temp.textures[0], &temp.normals[0],
							&temp.vertices[1], &temp.textures[1], &temp.normals[1],
							&temp.vertices[2], &temp.textures[2], &temp.normals[2]);
					}
					else if (hasUvs && !hasNormals)
					{
						std::sscanf(inputString, "f %u/%u %u/%u %u/%u",
							&temp.vertices[0], &temp.textures[0],
							&temp.vertices[1], &temp.textures[1],
							&temp.vertices[2], &temp.textures[2]);
					}
					else if (!hasUvs && hasNormals)
					{
						std::sscanf(inputString, "f %u//%u %u//%u %u//%u",
							&temp.vertices[0], &temp.normals[0],
							&temp.vertices[1], &temp.normals[1],
							&temp.vertices[2], &temp.normals[2]);
					}
					else
					{
						// Only vertices.
						std::sscanf(inputString, "f %u %u %u",
							&temp.vertices[0], &temp.vertices[1], &temp.vertices[2]);
					}

					faceData.push_back(temp);
				}
			}

			input.close();

			// Unpack the data.
			for (unsigned i = 0; i < faceData.size(); i++)
			{
				for (unsigned j = 0; j < 3; j++)
				{
					unPackedData.push_back(vertexData[faceData[i].vertices[j] - 1].x);
					unPackedData.push_back(vertexData[faceData[i].vertices[j] - 1].y);
					unPackedData.push_back(vertexData[faceData[i].vertices[j] - 1].z);

					if (hasUvs)
					{
						unPackedData.push_back(textureData[faceData[i].textures[j] - 1].x);
						unPackedData.push_back(textureData[faceData[i].textures[j] - 1].y);
					}

					if (hasNormals)
					{
						unPackedData.push_back(normalData[faceData[i].normals[j] - 1].x);
						unPackedData.push_back(normalData[faceData[i].normals[j] - 1].y);
						unPackedData.push_back(normalData[faceData[i].normals[j] - 1].z);
					}
				}
			}

			numVertices = faceData.size() * 3;
			numFaces = faceData.size();

			// Send data to OpenGL.
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * unPackedData.size(), unPackedData.data(), GL_STATIC_DRAW);
		}
		else
		{
			Error("Model: ( %s )\nAttempted to load unknown file type as mesh.", InputFile.c_str());
			return false;
		}

		// Determine the stride.
		int stride = sizeof(float) * 3;
		if (hasUvs)
			stride += sizeof(float) * 2;
		if (hasNormals)
			stride += sizeof(float) * 3;

		// Vertex attribute  : 0
		// Texture attribute : 1
		// Normal attribute  : 2
		glEnableVertexAttribArray(0);
		if (hasUvs)
			glEnableVertexAttribArray(1);
		if (hasNormals)
			glEnableVertexAttribArray(2);

		glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));

		if (hasUvs)
			glVertexAttribPointer(1u, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(sizeof(float) * 3));
		if (hasNormals)
			glVertexAttribPointer(2u, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(sizeof(float) * (hasUvs ? 5 : 3)));

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

	unsigned Model::GetNumFaces() const
	{
		return numFaces;
	}

	unsigned Model::GetNumVerticies() const
	{
		return numVertices;
	}
}
