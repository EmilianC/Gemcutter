// Copyright (c) 2017 Emilian Cioca
#include "Primitives.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Math/Vector.h"
#include "gemcutter/Resource/Texture.h"

#include <GL/glew.h>

namespace
{
	constexpr std::string_view LINE_PROGRAM = R"(
		Uniforms{
			static Data : 0{
				vec4 uP1;
				vec4 uP2;
				vec4 uC1;
				vec4 uC2;
			}
		}
		Vertex{
			void main() {}
		}
		Geometry{
			layout(points) in;
			layout(line_strip, max_vertices = 2) out;
			out vec4 color;
			void main()
			{
				color = Data.uC1;
				gl_Position = Gem_ViewProj * Data.uP1;
				EmitVertex();
				color = Data.uC2;
				gl_Position = Gem_ViewProj * Data.uP2;
				EmitVertex();
				EndPrimitive();
			}
		}
		Fragment{
			in vec4 color;
			out vec4 outColor;
			void main()
			{
				outColor = color;
			}
		}
	)";

	constexpr std::string_view TEXTURED_LINE_PROGRAM = R"(
		Uniforms{
			static Data : 0{
				vec4 uP1;
				vec4 uP2;
			}
		}
		Vertex{
			void main() {}
		}
		Geometry{
			layout(points) in;
			layout(line_strip, max_vertices = 2) out;

			out vec2 texcoord;
			void main()
			{
				texcoord = vec2(0.0f, 0.5f);
				gl_Position = Gem_ViewProj * Data.uP1;
				EmitVertex();
				texcoord = vec2(1.0f, 0.5f);
				gl_Position = Gem_ViewProj * Data.uP2;
				EmitVertex();
				EndPrimitive();
			}
		}
		Samplers{
			sampler2D sTex : 0;
		}
		Fragment{
			in vec2 texcoord;
			out vec4 outColor;
			void main()
			{
				outColor = texture(sTex, texcoord).rgba;
			}
		}
	)";

	constexpr std::string_view TRIANGLE_PROGRAM = R"(
		Uniforms{
			static Data : 0{
				vec4 uP1;
				vec4 uP2;
				vec4 uP3;
				vec4 uC1;
				vec4 uC2;
				vec4 uC3;
			}
		}
		Vertex{
			void main() {}
		}
		Geometry{
			layout(points) in;
			layout(triangle_strip, max_vertices = 3) out;
			out vec4 color;
			void main()
			{
				color = Data.uC1;
				gl_Position = Gem_ViewProj * Data.uP1;
				EmitVertex();
				color = Data.uC2;
				gl_Position = Gem_ViewProj * Data.uP2;
				EmitVertex();
				color = Data.uC3;
				gl_Position = Gem_ViewProj * Data.uP3;
				EmitVertex();
				EndPrimitive();
			}
		}
		Fragment{
			in vec4 color;
			out vec4 outColor;
			void main()
			{
				outColor = color;
			}
		}
	)";

	constexpr std::string_view TEXTURED_TRIANGLE_PROGRAM = R"(
		Uniforms{
			static Data : 0{
				vec4 uP1;
				vec4 uP2;
				vec4 uP3;
			}
		}
		Vertex{
			void main() {}
		}
		Geometry{
			layout(points) in;
			layout(triangle_strip, max_vertices = 3) out;

			out vec2 texcoord;
			void main()
			{
				texcoord = vec2(0.0f, 0.0f);
				gl_Position = Gem_ViewProj * Data.uP1;
				EmitVertex();
				texcoord = vec2(1.0f, 0.0f);
				gl_Position = Gem_ViewProj * Data.uP2;
				EmitVertex();
				texcoord = vec2(0.5f, 1.0f);
				gl_Position = Gem_ViewProj * Data.uP3;
				EmitVertex();
				EndPrimitive();
			}
		}
		Samplers{
			sampler2D sTex : 0;
		}
		Fragment{
			in vec2 texcoord;
			out vec4 outColor;
			void main()
			{
				outColor = texture(sTex, texcoord).rgba;
			}
		}
	)";

	constexpr std::string_view RECTANGE_PROGRAM = R"(
		Uniforms{
			static Data : 0{
				vec4 uP1;
				vec4 uP2;
				vec4 uP3;
				vec4 uP4;
				vec4 uC1;
				vec4 uC2;
				vec4 uC3;
				vec4 uC4;
			}
		}
		Vertex{
			void main() {}
		}
		Geometry{
			layout(points) in;
			layout(triangle_strip, max_vertices = 4) out;

			out vec4 color;
			void main()
			{
				color = Data.uC1;
				gl_Position = Gem_ViewProj * Data.uP1;
				EmitVertex();
				color = Data.uC2;
				gl_Position = Gem_ViewProj * Data.uP2;
				EmitVertex();
				color = Data.uC3;
				gl_Position = Gem_ViewProj * Data.uP3;
				EmitVertex();
				color = Data.uC4;
				gl_Position = Gem_ViewProj * Data.uP4;
				EmitVertex();
				EndPrimitive();
			}
		}
		Fragment{
			in vec4 color;
			out vec4 outColor;
			void main()
			{
				outColor = color;
			}
		}
	)";

	constexpr std::string_view TEXTURED_RECTANGLE_PROGRAM = R"(
		Uniforms{
			static Data : 0{
				vec4 uP1;
				vec4 uP2;
				vec4 uP3;
				vec4 uP4;
			}
		}
		Vertex{
			void main() {}
		}
		Geometry{
			layout(points) in;
			layout(triangle_strip, max_vertices = 4) out;

			out vec2 texcoord;
			void main()
			{
				texcoord = vec2(0.0f, 0.0f);
				gl_Position = Gem_ViewProj * Data.uP1;
				EmitVertex();
				texcoord = vec2(1.0f, 0.0f);
				gl_Position = Gem_ViewProj * Data.uP2;
				EmitVertex();
				texcoord = vec2(0.0f, 1.0f);
				gl_Position = Gem_ViewProj * Data.uP3;
				EmitVertex();
				texcoord = vec2(1.0f, 0.0f);
				gl_Position = Gem_ViewProj * Data.uP4;
				EmitVertex();
				EndPrimitive();
			}
		}
		Samplers{
			sampler2D sTex : 0;
		}
		Fragment{
			in vec2 texcoord;
			out vec4 outColor;
			void main()
			{
				outColor = texture(sTex, texcoord).rgba;
			}
		}
	)";

	constexpr std::string_view TEXTURED_FULLSCREEN_QUAD_PROGRAM = R"(
		Attributes{
			vec4 a_vert : 0;
			vec2 a_uv : 1;
		}
		Vertex{
			out vec2 texcoord;
			void main()
			{
				texcoord = a_uv;
				gl_Position = a_vert;
			}
		}
		Samplers{
			sampler2D sTex : 0;
		}
		Fragment{
			in vec2 texcoord;
			out vec4 outColor;
			void main()
			{
				outColor = texture(sTex, texcoord).rgba;
			}
		}
	)";

	constexpr float unitQuad_Data[30] =
	{
		-1.0f, -1.0f, 0.0f, // vec3 Vertex
		0.0f, 0.0f,         // vec2 Texcoord

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

	constexpr float quad_Data[30] =
	{
		0.0f, 0.0f, 0.0f, // vec3 Vertex
		0.0f, 0.0f,       // vec2 Texcoord

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

	constexpr float unitCube_Data[108] =
	{
		// posX
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,

		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,

		// negX
		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,

		// posY
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,

		// negY
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,

		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,

		// posZ
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		// negZ
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f
	};
}

namespace gem
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

		{
			auto buffer = VertexBuffer::MakeNew(static_cast<unsigned>(sizeof(quad_Data)), quad_Data, BufferUsage::Static, VertexBufferType::Data);

			VertexStream streamPos {
				.buffer      = buffer,
				.bindingUnit = 0,
				.format      = VertexFormat::Vec3,
				.startOffset = 0,
				.stride      = sizeof(float) * 5
			};

			VertexStream streamUv {
				.buffer      = std::move(buffer),
				.bindingUnit = 1,
				.format      = VertexFormat::Vec2,
				.startOffset = sizeof(float) * 3,
				.stride      = sizeof(float) * 5
			};

			quadArray = VertexArray::MakeNew();
			quadArray->AddStream(std::move(streamPos));
			quadArray->AddStream(std::move(streamUv));
			quadArray->SetVertexCount(6);
		}

		{
			auto buffer = VertexBuffer::MakeNew(static_cast<unsigned>(sizeof(unitQuad_Data)), unitQuad_Data, BufferUsage::Static, VertexBufferType::Data);

			VertexStream streamPos {
				.buffer      = buffer,
				.bindingUnit = 0,
				.format      = VertexFormat::Vec3,
				.startOffset = 0,
				.stride      = sizeof(float) * 5
			};

			VertexStream streamUv {
				.buffer      = std::move(buffer),
				.bindingUnit = 1,
				.format      = VertexFormat::Vec2,
				.startOffset = sizeof(float) * 3,
				.stride      = sizeof(float) * 5
			};

			unitQuadArray = VertexArray::MakeNew();
			unitQuadArray->AddStream(std::move(streamPos));
			unitQuadArray->AddStream(std::move(streamUv));
			unitQuadArray->SetVertexCount(6);
		}

		{
			auto buffer = VertexBuffer::MakeNew(static_cast<unsigned>(sizeof(unitCube_Data)), unitCube_Data, BufferUsage::Static, VertexBufferType::Data);

			VertexStream streamPos {
				.buffer = std::move(buffer),
				.format = VertexFormat::Vec3
			};

			unitCubeArray = VertexArray::MakeNew();
			unitCubeArray->AddStream(std::move(streamPos));
			unitCubeArray->SetVertexCount(36);
		}

		glGenVertexArrays(1, &dummyVAO);
		glBindVertexArray(dummyVAO);
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
		lineProgram.Unload();
		triangleProgram.Unload();
		rectangleProgram.Unload();
		texturedLineProgram.Unload();
		texturedTriangleProgram.Unload();
		texturedRectangleProgram.Unload();
		texturedFullScreenQuadProgram.Unload();

		quadArray.reset();
		unitQuadArray.reset();
		unitCubeArray.reset();

		glDeleteVertexArrays(1, &dummyVAO);
		dummyVAO = GL_NONE;

		isLoaded = false;
	}

	void PrimitivesSingleton::DrawLine(const vec3& p1, const vec3& p2, const vec4& color)
	{
		DrawLine(p1, p2, color, color);
	}

	void PrimitivesSingleton::DrawLine(const vec3& p1, const vec3& p2, const vec4& color1, const vec4& color2)
	{
		ASSERT(IsLoaded(), "Primitives must be initialized to call this function.");

		lineProgram.buffers[0].SetUniform("uP1", vec4(p1, 1.0f));
		lineProgram.buffers[0].SetUniform("uP2", vec4(p2, 1.0f));
		lineProgram.buffers[0].SetUniform("uC1", color1);
		lineProgram.buffers[0].SetUniform("uC2", color2);
		lineProgram.Bind();

		glBindVertexArray(dummyVAO);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(GL_NONE);

		lineProgram.UnBind();
	}

	void PrimitivesSingleton::DrawLine(const vec3& p1, const vec3& p2, Texture& tex)
	{
		ASSERT(IsLoaded(), "Primitives must be initialized to call this function.");

		tex.Bind(0);

		lineProgram.buffers[0].SetUniform("uP1", vec4(p1, 1.0f));
		lineProgram.buffers[0].SetUniform("uP2", vec4(p2, 1.0f));
		lineProgram.Bind();

		glBindVertexArray(dummyVAO);
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

		triangleProgram.buffers[0].SetUniform("uP1", vec4(p1, 1.0f));
		triangleProgram.buffers[0].SetUniform("uP2", vec4(p2, 1.0f));
		triangleProgram.buffers[0].SetUniform("uP3", vec4(p3, 1.0f));
		triangleProgram.buffers[0].SetUniform("uC1", color1);
		triangleProgram.buffers[0].SetUniform("uC2", color2);
		triangleProgram.buffers[0].SetUniform("uC3", color3);
		triangleProgram.Bind();

		glBindVertexArray(dummyVAO);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(GL_NONE);

		triangleProgram.UnBind();
	}

	void PrimitivesSingleton::DrawTriangle(const vec3& p1, const vec3& p2, const vec3& p3, Texture& tex)
	{
		ASSERT(IsLoaded(), "Primitives must be initialized to call this function.");

		tex.Bind(0);

		texturedTriangleProgram.buffers[0].SetUniform("uP1", vec4(p1, 1.0f));
		texturedTriangleProgram.buffers[0].SetUniform("uP2", vec4(p2, 1.0f));
		texturedTriangleProgram.buffers[0].SetUniform("uP3", vec4(p3, 1.0f));
		texturedTriangleProgram.Bind();

		glBindVertexArray(dummyVAO);
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

		rectangleProgram.buffers[0].SetUniform("uP1", vec4(p1, 1.0f));
		rectangleProgram.buffers[0].SetUniform("uP2", vec4(p2, 1.0f));
		rectangleProgram.buffers[0].SetUniform("uP3", vec4(p3, 1.0f));
		rectangleProgram.buffers[0].SetUniform("uP4", vec4(p4, 1.0f));
		rectangleProgram.buffers[0].SetUniform("uC1", color1);
		rectangleProgram.buffers[0].SetUniform("uC2", color2);
		rectangleProgram.buffers[0].SetUniform("uC3", color3);
		rectangleProgram.buffers[0].SetUniform("uC4", color4);
		rectangleProgram.Bind();

		glBindVertexArray(dummyVAO);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(GL_NONE);

		rectangleProgram.UnBind();
	}

	void PrimitivesSingleton::DrawRectangle(const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4, Texture& tex)
	{
		ASSERT(IsLoaded(), "Primitives must be initialized to call this function.");

		tex.Bind(0);

		rectangleProgram.buffers[0].SetUniform("uP1", vec4(p1, 1.0f));
		rectangleProgram.buffers[0].SetUniform("uP2", vec4(p2, 1.0f));
		rectangleProgram.buffers[0].SetUniform("uP3", vec4(p3, 1.0f));
		rectangleProgram.buffers[0].SetUniform("uP4", vec4(p4, 1.0f));
		rectangleProgram.Bind();

		glBindVertexArray(dummyVAO);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(GL_NONE);

		rectangleProgram.UnBind();

		tex.UnBind(0);
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

		unitQuadArray->Bind();
		unitQuadArray->Draw();
		unitQuadArray->UnBind();

		program.UnBind();
	}

	void PrimitivesSingleton::DrawFullScreenQuad(Texture& tex)
	{
		ASSERT(IsLoaded(), "Primitives must be initialized to call this function.");

		tex.Bind(0);
		texturedFullScreenQuadProgram.Bind();

		unitQuadArray->Bind();
		unitQuadArray->Draw();
		unitQuadArray->UnBind();

		texturedFullScreenQuadProgram.UnBind();
		tex.UnBind(0);
	}

	VertexArray::Ptr PrimitivesSingleton::GetQuadArray() const
	{
		ASSERT(IsLoaded(), "Primitives must be initialized to call this function.");

		return quadArray;
	}

	VertexArray::Ptr PrimitivesSingleton::GetUnitQuadArray() const
	{
		ASSERT(IsLoaded(), "Primitives must be initialized to call this function.");

		return unitQuadArray;
	}

	VertexArray::Ptr PrimitivesSingleton::GetUnitCubeArray() const
	{
		ASSERT(IsLoaded(), "Primitives must be initialized to call this function.");

		return unitCubeArray;
	}
}
