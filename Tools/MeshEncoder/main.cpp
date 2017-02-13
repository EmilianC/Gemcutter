#include "Jewel3D/Application/CmdArgs.h"
#include "Jewel3D/Math/Vector.h"
#include "Jewel3D/Utilities/ScopeGuard.h"

#include <Windows.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

constexpr unsigned CHAR_BUFFER_SIZE = 128;

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

enum class ConsoleColor
{
	DarkBlue = 1,
	DarkGreen,
	DarkTeal,
	DarkRed,
	DarkPink,
	DarkYellow,
	Gray,
	DarkGray,
	Blue,
	Green,
	Teal,
	Red,
	Pink,
	Yellow,
	White
};

static void SetColor(ConsoleColor color)
{
	static HANDLE StdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(StdOutputHandle, (WORD)color);
}

void OutputUsage()
{
	std::cout << "Generates Jewel3D Mesh Binaries.\n"
		<< "Usage:\n"
		<< "MeshEncoder.exe input output [options]\n"
		<< "Commands:\n"
		<< "input\t\t-i\tInput .obj file.\n"
		<< "output\t\t-o\tOutput .model file.\n"
		<< "Options:\n"
		<< "uvs\t\t-u\tEnables output of uvs. Default is off.\n"
		<< "normals\t\t-n\tEnables output of normals. Default is off.\n";
}

int main()
{
	if (Jwl::GetArgc() < 3)
	{
		OutputUsage();
		return EXIT_FAILURE;
	}

	std::string inputFile;
	std::string outputFile;
	bool packUvs = false;
	bool packNormals = false;

	if (!Jwl::GetCommandLineArgument("-i", inputFile) ||
		!Jwl::GetCommandLineArgument("-o", outputFile))
	{
		OutputUsage();
		return EXIT_FAILURE;
	}

	packUvs = Jwl::FindCommandLineArgument("-u") != -1;
	packNormals = Jwl::FindCommandLineArgument("-n") != -1;

	if (inputFile.find(".obj") == std::string::npos)
	{
		inputFile += ".obj";
	}

	if (outputFile.find(".model") == std::string::npos)
	{
		outputFile += ".model";
	}

	// load ASCII file.
	std::ifstream input;
	input.open(inputFile);
	if (!input)
	{
		std::cout << "Input file could not be opened or processed.\n";
		return EXIT_FAILURE;
	}

	char inputString[CHAR_BUFFER_SIZE];

	// Unique data.
	std::vector<Jwl::vec3> vertexData;
	std::vector<Jwl::vec2> textureData;
	std::vector<Jwl::vec3> normalData;
	// Indexing data.
	std::vector<MeshFace> faceData;
	// OpenGL ready data.
	std::vector<float> unPackedData;
	bool hasUvs = false;
	bool hasNormals = false;

	while (!input.eof())
	{
		input.getline(inputString, CHAR_BUFFER_SIZE);

		if (std::strstr(inputString, "#") != nullptr)
		{
			continue;
		}
		else if (std::strstr(inputString, "vt") == inputString)
		{
			hasUvs = true;

			// Load texture coordinates.
			Jwl::vec2 temp;
			std::sscanf(inputString, "vt %f %f", &temp.x, &temp.y);
			textureData.push_back(temp);
		}
		else if (std::strstr(inputString, "vn") == inputString)
		{
			hasNormals = true;

			// Load normals.
			Jwl::vec3 temp;
			std::sscanf(inputString, "vn %f %f %f", &temp.x, &temp.y, &temp.z);
			normalData.push_back(temp);
		}
		else if (std::strstr(inputString, "v") == inputString)
		{
			// Load vertices.
			Jwl::vec3 temp;
			std::sscanf(inputString, "v %f %f %f", &temp.x, &temp.y, &temp.z);
			vertexData.push_back(temp);
		}
		else if (std::strstr(inputString, "f") == inputString)
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

			if (packUvs && hasUvs)
			{
				unPackedData.push_back(textureData[faceData[i].textures[j] - 1].x);
				unPackedData.push_back(textureData[faceData[i].textures[j] - 1].y);
			}

			if (packUvs && hasNormals)
			{
				unPackedData.push_back(normalData[faceData[i].normals[j] - 1].x);
				unPackedData.push_back(normalData[faceData[i].normals[j] - 1].y);
				unPackedData.push_back(normalData[faceData[i].normals[j] - 1].z);
			}
		}
	}

	// Save file.
	FILE* modelFile = fopen(outputFile.c_str(), "wb");
	if (modelFile == nullptr)
	{
		SetColor(ConsoleColor::Red);
		std::cout << "Output file could not be created.\n";

		SetColor(ConsoleColor::Gray);
		return EXIT_FAILURE;
	}

	// Write header.
	unsigned numFaces = faceData.size();
	bool useUvs = packUvs && hasUvs;
	bool useNormals = packUvs && hasNormals;
	fwrite(&numFaces, sizeof(int), 1, modelFile);
	fwrite(&useUvs, sizeof(bool), 1, modelFile);
	fwrite(&useNormals, sizeof(bool), 1, modelFile);

	// Write Data.
	fwrite(unPackedData.data(), sizeof(float), unPackedData.size(), modelFile);
	auto result = fclose(modelFile);

	// Report results.
	if (result != 0)
	{
		SetColor(ConsoleColor::Red);
		std::cout << "-Failed to generate Jewel3D Mesh Binary-\nOutput file could not be saved.\n";

		SetColor(ConsoleColor::Gray);
		return EXIT_FAILURE;
	}
	else
	{
		if (packUvs && !hasUvs)
		{
			SetColor(ConsoleColor::Yellow);
			std::cout << "Output of uvs was enabled but no uvs were found in the mesh.\n";
		}
		
		if (packUvs && !hasNormals)
		{
			SetColor(ConsoleColor::Yellow);
			std::cout << "Output of normals was enabled but no normals were found in the mesh.\n";
		}

		SetColor(ConsoleColor::Green);
		std::cout << "-Jewel3D Mesh Binary created successfully-\n";

		SetColor(ConsoleColor::Gray);
		return EXIT_SUCCESS;
	}
}
