// Copyright (c) 2017 Emilian Cioca
#include "Primitives.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Math/Vector.h"
#include "gemcutter/Resource/Texture.h"

#include <glew/glew.h>

namespace
{
	constexpr char LINE_PROGRAM[] =
		"Uniforms\n{\n"
		"	static Data : 0\n{\n"
		"		vec4 uP1;\n"
		"		vec4 uP2;\n"
		"		vec4 uC1;\n"
		"		vec4 uC2;\n"
		"	}\n"
		"}\n"
		"Vertex\n{\n"
		"	void main() { }\n"
		"}\n"
		"Geometry\n{\n"
		"	layout(points) in;\n"
		"	layout(line_strip, max_vertices = 2) out;\n"
		"	out vec4 color;\n"
		"	void main()\n"
		"	{\n"
		"		color = Data.uC1;\n"
		"		gl_Position = Jwl_ViewProj * Data.uP1;\n"
		"		EmitVertex();\n"
		"		color = Data.uC2;\n"
		"		gl_Position = Jwl_ViewProj * Data.uP2;\n"
		"		EmitVertex();\n"
		"		EndPrimitive();\n"
		"	}\n"
		"}\n"
		"Fragment\n{\n"
		"	in vec4 color;\n"
		"	out vec4 outColor;\n"
		"	void main()\n"
		"	{\n"
		"		outColor = color;\n"
		"	}\n"
		"}\n";

	constexpr char TEXTURED_LINE_PROGRAM[] =
		"Uniforms\n{\n"
		"	static Data : 0\n{\n"
		"		vec4 uP1;\n"
		"		vec4 uP2;\n"
		"	}\n"
		"}\n"
		"Vertex\n{\n"
		"	void main() { }\n"
		"}\n"
		"Geometry\n{\n"
		"	layout(points) in;\n"
		"	layout(line_strip, max_vertices = 2) out;\n"
		"	\n"
		"	out vec2 texcoord;\n"
		"	void main()\n"
		"	{\n"
		"		texcoord = vec2(0.0f, 0.5f);\n"
		"		gl_Position = Jwl_ViewProj * Data.uP1;\n"
		"		EmitVertex();\n"
		"		texcoord = vec2(1.0f, 0.5f);\n"
		"		gl_Position = Jwl_ViewProj * Data.uP2;\n"
		"		EmitVertex();\n"
		"		EndPrimitive();\n"
		"	}\n"
		"}\n"
		"Samplers\n{\n"
		"	sampler2D sTex : 0;"
		"}\n"
		"Fragment\n{\n"
		"	in vec2 texcoord;\n"
		"	out vec4 outColor;\n"
		"	void main()\n"
		"	{\n"
		"		outColor = texture(sTex, texcoord).rgba;\n"
		"	}\n"
		"}\n";

	constexpr char TRIANGLE_PROGRAM[] =
		"Uniforms\n{\n"
		"	static Data : 0\n{\n"
		"		vec4 uP1;\n"
		"		vec4 uP2;\n"
		"		vec4 uP3;\n"
		"		vec4 uC1;\n"
		"		vec4 uC2;\n"
		"		vec4 uC3;\n"
		"	}\n"
		"}\n"
		"Vertex\n{\n"
		"void main() { }\n"
		"}\n"
		"Geometry\n{\n"
		"	layout(points) in;\n"
		"	layout(triangle_strip, max_vertices = 3) out;\n"
		"	out vec4 color;\n"
		"	void main()\n"
		"	{\n"
		"		color = Data.uC1;\n"
		"		gl_Position = Jwl_ViewProj * Data.uP1;\n"
		"		EmitVertex();\n"
		"		color = Data.uC2;\n"
		"		gl_Position = Jwl_ViewProj * Data.uP2;\n"
		"		EmitVertex();\n"
		"		color = Data.uC3;\n"
		"		gl_Position = Jwl_ViewProj * Data.uP3;\n"
		"		EmitVertex();\n"
		"		EndPrimitive();\n"
		"	}\n"
		"}\n"
		"Fragment\n{\n"
		"	in vec4 color;\n"
		"	out vec4 outColor;\n"
		"	void main()\n"
		"	{\n"
		"		outColor = color;\n"
		"	}\n"
		"}\n";

	constexpr char TEXTURED_TRIANGLE_PROGRAM[] =
		"Uniforms\n{\n"
		"	static Data : 0\n{\n"
		"		vec4 uP1;\n"
		"		vec4 uP2;\n"
		"		vec4 uP3;\n"
		"	}\n"
		"}\n"
		"Vertex\n{\n"
		"	void main() { }\n"
		"}\n"
		"Geometry\n{\n"
		"	layout(points) in;\n"
		"	layout(triangle_strip, max_vertices = 3) out;\n"
		"	\n"
		"	out vec2 texcoord;\n"
		"	void main()\n"
		"	{\n"
		"		texcoord = vec2(0.0f, 0.0f);\n"
		"		gl_Position = Jwl_ViewProj * Data.uP1;\n"
		"		EmitVertex();\n"
		"		texcoord = vec2(1.0f, 0.0f);\n"
		"		gl_Position = Jwl_ViewProj * Data.uP2;\n"
		"		EmitVertex();\n"
		"		texcoord = vec2(0.5f, 1.0f);\n"
		"		gl_Position = Jwl_ViewProj * Data.uP3;\n"
		"		EmitVertex();\n"
		"		EndPrimitive();\n"
		"	}\n"
		"}\n"
		"Samplers\n{\n"
		"	sampler2D sTex : 0;\n"
		"}\n"
		"Fragment\n{\n"
		"	in vec2 texcoord;\n"
		"	out vec4 outColor;\n"
		"	void main()\n"
		"	{\n"
		"		outColor = texture(sTex, texcoord).rgba;\n"
		"	}\n"
		"}\n";

	constexpr char RECTANGE_PROGRAM[] =
		"Uniforms\n{\n"
		"	static Data : 0\n{\n"
		"		vec4 uP1;\n"
		"		vec4 uP2;\n"
		"		vec4 uP3;\n"
		"		vec4 uP4;\n"
		"		vec4 uC1;\n"
		"		vec4 uC2;\n"
		"		vec4 uC3;\n"
		"		vec4 uC4;\n"
		"	}\n"
		"}\n"
		"Vertex\n{\n"
		"	void main() { }\n"
		"}\n"
		"Geometry\n{\n"
		"	layout(points) in;\n"
		"	layout(triangle_strip, max_vertices = 4) out;\n"
		"	\n"
		"	out vec4 color;\n"
		"	void main()\n"
		"	{\n"
		"		color = Data.uC1;\n"
		"		gl_Position = Jwl_ViewProj * Data.uP1;\n"
		"		EmitVertex();\n"
		"		color = Data.uC2;\n"
		"		gl_Position = Jwl_ViewProj * Data.uP2;\n"
		"		EmitVertex();\n"
		"		color = Data.uC3;\n"
		"		gl_Position = Jwl_ViewProj * Data.uP3;\n"
		"		EmitVertex();\n"
		"		color = Data.uC4;\n"
		"		gl_Position = Jwl_ViewProj * Data.uP4;\n"
		"		EmitVertex();\n"
		"		EndPrimitive();\n"
		"	}\n"
		"}\n"
		"Fragment\n{\n"
		"	in vec4 color;\n"
		"	out vec4 outColor;\n"
		"	void main()\n"
		"	{\n"
		"		outColor = color;\n"
		"	}\n"
		"}\n";

	constexpr char TEXTURED_RECTANGLE_PROGRAM[] =
		"Uniforms\n{\n"
		"	static Data : 0\n{\n"
		"		vec4 uP1;\n"
		"		vec4 uP2;\n"
		"		vec4 uP3;\n"
		"		vec4 uP4;\n"
		"	}\n"
		"}\n"
		"Vertex\n{\n"
		"	void main() { }\n"
		"}\n"
		"Geometry\n{\n"
		"	layout(points) in;\n"
		"	layout(triangle_strip, max_vertices = 4) out;\n"
		"	\n"
		"	out vec2 texcoord;\n"
		"	void main()\n"
		"	{\n"
		"		texcoord = vec2(0.0f, 0.0f);\n"
		"		gl_Position = Jwl_ViewProj * Data.uP1;\n"
		"		EmitVertex();\n"
		"		texcoord = vec2(1.0f, 0.0f);\n"
		"		gl_Position = Jwl_ViewProj * Data.uP2;\n"
		"		EmitVertex();\n"
		"		texcoord = vec2(0.0f, 1.0f);\n"
		"		gl_Position = Jwl_ViewProj * Data.uP3;\n"
		"		EmitVertex();\n"
		"		texcoord = vec2(1.0f, 0.0f);\n"
		"		gl_Position = Jwl_ViewProj * Data.uP4;\n"
		"		EmitVertex();\n"
		"		EndPrimitive();\n"
		"	}\n"
		"}\n"
		"Samplers\n{\n"
		"	sampler2D sTex : 0;\n"
		"}\n"
		"Fragment\n{\n"
		"	in vec2 texcoord;\n"
		"	out vec4 outColor;\n"
		"	void main()\n"
		"	{\n"
		"		outColor = texture(sTex, texcoord).rgba;\n"
		"	}\n"
		"}\n";

	constexpr char TEXTURED_FULLSCREEN_QUAD_PROGRAM[] =
		"Attributes\n{\n"
		"	vec4 a_vert : 0;"
		"	vec2 a_uv : 1;\n"
		"}\n"
		"Vertex\n{\n"
		"	out vec2 texcoord;\n"
		"	void main()\n"
		"	{\n"
		"		texcoord = a_uv;\n"
		"		gl_Position = a_vert;\n"
		"	}\n"
		"}\n"
		"Samplers\n{\n"
		"	sampler2D sTex : 0;\n"
		"}\n"
		"Fragment\n{\n"
		"	in vec2 texcoord;\n"
		"	out vec4 outColor;\n"
		"	void main()\n"
		"	{\n"
		"		outColor = texture(sTex, texcoord).rgba;\n"
		"	}\n"
		"}\n";

	constexpr char SKYBOX_PROGRAM[] =
		"Attributes\n{\n"
		"	vec3 a_vert : 0;"
		"}\n"
		"Vertex\n{\n"
		"	out vec3 texcoord;\n"
		"	void main()\n"
		"	{\n"
		"		texcoord = a_vert;\n"
		"		//After perspective divide, Z will be very close to 1.0\n"
		"		gl_Position = (Jwl_ViewProj * vec4(a_vert, 0.0)).xyww;\n"
		"		gl_Position.w *= 1.0001;\n"
		"	}\n"
		"}\n"
		"Samplers\n{\n"
		"	samplerCube sTex : 0;\n"
		"}\n"
		"Fragment\n{\n"
		"	in vec3 texcoord;\n"
		"	out vec4 outColor;\n"
		"	void main()\n"
		"	{\n"
		"		outColor = texture(sTex, texcoord);\n"
		"	}\n"
		"}\n";
}

namespace Jwl
{
	PrimitivesSingleton Primitives;

	bool PrimitivesSingleton::Load()
	{
		if (isLoaded)
		{
			return true;
		}

		/* Load Shaders */
		if (!lineProgram.LoadFromSource(LINE_PROGRAM))
		{
			Error("Primitives: ( Line )\nFailed to initialize.");
			Unload();
			return false;
		}

		if (!texturedLineProgram.LoadFromSource(TEXTURED_LINE_PROGRAM))
		{
			Error("Primitives: ( Textured Line )\nFailed to initialize.");
			Unload();
			return false;
		}

		if (!triangleProgram.LoadFromSource(TRIANGLE_PROGRAM))
		{
			Error("Primitives: ( Triangle )\nFailed to initialize.");
			Unload();
			return false;
		}

		if (!texturedTriangleProgram.LoadFromSource(TEXTURED_TRIANGLE_PROGRAM))
		{
			Error("Primitives: ( Textured Triangle )\nFailed to initialize.");
			Unload();
			return false;
		}

		if (!rectangleProgram.LoadFromSource(RECTANGE_PROGRAM))
		{
			Error("Primitives: ( Rectangle )\nFailed to initialize.");
			Unload();
			return false;
		}

		if (!texturedRectangleProgram.LoadFromSource(TEXTURED_RECTANGLE_PROGRAM))
		{
			Error("Primitives: ( Textured Rectangle )\nFailed to initialize.");
			Unload();
			return false;
		}

		if (!texturedFullScreenQuadProgram.LoadFromSource(TEXTURED_FULLSCREEN_QUAD_PROGRAM))
		{
			Error("Primitives: ( Textured Fullscreen Quad )\nFailed to initialize.");
			Unload();
			return false;
		}

		if (!skyboxProgram.LoadFromSource(SKYBOX_PROGRAM))
		{
			Error("Primitives: ( Skybox )\nFailed to initialize.");
			Unload();
			return false;
		}

		// Create dummy VAO for attribute-less rendering of primitives.
		glGenVertexArrays(1, &primitivesVAO);
		glBindVertexArray(primitivesVAO);

		// Set up full screen quad VAO containing vertices and textureCoords.
		unsigned vertexSize = sizeof(float) * 6 * 3;
		unsigned texCoordSize = sizeof(float) * 6 * 2;
		float VBO_Data[30] =
		{
			/*
			vec3 Vertex
			vec2 Texcoord
			*/

			-1.0f, -1.0f, 0.0f,
			0.0f, 0.0f,

			1.0f, -1.0f, 0.0f,
			1.0f, 0.0f,

			-1.0f, 1.0f, 0.0f,
			0.0f, 1.0f,

			1.0f, 1.0f, 0.0f,
			1.0f, 1.0f,

			-1.0f, 1.0f, 0.0f,
			0.0f, 1.0f,

			1.0f, -1.0f, 0.0f,
			1.0f, 0.0f
		};

		glGenVertexArrays(1, &fullScreenVAO);
		glBindVertexArray(fullScreenVAO);

		glEnableVertexAttribArray(0); //Positions
		glEnableVertexAttribArray(1); //UV's

		glGenBuffers(1, &fullScreenVBO);
		glBindBuffer(GL_ARRAY_BUFFER, fullScreenVBO);

		glBufferData(GL_ARRAY_BUFFER, vertexSize + texCoordSize, VBO_Data, GL_STATIC_DRAW);

		glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, reinterpret_cast<void*>(0));
		glVertexAttribPointer(1u, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, reinterpret_cast<void*>(sizeof(float) * 3));

		// Load unit cube for skyboxes.
		unsigned skyboxSize = sizeof(float) * 6 * 3 * 6;
		float skyboxData[108] =
		{
			//posX
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, 1.0f,
			1.0f, 1.0f, -1.0f,

			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, 1.0f,
			//negX
			-1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,

			-1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,
			//posY
			-1.0f, 1.0f, -1.0f,
			1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,

			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, -1.0f,
			//negY
			-1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,

			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, 1.0f,
			//posZ
			1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,

			-1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,
			//negZ
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,

			1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, -1.0f
		};

		glGenVertexArrays(1, &skyboxVAO);
		glBindVertexArray(skyboxVAO);

		glEnableVertexAttribArray(0); //Position

		glGenBuffers(1, &skyboxVBO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);

		glBufferData(GL_ARRAY_BUFFER, skyboxSize, skyboxData, GL_STATIC_DRAW);
		glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));

		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
		glBindVertexArray(GL_NONE);

		// Prepare buffer for unit quad rendering.
		vertexSize = sizeof(float) * 6 * 3;
		texCoordSize = sizeof(float) * 6 * 2;
		float quad_Data[30] =
		{
			/*
			 vec3 Vertex
			 vec2 Texcoord
			 */

			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f,

			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f,

			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f,

			1.0f, 1.0f, 0.0f,
			1.0f, 1.0f,

			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f,

			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f
		};

		glGenVertexArrays(1, &quadVAO);
		glBindVertexArray(quadVAO);

		glEnableVertexAttribArray(0); //vertices
		glEnableVertexAttribArray(1); //texcoords

		glGenBuffers(1, &quadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, vertexSize + texCoordSize, quad_Data, GL_STATIC_DRAW);

		glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, reinterpret_cast<void*>(0));
		glVertexAttribPointer(1u, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, reinterpret_cast<void*>(sizeof(float) * 3));

		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
		glBindVertexArray(GL_NONE);

		isLoaded = true;
		return true;
	}

	bool PrimitivesSingleton::IsLoaded() const
	{
		return isLoaded;
	}

	void PrimitivesSingleton::Unload()
	{
		skyboxProgram.Unload();
		lineProgram.Unload();
		triangleProgram.Unload();
		rectangleProgram.Unload();
		texturedLineProgram.Unload();
		texturedTriangleProgram.Unload();
		texturedRectangleProgram.Unload();
		texturedFullScreenQuadProgram.Unload();

		auto unloadVBO = [](unsigned& vbo) {
			glDeleteBuffers(1, &vbo);
			vbo = GL_NONE;
		};

		auto unloadVAO = [](unsigned& vao) {
			glDeleteVertexArrays(1, &vao);
			vao = GL_NONE;
		};

		unloadVAO(fullScreenVAO);
		unloadVBO(fullScreenVBO);
		unloadVAO(quadVAO);
		unloadVBO(quadVBO);
		unloadVAO(skyboxVAO);
		unloadVBO(skyboxVBO);
		unloadVAO(primitivesVAO);

		isLoaded = false;
	}

	void PrimitivesSingleton::DrawLine(const vec3& p1, const vec3& p2, const vec4& color)
	{
		DrawLine(p1, p2, color, color);
	}

	void PrimitivesSingleton::DrawLine(const vec3& p1, const vec3& p2, const vec4& color1, const vec4& color2)
	{
		ASSERT(IsLoaded(), "Primitives must be initialized to call this function.");

		lineProgram.buffers[0]->SetUniform("uP1", vec4(p1, 1.0f));
		lineProgram.buffers[0]->SetUniform("uP2", vec4(p2, 1.0f));
		lineProgram.buffers[0]->SetUniform("uC1", color1);
		lineProgram.buffers[0]->SetUniform("uC2", color2);
		lineProgram.Bind();

		glBindVertexArray(primitivesVAO);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(GL_NONE);

		lineProgram.UnBind();
	}

	void PrimitivesSingleton::DrawLine(const vec3& p1, const vec3& p2, Texture& tex)
	{
		ASSERT(IsLoaded(), "Primitives must be initialized to call this function.");

		tex.Bind(0);

		lineProgram.buffers[0]->SetUniform("uP1", vec4(p1, 1.0f));
		lineProgram.buffers[0]->SetUniform("uP2", vec4(p2, 1.0f));
		lineProgram.Bind();

		glBindVertexArray(primitivesVAO);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(GL_NONE);

		lineProgram.UnBind();

		tex.UnBind(0);
	}

	void PrimitivesSingleton::DrawTriangle(const vec3& p1, const vec3& p2, const vec3& p3, const vec4& color)
	{
		DrawTriangle(p1, p2, p3, color, color, color);
	}

	void PrimitivesSingleton::DrawTriangle(const vec3& p1, const vec3& p2, const vec3& p3, const vec4& color1, const vec4& color2, const vec4& color3)
	{
		ASSERT(IsLoaded(), "Primitives must be initialized to call this function.");

		triangleProgram.buffers[0]->SetUniform("uP1", vec4(p1, 1.0f));
		triangleProgram.buffers[0]->SetUniform("uP2", vec4(p2, 1.0f));
		triangleProgram.buffers[0]->SetUniform("uP3", vec4(p3, 1.0f));
		triangleProgram.buffers[0]->SetUniform("uC1", color1);
		triangleProgram.buffers[0]->SetUniform("uC2", color2);
		triangleProgram.buffers[0]->SetUniform("uC3", color3);
		triangleProgram.Bind();

		glBindVertexArray(primitivesVAO);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(GL_NONE);

		triangleProgram.UnBind();
	}

	void PrimitivesSingleton::DrawTriangle(const vec3& p1, const vec3& p2, const vec3& p3, Texture& tex)
	{
		ASSERT(IsLoaded(), "Primitives must be initialized to call this function.");

		tex.Bind(0);

		texturedTriangleProgram.buffers[0]->SetUniform("uP1", vec4(p1, 1.0f));
		texturedTriangleProgram.buffers[0]->SetUniform("uP2", vec4(p2, 1.0f));
		texturedTriangleProgram.buffers[0]->SetUniform("uP3", vec4(p3, 1.0f));
		texturedTriangleProgram.Bind();

		glBindVertexArray(primitivesVAO);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(GL_NONE);

		texturedTriangleProgram.UnBind();

		tex.UnBind(0);
	}

	void PrimitivesSingleton::DrawRectangle(const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4, const vec4& color)
	{
		DrawRectangle(p1, p2, p3, p4, color, color, color, color);
	}

	void PrimitivesSingleton::DrawRectangle(const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4, const vec4& color1, const vec4& color2, const vec4& color3, const vec4& color4)
	{
		ASSERT(IsLoaded(), "Primitives must be initialized to call this function.");

		rectangleProgram.buffers[0]->SetUniform("uP1", vec4(p1, 1.0f));
		rectangleProgram.buffers[0]->SetUniform("uP2", vec4(p2, 1.0f));
		rectangleProgram.buffers[0]->SetUniform("uP3", vec4(p3, 1.0f));
		rectangleProgram.buffers[0]->SetUniform("uP4", vec4(p4, 1.0f));
		rectangleProgram.buffers[0]->SetUniform("uC1", color1);
		rectangleProgram.buffers[0]->SetUniform("uC2", color2);
		rectangleProgram.buffers[0]->SetUniform("uC3", color3);
		rectangleProgram.buffers[0]->SetUniform("uC4", color4);
		rectangleProgram.Bind();

		glBindVertexArray(primitivesVAO);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(GL_NONE);

		rectangleProgram.UnBind();
	}

	void PrimitivesSingleton::DrawRectangle(const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4, Texture& tex)
	{
		ASSERT(IsLoaded(), "Primitives must be initialized to call this function.");

		tex.Bind(0);

		rectangleProgram.buffers[0]->SetUniform("uP1", vec4(p1, 1.0f));
		rectangleProgram.buffers[0]->SetUniform("uP2", vec4(p2, 1.0f));
		rectangleProgram.buffers[0]->SetUniform("uP3", vec4(p3, 1.0f));
		rectangleProgram.buffers[0]->SetUniform("uP4", vec4(p4, 1.0f));
		rectangleProgram.Bind();

		glBindVertexArray(primitivesVAO);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(GL_NONE);

		rectangleProgram.UnBind();

		tex.UnBind(0);
	}

	void PrimitivesSingleton::DrawUnitRectangle()
	{
		ASSERT(IsLoaded(), "Primitives must be initialized to call this function.");

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(GL_NONE);
	}

	void PrimitivesSingleton::DrawGrid(const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4, const vec4& color, unsigned numDivisions)
	{
		ASSERT(IsLoaded(), "Primitives must be initialized to call this function.");

		// Draw perimeter.
		DrawLine(p1, p2, color);
		DrawLine(p2, p3, color);
		DrawLine(p3, p4, color);
		DrawLine(p4, p1, color);

		// Draw grid lines.
		vec3 step = (p2 - p1) / static_cast<float>(numDivisions + 1);
		for (unsigned i = 1; i <= numDivisions; ++i)
		{
			auto num = static_cast<float>(i);
			DrawLine(p4 + step * num, p1 + step * num, color);
		}

		step = (p3 - p2) / static_cast<float>(numDivisions + 1);
		for (unsigned i = 1; i <= numDivisions; ++i)
		{
			auto num = static_cast<float>(i);
			DrawLine(p1 + step * num, p2 + step * num, color);
		}
	}

	void PrimitivesSingleton::DrawFullScreenQuad(Shader& program)
	{
		ASSERT(IsLoaded(), "Primitives must be initialized to call this function.");

		program.Bind();

		glBindVertexArray(fullScreenVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(GL_NONE);

		program.UnBind();
	}

	void PrimitivesSingleton::DrawFullScreenQuad(Texture& tex)
	{
		ASSERT(IsLoaded(), "Primitives must be initialized to call this function.");

		tex.Bind(0);
		texturedFullScreenQuadProgram.Bind();

		glBindVertexArray(fullScreenVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(GL_NONE);

		texturedFullScreenQuadProgram.UnBind();
		tex.UnBind(0);
	}

	void PrimitivesSingleton::DrawSkyBox(Texture& tex)
	{
		DrawSkyBox(tex, skyboxProgram);
	}

	void PrimitivesSingleton::DrawSkyBox(Texture& tex, Shader& program) const
	{
		ASSERT(IsLoaded(), "Primitives must be initialized to call this function.");
		ASSERT(tex.IsCubeMap(), "'tex' must be a cubemap to be rendered as a skybox.");

		tex.Bind(0);
		program.Bind();

		glDepthMask(false);
		glBindVertexArray(skyboxVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6 * 2 * 3);
		glBindVertexArray(GL_NONE);
		glDepthMask(true);

		program.UnBind();
		tex.UnBind(0);
	}
}
