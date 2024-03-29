// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Resource/Shader.h"
#include "gemcutter/Resource/VertexArray.h"

namespace gem
{
	struct vec3;
	struct vec4;

	// Provides simple, intimidate, geometry rendering.
	// Line, Triangle, Rectangle, and Grid functions are intended for debugging; they are not particularly efficient.
	extern class PrimitivesSingleton Primitives;
	class PrimitivesSingleton
	{
	public:
		bool Load();
		bool IsLoaded() const;
		void Unload();

		void DrawLine(const vec3& p1, const vec3& p2, const vec4& color);
		void DrawLine(const vec3& p1, const vec3& p2, const vec4& color1, const vec4& color2);
		void DrawLine(const vec3& p1, const vec3& p2, Texture& tex);

		void DrawTriangle(const vec3& p1, const vec3& p2, const vec3& p3, const vec4& color);
		void DrawTriangle(const vec3& p1, const vec3& p2, const vec3& p3, const vec4& color1, const vec4& color2, const vec4& color3);
		void DrawTriangle(const vec3& p1, const vec3& p2, const vec3& p3, Texture& tex);

		void DrawRectangle(const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4, const vec4& color);
		void DrawRectangle(const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4, const vec4& color1, const vec4& color2, const vec4& color3, const vec4& color4);
		void DrawRectangle(const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4, Texture& tex);

		void DrawGrid(const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4, const vec4& color, unsigned numDivisions);

		void DrawFullScreenQuad(Shader& program);
		void DrawFullScreenQuad(Texture& tex);

		// Returns a rectangle mesh spanning from 0 to 1 on the x and y axes.
		VertexArray::Ptr GetQuadArray() const;
		// Returns a rectangle mesh spanning from -1 to 1 on the x and y axes.
		VertexArray::Ptr GetUnitQuadArray() const;
		// Returns a cube mesh spanning from -1 to 1 on all axes. Can be used for skyboxes.
		VertexArray::Ptr GetUnitCubeArray() const;

	private:
		bool isLoaded = false;

		Shader lineProgram;
		Shader triangleProgram;
		Shader rectangleProgram;
		Shader texturedLineProgram;
		Shader texturedTriangleProgram;
		Shader texturedRectangleProgram;
		Shader texturedFullScreenQuadProgram;

		VertexArray::Ptr quadArray;
		VertexArray::Ptr unitQuadArray;
		VertexArray::Ptr unitCubeArray;

		// Allows the rendering of primitives without vertex attributes.
		unsigned dummyVAO = 0;
	};
}
