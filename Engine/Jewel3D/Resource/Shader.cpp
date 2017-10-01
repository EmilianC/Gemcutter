// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Shader.h"
#include "Jewel3D/Application/FileSystem.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Math/Vector.h"
#include "Jewel3D/Utilities/ScopeGuard.h"
#include "Jewel3D/Utilities/String.h"

#include <GLEW/GL/glew.h>
#include <algorithm>
#include <cctype>
#include <functional>
#include <iostream>

namespace
{
	// Built in shaders.
	constexpr char passThroughVertex[] =
		"layout(location = 0) in vec4 a_vert;\n"
		"layout(location = 1) in vec2 a_uv;\n"
		"out vec2 texcoord;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = Jwl_MVP * a_vert;\n"
		"	texcoord = a_uv;\n"
		"}\n";

	constexpr char passThroughFragment[] =
		"uniform sampler2D sTex;\n"
		"in vec2 texcoord;\n"
		"out vec4 outColor;\n"
		"void main()\n"
		"{\n"
		"	outColor = texture(sTex, texcoord);\n"
		"}\n";

	constexpr char passThroughProgram[] =
		"Attributes\n{\n"
		"	vec4 a_vert : 0;\n"
		"	vec2 a_uv : 1;\n"
		"}\n"
		"Vertex\n{\n"
		"	out vec2 texcoord;\n"
		"	void main()\n"
		"	{\n"
		"		gl_Position = Jwl_MVP * a_vert;\n"
		"		texcoord = a_uv;\n"
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
		"		outColor = texture(sTex, texcoord);\n"
		"	}\n"
		"}\n";

	constexpr char header[] =
		"\n"
		"#define M_PI 3.14159265358979323846\n"
		"#define M_E 2.71828182845904523536\n"
		"#define M_LOG2E 1.44269504088896340736\n"
		"#define M_LOG10E 0.434294481903251827651\n"
		"#define M_LN2 0.693147180559945309417\n"
		"#define M_LN10 2.30258509299404568402\n"
		"\n"
		"layout(std140) uniform Jwl_Camera_Uniforms\n"
		"{\n"
		"	mat4 Jwl_View;\n"
		"	mat4 Jwl_Proj;\n"
		"	mat4 Jwl_ViewProj;\n"
		"	mat4 Jwl_InvView;\n"
		"	mat4 Jwl_InvProj;\n"
		"	vec3 Jwl_CameraPosition;\n"
		"};"
		"\n"
		"layout(std140) uniform Jwl_Model_Uniforms\n"
		"{\n"
		"	mat4 Jwl_MVP;\n"
		"	mat4 Jwl_ModelView;\n"
		"	mat4 Jwl_Model;\n"
		"	mat4 Jwl_InvModel;\n"
		"	mat4 Jwl_NormalToWorld;\n"
		"};"
		"\n"
		"layout(std140) uniform Jwl_Engine_Uniforms\n"
		"{\n"
		"	vec4 ScreenParams;\n"
		"};"
		"\n"
		"layout(std140) uniform Jwl_Time_Uniforms\n"
		"{\n"
		"	vec4 Jwl_Time;\n"
		"	vec4 Jwl_Sin;\n"
		"	vec4 Jwl_Cos;\n"
		"	float Jwl_DeltaTime;\n"
		"};\n"
		"\n" // Normal mapping helper.
		"mat3 make_TBN(vec3 normal, vec3 tangent, float handedness)\n"
		"{\n"
		"	vec3 N = mat3(Jwl_NormalToWorld) * normal;\n"
		"	vec3 T = mat3(Jwl_NormalToWorld) * tangent;\n"
		"	vec3 B = cross(T, N) * handedness;\n"
		"	return mat3(T, B, N);\n"
		"}\n"
		"\n" // sRGB <-> linear conversions.
		"float linear_to_sRGB(float x)\n"
		"{\n"
		"	if (x <= 0.00031308) return 12.92 * x;\n"
		"	else return 1.055 * pow(x, (1.0 / 2.4)) - 0.055;\n"
		"}\n"
		"vec3 linear_to_sRGB(vec3 v) { return vec3(linear_to_sRGB(v.x), linear_to_sRGB(v.y), linear_to_sRGB(v.z)); }\n"
		"\n"
		"float sRGB_to_linear(float x)\n"
		"{\n"
		"	if (x <= 0.040449936) return x / 12.92;\n"
		"	else return pow((x + 0.055) / 1.055, 2.4);\n"
		"}\n"
		"vec3 sRGB_to_linear(vec3 v) { return vec3(sRGB_to_linear(v.x), sRGB_to_linear(v.y), sRGB_to_linear(v.z)); }\n"
		"\n" // These are enum values from Jwl::Light::Type
		"bool JWL_IS_POINT_LIGHT(uint type) { return type == 0u; }\n"
		"bool JWL_IS_DIRECTIONAL_LIGHT(uint type) { return type == 1u; }\n"
		"bool JWL_IS_SPOT_LIGHT(uint type) { return type == 2u; }\n"
		"\n"
		"vec3 JWL_COMPUTE_LIGHT(vec3 normal, vec3 surfacePos, vec3 color, vec3 lightPos, vec3 direction, float attenLinear, float attenQuadratic, float angle, uint type)\n"
		"{\n"
		"	if (JWL_IS_POINT_LIGHT(type))\n"
		"	{\n"
		"		vec3 lightDir = lightPos - surfacePos;\n"
		"		float dist = length(lightDir);\n"
		"		lightDir /= dist;\n"
		"		float NdotL = dot(normal, lightDir);\n"
		""
		"		if (NdotL > 0.0)\n"
		"		{\n"
		"			float attenuation = 1.0 / (0.75 + attenLinear * dist + attenQuadratic * dist * dist);\n"
		"			return color * NdotL * attenuation;\n"
		"		}\n"
		"	}\n"
		"	else if (JWL_IS_DIRECTIONAL_LIGHT(type))\n"
		"	{\n"
		"		float NdotL = dot(normal, -direction);\n"
		""
		"		return color * max(NdotL, 0.0);\n"
		"	}\n"
		"	else\n" // Spot light is assumed if the other cases fail.
		"	{\n"
		"		vec3 lightDir = lightPos - surfacePos;\n"
		"		float dist = length(lightDir);\n"
		"		lightDir /= dist;\n"
		"		float NdotL = dot(normal, lightDir);\n"
		""
		"		if (NdotL > 0.0)\n"
		"		{\n"
		"			float attenuation = 1.0 / (0.75 + attenLinear * dist + attenQuadratic * dist * dist);\n"
		"			float coneCos = dot(lightDir, -direction);\n"
		"			if (coneCos > angle)\n"
		"			{\n"
		"				float coneFactor = (coneCos - angle) / (1.0 - angle);\n"
		"				return color * NdotL * attenuation * min(coneFactor, 1.0);\n"
		"			}\n"
		"		}\n"
		"	}\n"
		""
		"	return vec3(0.0);\n"
		"}"
		"\n\n"
		"#define is_point_light(light) JWL_IS_POINT_LIGHT(light##.Type)\n"
		"#define is_directional_light(light) JWL_IS_DIRECTIONAL_LIGHT(light##.Type)\n"
		"#define is_spot_light(light) JWL_IS_SPOT_LIGHT(light##.Type)\n"
		"#define compute_light(light, normal, pos) JWL_COMPUTE_LIGHT(normal, pos, light##.Color, light##.Position, light##.Direction, light##.AttenuationLinear, light##.AttenuationQuadratic, light##.Angle, light##.Type)\n";
}

namespace Jwl
{
	static bool CompileShader(unsigned& shader)
	{
		GLint success;
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		// Output GL error to log on failure.
		if (success == GL_FALSE)
		{
			char* infoLog = nullptr;

			GLint infoLen;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			ASSERT(infoLen > 0, "Could not retrieve shader compilation log");

			infoLog = static_cast<char*>(malloc(sizeof(char) * infoLen));
			defer { free(infoLog); };

			glGetShaderInfoLog(shader, sizeof(char) * infoLen, nullptr, infoLog);

			// Avoid duplicate newline characters.
			if (infoLog[infoLen - 2] == '\n')
			{
				infoLog[infoLen - 2] = '\0';
			}

			Error(infoLog);

			glDeleteShader(shader);
			shader = GL_NONE;

			return false;
		}

		return true;
	}

	static bool LinkProgram(unsigned program)
	{
		GLint success;
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &success);

		// Output GL error to log on failure.
		if (success == GL_FALSE)
		{
			char* infoLog = nullptr;

			GLint infoLen;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
			ASSERT(infoLen > 0, "Could not retrieve program compilation log");

			infoLog = static_cast<char*>(malloc(sizeof(char) * infoLen));
			defer { free(infoLog); };

			glGetProgramInfoLog(program, sizeof(char) * infoLen, nullptr, infoLog);

			// Avoid duplicate newline characters.
			if (infoLog[infoLen - 2] == '\n')
			{
				infoLog[infoLen - 2] = '\0';
			}

			Error(infoLog);

			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------------------------------------

	void ShaderVariantControl::Define(const std::string& name, const std::string& value)
	{
		ASSERT(!name.empty(), "Name cannot be empty.");

		defer{ UpdateHash(); };

		// Search for and update the define if it already exists.
		for (auto& define : defines)
		{
			if (define.name == name)
			{
				define.value = value;
				return;
			}
		}

		// Add as a new define.
		defines.push_back({ name, value });
	}

	void ShaderVariantControl::Switch(const std::string& name, bool state)
	{
		if (state)
		{
			Define(name);
		}
		else
		{
			Undefine(name);
		}
	}

	void ShaderVariantControl::Toggle(const std::string& name)
	{
		ASSERT(!name.empty(), "Name cannot be empty.");

		defer { UpdateHash(); };

		for (unsigned i = 0; i < defines.size(); ++i)
		{
			if (defines[i].name == name)
			{
				defines.erase(defines.begin() + i);
				return;
			}
		}

		defines.push_back({ name, "" });
	}

	bool ShaderVariantControl::IsDefined(const std::string& name) const
	{
		for (auto& define : defines)
		{
			if (define.name == name)
			{
				return true;
			}
		}

		return false;
	}

	bool ShaderVariantControl::IsEmpty() const
	{
		return defines.empty();
	}

	void ShaderVariantControl::Undefine(const std::string& name)
	{
		for (unsigned i = 0; i < defines.size(); ++i)
		{
			if (defines[i].name == name)
			{
				defines.erase(defines.begin() + i);
				UpdateHash();

				return;
			}
		}
	}

	void ShaderVariantControl::Reset()
	{
		defines.clear();
		hash = 0;
	}

	std::string ShaderVariantControl::GetString() const
	{
		std::string result;

		// Rough estimate of total length.
		result.reserve(defines.size() * 12);
		
		for (auto& define : defines)
		{
			result += "#define " + define.name + ' ' + define.value + '\n';
		}

		return result;
	}

	unsigned ShaderVariantControl::GetHash() const
	{
		return hash;
	}

	bool ShaderVariantControl::operator==(const ShaderVariantControl& other) const
	{
		if (defines.size() != other.defines.size())
			return false;

		for (unsigned i = 0; i < defines.size(); ++i)
		{
			if (defines[i].name != other.defines[i].name ||
				defines[i].value != other.defines[i].value)
				return false;
		}

		return true;
	}

	bool ShaderVariantControl::operator!=(const ShaderVariantControl& other) const
	{
		return !operator==(other);
	}

	void ShaderVariantControl::UpdateHash()
	{
		// Sort alphabetically to eliminate order of defines creating different hashes.
		std::sort(defines.begin(), defines.end(), [](const auto& lhs, const auto& rhs)
		{
			return lhs.name < rhs.name;
		});

		std::hash<std::string> str_hash;
		hash = 0;

		for (auto& define : defines)
		{
			hash = hash ^ str_hash(define.name) ^ str_hash(define.value);
		}
	}

	//-----------------------------------------------------------------------------------------------------

	std::string Shader::commonHeader;

	Shader::~Shader()
	{
		Unload();
	}

	bool Shader::Load(std::string filePath)
	{
		ASSERT(!IsLoaded(), "ShaderData already has a Shader loaded.");

		auto ext = ExtractFileExtension(filePath);
		if (ext.empty())
		{
			filePath += ".shader";
		}
		else if (!CompareLowercase(ext, ".shader"))
		{
			Error("Shader: ( %s )\nAttempted to load unknown file type as a shader.", filePath.c_str());
			return false;
		}

		std::string source = LoadFileAsString(filePath);
		if (source.empty())
		{
			Error("Shader: ( %s )\nUnable to open file.", filePath.c_str());
			return false;
		}
		
		if (!LoadInternal(source))
		{
			Error("Shader: ( %s )", filePath.c_str());
			return false;
		}

		return true;
	}

	bool Shader::LoadFromSource(const std::string& source)
	{
		ASSERT(!IsLoaded(), "ShaderData already has a Shader loaded.");

		if (!LoadInternal(source))
		{
			Error("Shader: ( From Source )");
			return false;
		}

		return true;
	}

	bool Shader::LoadPassThrough()
	{
		ASSERT(!IsLoaded(), "ShaderData already has a Shader loaded.");

		if (!LoadInternal(passThroughProgram))
		{
			Error("Shader: ( PassThrough )");
			return false;
		}

		return true;
	}

	Shader::Ptr Shader::MakeNewPassThrough()
	{
		auto shader = MakeNew();
		shader->LoadPassThrough();

		return shader;
	}

	bool Shader::LoadInternal(std::string source)
	{
		ASSERT(!IsLoaded(), "A Shader is already loaded. Call ShaderData::Unload() first.");

		// Build version identifier from the current openGL version.
		if (commonHeader.empty())
		{
			int major = 0;
			int minor = 0;
			glGetIntegerv(GL_MAJOR_VERSION, &major);
			glGetIntegerv(GL_MINOR_VERSION, &minor);

			// Our minimum supported version is 3.3, where the format of the GLSL
			// version identifier begins to be symmetrical with the GL version.
			commonHeader = "#version " + std::to_string(major) + std::to_string(minor) + "0\n" + header;
		}

		// Clean up the source to make parsing easier.
		RemoveComments(source);
		RemoveRedundantWhitespace(source);

		size_t pos = 0;
		while (pos < source.size())
		{
			if (!std::isspace(source[pos]))
			{
				Block block(source);

				// We are not currently in a block, so we expect a block identifier.
				if (source.compare(pos, 10, "Attributes") == 0)
					block.type = BlockType::Attributes;
				else if (source.compare(pos, 8, "Uniforms") == 0)
					block.type = BlockType::Uniforms;
				else if (source.compare(pos, 8, "Samplers") == 0)
					block.type = BlockType::Samplers;
				else if (source.compare(pos, 6, "Vertex") == 0)
					block.type = BlockType::Vertex;
				else if (source.compare(pos, 8, "Geometry") == 0)
					block.type = BlockType::Geometry;
				else if (source.compare(pos, 8, "Fragment") == 0)
					block.type = BlockType::Fragment;
				else
				{
					Error("Unrecognized block identifier.");
					Unload();
					return false;
				}

				// Find the block start.
				pos = source.find('{', pos);

				if (pos == std::string::npos)
				{
					Error("Expected '{' to start block after identifier.");
					Unload();
					return false;
				}

				// Save the starting position (starting after the '{').
				pos++;
				block.start = pos;

				// Find the end of the block.
				int indentStack = 1;
				while (pos < source.size())
				{
					if (source[pos] == '{')
					{
						indentStack++;
					}
					else if (source[pos] == '}')
					{
						indentStack--;
					}
					
					if (indentStack == 0)
					{
						// We found the closing brace.
						block.end = pos - 1;
						break;
					}

					pos++;
				}

				// Did we find the matching brace?
				if (indentStack != 0)
				{
					Error("Unexpected end of file. '{' without matching '}'.");
					Unload();
					return false;
				}

				// Process the block according to its type.
				switch (block.type)
				{
				case BlockType::Attributes:
					if (block.source.find("#", block.start, block.end - block.start) != std::string::npos)
					{
						Error("Preprocessor use is not allowed inside of Attribute blocks.");
						Unload();
						return false;
					}

					if (!ParseAttributes(block))
					{
						Unload();
						return false;
					}
					break;
				case BlockType::Uniforms:
					if (block.source.find("#", block.start, block.end - block.start) != std::string::npos)
					{
						Error("Preprocessor use is not allowed inside of Uniform blocks.");
						Unload();
						return false;
					}

					if (!ParseUniforms(block))
					{
						Unload();
						return false;
					}
					break;
				case BlockType::Samplers:
					if (block.source.find("#", block.start, block.end - block.start) != std::string::npos)
					{
						Error("Preprocessor use is not allowed inside of Sampler blocks.");
						Unload();
						return false;
					}

					if (!ParseSamplers(block))
					{
						Unload();
						return false;
					}
					break;
				case BlockType::Vertex:
				case BlockType::Geometry:
				case BlockType::Fragment:
					if (!ParseShader(block))
					{
						Unload();
						return false;
					}
					break;
				}
			}

			// Continue through the file.
			pos++;
		}

		// We must have at least one shader.
		if (vertexSource.empty() && geometrySource.empty() && fragmentSrouce.empty())
		{
			Error("Must specify at least one shader.");
			return false;
		}

		// If a shader block was left empty, we substitute a pass-through.
		if (vertexSource.empty())
		{
			vertexSource = passThroughVertex;
		}
		else if (fragmentSrouce.empty())
		{
			fragmentSrouce = passThroughFragment;
		}

		loaded = true;
		return true;
	}

	bool Shader::ParseAttributes(const Block &block)
	{
		ASSERT(block.type == BlockType::Attributes, "Expected an Attribute block type.");

		size_t pos = block.start;

		char type[16] = { '\0' };
		char name[128] = { '\0' };
		unsigned Id = 0;

		while (pos < block.end)
		{
			if (!std::isspace(block.source[pos]))
			{
				if (sscanf(&block.source[pos], "%15s %127s : %u;", type, name, &Id) != 3)
				{
					Error("Failed to parse Attribute.");
					return false;
				}

				attributes += "layout(location = " + std::to_string(Id) + ") in " + std::string(type) + ' ' + std::string(name) + ";\n";

				// Jump past the line we just read.
				pos = block.source.find(';', pos);
			}

			pos++;
		}

		return true;
	}

	bool Shader::ParseShader(const Block& block)
	{
		std::string *output = nullptr;
		switch (block.type)
		{
		case BlockType::Vertex:
			output = &vertexSource;
			break;
		case BlockType::Geometry:
			output = &geometrySource;
			break;
		case BlockType::Fragment:
			output = &fragmentSrouce;
			break;
		default:
			ASSERT(false, "Expected a Shader block type.");
		}

		// The first keyword in the block might be an external reference or the indicating a passthrough shader.
		size_t pos = block.start;
		char path[256] = { '\0' };
		while (pos < block.end)
		{
			if (!std::isspace(block.source[pos]))
			{
				if (sscanf(&block.source[pos], "#include \"%255s\"", path) == 1)
				{
					// Remove invalid characters.
					while (char* chr = strchr(path, '\"'))
					{
						*chr = '\0';
					}

					if (IsPathRelative(path))
					{
						*output = LoadFileAsString(RootAssetDirectory + path);
					}
					else
					{
						*output = LoadFileAsString(path);
					}

					if (output->empty())
					{
						Error("Shader include (%s) failed to load.", path);
						return false;
					}
				}
				else
				{
					// Regular shader, take the whole block.
					*output = block.source.substr(block.start, block.end - block.start);
				}

				break;
			}

			pos++;
		}

		return true;
	}

	bool Shader::ParseUniforms(const Block& block)
	{
		ASSERT(block.type == BlockType::Uniforms, "Expected a Uniform block type.");

		size_t pos = block.start;

		char modifer1[128] = { '\0' };
		char modifer2[128] = { '\0' };
		char name[128] = { '\0' };
		unsigned Id = 0;

		while (pos < block.end)
		{
			bool isStatic = false;
			bool isTemplate = false;

			if (!std::isspace(block.source[pos]))
			{
				if (sscanf(&block.source[pos], "%127s %127s %127s : %u;", modifer1, modifer2, name, &Id) == 4)
				{
					// There are only two possible modifier keywords and both are being used.
					if ((strcmp(modifer1, "template") == 0 && strcmp(modifer2, "static") == 0) ||
						(strcmp(modifer1, "static") == 0 && strcmp(modifer2, "template") == 0))
					{
						isStatic = true;
						isTemplate = true;
					}
					else
					{
						Error("Unrecognized modifier keyword on Uniform buffer.");
						return false;
					}
				}
				else if (sscanf(&block.source[pos], "%127s %127s : %u;", modifer1, name, &Id) == 3)
				{
					if (strcmp(modifer1, "template") == 0)
					{
						isTemplate = true;
					}
					else if (strcmp(modifer1, "static") == 0)
					{
						isStatic = true;
					}
					else
					{
						Error("Unrecognized modifier keyword on Uniform buffer.");
						return false;
					}
				}
				else 
				{
					if (sscanf(&block.source[pos], "%127s : %u;", name, &Id) != 2)
					{
						Error("Failed to parse Uniform buffer.");
						return false;
					}
				}

				if (Id == static_cast<unsigned>(UniformBufferSlot::Camera) ||
					Id == static_cast<unsigned>(UniformBufferSlot::Engine) ||
					Id == static_cast<unsigned>(UniformBufferSlot::Model) ||
					Id == static_cast<unsigned>(UniformBufferSlot::Time))
				{
					Error("Buffer \"%s\" uses a reserved slot binding.", name);
					return false;
				}

				// Find the block start.
				pos = block.source.find('{', pos);

				if (pos == std::string::npos)
				{
					Error("Expected '{' to start block after uniform buffer.");
					return false;
				}

				Block buffer(block.source);
				buffer.type = BlockType::Uniforms;

				// Save the starting position (starting after the '{').
				pos++;
				buffer.start = pos;

				// Find the end of the block.
				while (pos < block.source.size())
				{
					if (block.source[pos] == '}')
					{
						buffer.end = pos - 1;
						break;
					}

					pos++;
				}

				// Did we find the matching brace?
				if (buffer.end == 0)
				{
					Error("Unexpected end of file. '{' without matching '}'.");
					return false;
				}

				if (!ParseUniformBlock(buffer, name, Id, isTemplate, isStatic))
				{
					return false;
				}
			}

			pos++;
		}

		return true;
	}

	bool Shader::ParseUniformBlock(const Block& block, const char* name, unsigned Id, bool makeTemplate, bool makeStatic)
	{
		ASSERT(block.type == BlockType::Uniforms, "Expected a Uniform block type.");
		ASSERT(name != nullptr, "Must provide name.");

		// The struct with all default assignments intact.
		std::string rawStruct = block.source.substr(block.start, block.end - block.start);

		/* Convert to OpenGL compatible code */
		std::string uniformStruct = rawStruct;
		size_t assignmentPos = uniformStruct.find('=');
		while (assignmentPos != std::string::npos)
		{
			// We have to remove the assignment operation because it's our own
			// syntax and OpenGL will not recognize it. We will erase everything between the
			// end of the name and the semicolon.
			const size_t semicolon = uniformStruct.find(';', assignmentPos);
			if (semicolon == std::string::npos)
			{
				Error("Block member assignment missing trailing semicolon.");
				return false;
			}

			uniformStruct.erase(assignmentPos, semicolon - assignmentPos);

			assignmentPos = uniformStruct.find('=', assignmentPos);
		}

		uniformStruct = FormatString("layout(std140) uniform Jwl_User_%s\n{%s} %s;\n", name, uniformStruct.c_str(), name);

		/* Create template and the shader owned buffer */
		if (makeTemplate || makeStatic)
		{
			auto buffer = UniformBuffer::MakeNew();

			struct UniformMember
			{
				std::string name;
				std::string value;
				GLenum type;
			};

			std::vector<UniformMember> uniforms;

			// Find all uniforms and parse their information.
			for (unsigned i = 0; i < rawStruct.size(); ++i)
			{
				if (std::isspace(rawStruct[i]))
					continue;

				uniforms.push_back(UniformMember());
				UniformMember& uniform = uniforms.back();

				const size_t semicolon = rawStruct.find(';', i);
				if (semicolon == std::string::npos)
				{
					Error("Uniform member is missing a trailing semicolon.");
					return false;
				}

				const std::string line = rawStruct.substr(i, semicolon - i);

				/* Resolve Name */
				// If we don't find a '=', the result will be no-position, thus causing the search to simply start at the end of the string.
				const size_t assignment = line.find('=');
				const size_t nameEnd = line.find_last_not_of(" \t\n=", assignment);
				if (nameEnd == std::string::npos)
				{
					Error("Failed to parse a uniform member's name.");
					return false;
				}

				// Continue to search until we hit more whitespace to isolate the name.
				const size_t nameStart = line.find_last_of(" \t\n", nameEnd);
				if (nameStart == std::string::npos)
				{
					Error("Failed to parse a uniform member's name.");
					return false;
				}

				uniform.name = line.substr(nameStart + 1, nameEnd - nameStart);
					
				if (StartsWith(line, "float"))			uniform.type = GL_FLOAT;
				else if (StartsWith(line, "vec2"))		uniform.type = GL_FLOAT_VEC2;
				else if (StartsWith(line, "vec3"))		uniform.type = GL_FLOAT_VEC3;
				else if (StartsWith(line, "vec4"))		uniform.type = GL_FLOAT_VEC4;
				else if (StartsWith(line, "mat4"))		uniform.type = GL_FLOAT_MAT4;
				else if (StartsWith(line, "mat3"))		uniform.type = GL_FLOAT_MAT3;
				else if (StartsWith(line, "mat2"))		uniform.type = GL_FLOAT_MAT2;
				else if (StartsWith(line, "int"))		uniform.type = GL_INT;
				else if (StartsWith(line, "unsigned"))	uniform.type = GL_UNSIGNED_INT;
				else if (StartsWith(line, "bool"))		uniform.type = GL_BOOL;
				else if (StartsWith(line, "double"))	uniform.type = GL_DOUBLE;
				else
				{
					Error("Uniform member ( %s ) has an unsupported type.", uniform.name.c_str());
					return false;
				}

				if (assignment != std::string::npos)
				{
					// Extract assignment value.
					uniform.value = line.substr(assignment + 1);
					RemoveWhitespace(uniform.value);

					if (uniform.value.length() == 0)
					{
						Error("Could not resolve assignment of block member ( %s )", uniform.name.c_str());
						return false;
					}
				}

				i += line.size();
			}

			// Add all members to the buffer.
			for (const auto& uniform : uniforms)
			{
				switch (uniform.type)
				{
				case GL_FLOAT:			buffer->AddUniform<float>(uniform.name); break;
				case GL_FLOAT_VEC2:		buffer->AddUniform<vec2>(uniform.name); break;
				case GL_FLOAT_VEC3:		buffer->AddUniform<vec3>(uniform.name); break;
				case GL_FLOAT_VEC4:		buffer->AddUniform<vec4>(uniform.name); break;
				case GL_FLOAT_MAT4:		buffer->AddUniform<mat4>(uniform.name); break;
				case GL_FLOAT_MAT3:		buffer->AddUniform<mat3>(uniform.name); break;
				case GL_FLOAT_MAT2:		buffer->AddUniform<mat2>(uniform.name); break;
				case GL_INT:			buffer->AddUniform<int>(uniform.name); break;
				case GL_UNSIGNED_INT:	buffer->AddUniform<unsigned>(uniform.name); break;
				case GL_BOOL:			buffer->AddUniform<bool>(uniform.name); break;
				case GL_DOUBLE:			buffer->AddUniform<double>(uniform.name); break;
				}
			}
			buffer->InitBuffer();

			/* Parse default values */
			for (auto& uniform : uniforms)
			{
				if (uniform.value.empty())
					continue;

				auto guard = MakeScopeGaurd([&]() {
					Error("Default value assignment of ( %s ) could not be parsed.", uniform.name.c_str()); 
				});

				vec4 vec; unsigned u; int i;
				switch (uniform.type)
				{
				case GL_FLOAT:
					if (sscanf(uniform.value.c_str(), "%f", &vec.x) != 1) 
						return false;

					buffer->SetUniform(uniform.name, vec.x);
					break;

				case GL_FLOAT_VEC2:
					if (sscanf(uniform.value.c_str(), "(%f,%f)", &vec.x, &vec.y) != 2)
						return false;

					buffer->SetUniform(uniform.name, vec2(vec.x, vec.y));
					break;

				case GL_FLOAT_VEC3:
					if (sscanf(uniform.value.c_str(), "(%f,%f,%f)", &vec.x, &vec.y, &vec.z) != 3)
						return false;

					buffer->SetUniform(uniform.name, vec3(vec.x, vec.y, vec.z));
					break;

				case GL_FLOAT_VEC4:
					if (sscanf(uniform.value.c_str(), "(%f,%f,%f,%f)", &vec.x, &vec.y, &vec.z, &vec.w) != 4)
						return false;

					buffer->SetUniform(uniform.name, vec4(vec.x, vec.y, vec.z, vec.w));
					break;

				case GL_INT:
					if (sscanf(uniform.value.c_str(), "%i", &i) != 1)
						return false;

					buffer->SetUniform(uniform.name, i);
					break;

				case GL_UNSIGNED_INT:
					if (sscanf(uniform.value.c_str(), "%u", &u) != 1)
						return false;

					buffer->SetUniform(uniform.name, u);
					break;

				case GL_BOOL:
					if (uniform.value == "true")
					{
						u = 1;
					}
					else if (uniform.value == "false")
					{
						u = 0;
					}
					else if (sscanf(uniform.value.c_str(), "%d", &u) != 1)
					{
						return false;
					}

					buffer->SetUniform(uniform.name, !!u);
					break;
				}

				guard.Dismiss();
			}

			bufferBindings.emplace_back(name, Id, makeTemplate ? buffer : nullptr);

			if (makeStatic)
			{
				auto staticBuf = UniformBuffer::MakeNew();
				staticBuf->Copy(*buffer);

				buffers.Add(staticBuf, Id);
			}
		}
		else
		{
			if (rawStruct.find('=') != std::string::npos)
			{
				Error("Default uniform buffer member assignment is only available on buffers that are marked as 'template' or 'static'.");
				return false;
			}

			bufferBindings.emplace_back(name, Id, nullptr);
		}

		uniformBuffers += uniformStruct;

		return true;
	}

	bool Shader::ParseSamplers(const Block& block)
	{
		ASSERT(block.type == BlockType::Samplers, "Expected a Sampler block type.");

		size_t pos = block.start;

		char type[32] = { '\0' };
		char name[128] = { '\0' };
		unsigned Id = 0;

		while (pos < block.end)
		{
			if (!std::isspace(block.source[pos]))
			{
				if (sscanf(&block.source[pos], "%31s %127s : %u", type, name, &Id) != 3)
				{
					Error("Sampler could not be parsed.");
					return false;
				}

				textureBindings.emplace_back(name, Id);

				// Add OpenGL correct entry into _Samplers string.
				samplers += "uniform " + std::string(type) + ' ' + std::string(name) + ";\n";

				// Jump past the line we just read.
				pos = block.source.find(';', pos);
			}

			pos++;
		}

		return true;
	}

	bool Shader::IsLoaded() const
	{
		return loaded;
	}

	void Shader::Unload()
	{
		loaded = false;

		textures.Clear();
		buffers.Clear();
		
		for (auto itr = variants.begin(); itr != variants.end(); ++itr)
		{
			itr->second.Unload();
		}
		variants.clear();

		textureBindings.clear();
		bufferBindings.clear();

		attributes.clear();
		samplers.clear();
		uniformBuffers.clear();
		vertexSource.clear();
		geometrySource.clear();
		fragmentSrouce.clear();
	}

	void Shader::Bind()
	{
		Bind(ShaderVariantControl());
	}

	void Shader::Bind(const ShaderVariantControl& definitions)
	{
		ASSERT(IsLoaded(), "Must have a shader loaded to call this function.");

		auto itr = variants.find(definitions);
		if (itr == variants.end())
		{
			// This shader variant is new and needs to be created.
			auto& variant = variants.emplace(definitions, ShaderVariant()).first->second;

			const std::string defines = definitions.GetString();

			if (!variant.Load(
				commonHeader + uniformBuffers + samplers + defines,
				attributes + vertexSource,
				geometrySource,
				fragmentSrouce))
			{
				if (!defines.empty())
				{
					Error("With variant definitions:\n%s", defines.c_str());
				}

				// Instead of doing nothing, we load a hard-coded pink shader on failure.
				if (!variant.Load(
					commonHeader,
					"layout(location = 0) in vec4 a_vert;\n"
					"void main()\n{\n"
					"	gl_Position = Jwl_MVP * a_vert;\n"
					"}\n",
					"",
					"out vec4 outColor;\n"
					"void main()\n{\n"
					"	outColor = vec4(1.0f, 0.5f, 0.7f, 1.0f);\n"
					"}\n"))
				{
					ASSERT(false, "Fallback pink-shader failed to compile.");
				}
			}
			else
			{
				// Make sure the samplers are all set to the correct bindings.
				for (auto& binding : textureBindings)
				{
					unsigned location = glGetUniformLocation(variant.hProgram, binding.name.c_str());
					// Not finding a location is not an error.
					// It is most likely because a uniform has been optimized away.
					if (location != GL_INVALID_INDEX)
					{
						glProgramUniform1i(variant.hProgram, location, binding.unit);
					}
				}

				// Make sure the UniformBuffers are all set to the correct bindings.
				for (auto& binding : bufferBindings)
				{
					unsigned block = glGetUniformBlockIndex(variant.hProgram, ("Jwl_User_" + binding.name).c_str());
					if (block != GL_INVALID_INDEX)
					{
						glUniformBlockBinding(variant.hProgram, block, binding.unit);
					}
				}
			}

			variant.Bind();
		}
		else
		{
			itr->second.Bind();
		}

		/* Bind global shader resources */
		textures.Bind();
		buffers.Bind();
	}

	void Shader::UnBind()
	{
		glUseProgram(GL_NONE);

		textures.UnBind();
		buffers.UnBind();
	}

	UniformBuffer::Ptr Shader::CreateBufferFromTemplate(unsigned unit) const
	{
		auto newBuf = UniformBuffer::MakeNew();

		for (auto& binding : bufferBindings)
		{
			if (binding.unit == unit)
			{
				ASSERT(binding.templateBuff, "Binding unit ( %d ) is not marked as 'template' in the shader.", unit);

				newBuf->Copy(*binding.templateBuff);

				return newBuf;
			}
		}

		ASSERT(false, "Specified binding unit ( %d ) does not have a uniform buffer bound to it.", unit);
		return newBuf;
	}

	const std::vector<BufferBinding>& Shader::GetBufferBindings() const
	{
		return bufferBindings;
	}

	//-----------------------------------------------------------------------------------------------------

	Shader::ShaderVariant::~ShaderVariant()
	{
		Unload();
	}

	bool Shader::ShaderVariant::Load(const std::string& _header, const std::string& vertSource, const std::string& geomSource, const std::string& fragSource)
	{
		hProgram = glCreateProgram();

		/* Load vertex shader */
		if (!vertSource.empty())
		{
			hVertShader = glCreateShader(GL_VERTEX_SHADER);

			std::string source = _header + vertSource;
			const char* charSource = source.c_str();
			glShaderSource(hVertShader, 1, &charSource, nullptr);

			if (!CompileShader(hVertShader))
			{
				Error("Shader variant's vertex stage failed to compile.");
				Unload();
				return false;
			}

			glAttachShader(hProgram, hVertShader);
		}

		/* Load geometry shader */
		if (!geomSource.empty())
		{
			hGeomShader = glCreateShader(GL_GEOMETRY_SHADER);

			std::string source = _header + geomSource;
			const char* charSource = source.c_str();
			glShaderSource(hGeomShader, 1, &charSource, nullptr);

			if (!CompileShader(hGeomShader))
			{
				Error("Shader variant's geometry stage failed to compile.");
				Unload();
				return false;
			}

			glAttachShader(hProgram, hGeomShader);
		}

		/* Load fragment shader */
		if (!fragSource.empty())
		{
			hFragShader = glCreateShader(GL_FRAGMENT_SHADER);

			std::string source = _header + fragSource;
			const char* charSource = source.c_str();
			glShaderSource(hFragShader, 1, &charSource, nullptr);

			if (!CompileShader(hFragShader))
			{
				Error("Shader variant's fragment stage failed to compile.");
				Unload();
				return false;
			}

			glAttachShader(hProgram, hFragShader);
		}

		if (!LinkProgram(hProgram))
		{
			Error("Shader variant failed to link.");
			Unload();
			return false;
		}

		/* Initialize built-in uniform blocks */
		unsigned cameraBlock = glGetUniformBlockIndex(hProgram, "Jwl_Camera_Uniforms");
		unsigned modelBlock = glGetUniformBlockIndex(hProgram, "Jwl_Model_Uniforms");
		unsigned engineBlock = glGetUniformBlockIndex(hProgram, "Jwl_Engine_Uniforms");
		unsigned timeBlock = glGetUniformBlockIndex(hProgram, "Jwl_Time_Uniforms");

		ASSERT(cameraBlock != GL_INVALID_INDEX, "Camera-Block could not be created.");
		ASSERT(modelBlock != GL_INVALID_INDEX, "Model-Block could not be created.");
		ASSERT(engineBlock != GL_INVALID_INDEX, "Engine-Block could not be created.");
		ASSERT(timeBlock != GL_INVALID_INDEX, "Time-Block could not be created.");

		glUniformBlockBinding(hProgram, cameraBlock, static_cast<unsigned>(UniformBufferSlot::Camera));
		glUniformBlockBinding(hProgram, modelBlock, static_cast<unsigned>(UniformBufferSlot::Model));
		glUniformBlockBinding(hProgram, engineBlock, static_cast<unsigned>(UniformBufferSlot::Engine));
		glUniformBlockBinding(hProgram, timeBlock, static_cast<unsigned>(UniformBufferSlot::Time));

		return true;
	}

	void Shader::ShaderVariant::Unload()
	{
		if (hVertShader != GL_NONE)
		{
			glDetachShader(hProgram, hVertShader);
			glDeleteShader(hVertShader);
			hVertShader = GL_NONE;
		}

		if (hGeomShader != GL_NONE)
		{
			glDetachShader(hProgram, hGeomShader);
			glDeleteShader(hGeomShader);
			hGeomShader = GL_NONE;
		}

		if (hFragShader != GL_NONE)
		{
			glDetachShader(hProgram, hFragShader);
			glDeleteShader(hFragShader);
			hFragShader = GL_NONE;
		}

		if (hProgram != GL_NONE)
		{
			glDeleteProgram(hProgram);
			hProgram = GL_NONE;
		}
	}

	void Shader::ShaderVariant::Bind() const
	{
		ASSERT(hProgram != GL_NONE, "ShaderVariant cannot be bound because it is not loaded.");

		glUseProgram(hProgram);
	}
}
