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
		"\n"
		"layout(location = 0) in vec4 a_vert;\n"
		"layout(location = 1) in vec2 a_uv;\n"
		"out vec2 texcoord;\n"
		"void main()\n{\n"
			"\tgl_Position = Jwl_MVP * a_vert;\n"
			"\ttexcoord = a_uv;\n"
		"}\n";

	constexpr char passThroughFragment[] =
		"\n"
		"uniform sampler2D sTex;\n"
		"in vec2 texcoord;\n"
		"out vec4 outColor;\n"
		"void main()\n{\n"
			"\toutColor = texture(sTex, texcoord);\n"
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

	constexpr char builtInBuffers[] =
		"\n"
		"layout(std140) uniform Jwl_Camera_Uniforms\n"
		"{\n"
			"\tmat4 Jwl_View;\n"
			"\tmat4 Jwl_Proj;\n"
			"\tmat4 Jwl_ViewProj;\n"
			"\tmat4 Jwl_InvView;\n"
			"\tmat4 Jwl_InvProj;\n"
		"};\n"
		"layout(std140) uniform Jwl_Model_Uniforms\n"
		"{\n"
			"\tmat4 Jwl_MVP;\n"
			"\tmat4 Jwl_ModelView;\n"
			"\tmat4 Jwl_Model;\n"
			"\tmat4 Jwl_InvModel;\n"
		"};\n"
		"layout(std140) uniform Jwl_Engine_Uniforms\n"
		"{\n"
			"\tvec4 ScreenParams;\n"
		"};\n"
		"layout(std140) uniform Jwl_Time_Uniforms\n"
		"{\n"
			"\tvec4 Jwl_Time;\n"
			"\tvec4 Jwl_Sin;\n"
			"\tvec4 Jwl_Cos;\n"
			"\tfloat Jwl_DeltaTime;\n"
		"};\n"
		"\n";

	constexpr char builtInLightingFunctions[] =
		// These are enum values from Jwl::Light::Type
		"bool JWL_IS_POINT_LIGHT(uint type) { return type == 0u; }\n"
		"bool JWL_IS_DIRECTIONAL_LIGHT(uint type) { return type == 1u; }\n"
		"bool JWL_IS_SPOT_LIGHT(uint type) { return type == 2u; }\n"
		""
		"vec3 JWL_COMPUTE_LIGHT(vec3 normal, vec3 surfacePos, vec3 color, vec3 lightPos, vec3 direction, float attenConstant, float attenLinear, float attenQuadratic, float angle, uint type)\n"
		"{\n"
		"	if (JWL_IS_POINT_LIGHT(type))\n"
		"	{\n"
		"		lightPos = (Jwl_View * vec4(lightPos, 1.0)).xyz;\n"
		"		vec3 lightDir = lightPos - surfacePos;\n"
		"		float dist = length(lightDir);\n"
		"		lightDir /= dist;\n"
		"		float NdotL = dot(normal, lightDir);\n"
		""
		"		if (NdotL > 0.0)\n"
		"		{\n"
		"			float attenuation = 1.0 / (attenConstant + attenLinear * dist + attenQuadratic * dist * dist);\n"
		"			return color * NdotL * attenuation;\n"
		"		}\n"
		"	}\n"
		"	else if (JWL_IS_DIRECTIONAL_LIGHT(type))\n"
		"	{\n"
		"		vec3 lightDir = mat3(Jwl_View) * -direction;\n"
		"		float NdotL = dot(normal, lightDir);\n"
		""
		"		return color * max(NdotL, 0.0);\n"
		"	}\n"
		"	else\n" // Spot light is assumed if the other cases fail.
		"	{\n"
		"		lightPos = (Jwl_View * vec4(lightPos, 1.0)).xyz;\n"
		"		vec3 lightDir = lightPos - surfacePos;\n"
		"		float dist = length(lightDir);\n"
		"		lightDir /= dist;\n"
		"		float NdotL = dot(normal, lightDir);\n"
		""
		"		if (NdotL > 0.0)\n"
		"		{\n"
		"			float attenuation = 1.0 / (attenConstant + attenLinear * dist + attenQuadratic * dist * dist);\n"
		"			vec3 coneDir = mat3(Jwl_View) * -direction;\n"
		"			float coneCos = dot(lightDir, coneDir);\n"
		"			if (coneCos > angle)\n"
		"			{\n"
		"				float coneFactor = (coneCos - angle) / (1.0 - angle);\n"
		"				return color * NdotL * attenuation * min(coneFactor, 1.0);\n"
		"			}\n"
		"		}\n"
		"	}\n"
		""
		"	return vec3(0.0);\n"
		"}\n";

	constexpr char builtInLightingMacros[] =
		"#define is_point_light(light) JWL_IS_POINT_LIGHT(light##.Type)\n"
		"#define is_directional_light(light) JWL_IS_DIRECTIONAL_LIGHT(light##.Type)\n"
		"#define is_spot_light(light) JWL_IS_SPOT_LIGHT(light##.Type)\n"
		"#define compute_light(light, normal, pos) "
		"JWL_COMPUTE_LIGHT(normal, pos, light##.Color, light##.Position, light##.Direction, light##.AttenuationConstant, light##.AttenuationLinear, light##.AttenuationQuadratic, light##.Angle, light##.Type)\n";
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

	void ShaderVariantControl::Define(const std::string& name)
	{
		if (IsDefined(name))
		{
			return;
		}

		defines.push_back(name);
		UpdateHash();
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

	bool ShaderVariantControl::IsDefined(const std::string& name) const
	{
		for (auto& define : defines)
		{
			if (define == name)
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
		for (unsigned i = 0; i < defines.size(); i++)
		{
			if (defines[i] == name)
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
		for (auto& define : defines)
		{
			result += "#define " + define + '\n';
		}

		return result;
	}

	unsigned ShaderVariantControl::GetHash() const
	{
		return hash;
	}

	bool ShaderVariantControl::operator==(const ShaderVariantControl& svc) const
	{
		return defines == svc.defines;
	}

	bool ShaderVariantControl::operator!=(const ShaderVariantControl& svc) const
	{
		return defines != svc.defines;
	}

	void ShaderVariantControl::UpdateHash()
	{
		// Sort alphabetically to eliminate order of defines creating different hashes.
		std::sort(defines.begin(), defines.end());

		std::hash<std::string> str_hash;
		hash = 0;

		for (auto& define : defines)
		{
			hash = hash ^ str_hash(define);
		}
	}

	//-----------------------------------------------------------------------------------------------------

	std::string Shader::version;
	std::string Shader::header;

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
		if (version.empty())
		{
			int major = 0;
			int minor = 0;
			glGetIntegerv(GL_MAJOR_VERSION, &major);
			glGetIntegerv(GL_MINOR_VERSION, &minor);

			// Our minimum supported version is 3.3, where the GLSL
			// version identifier begins to be symmetrical with the GL version.
			version = "#version " + std::to_string(major) + std::to_string(minor) + "0\n";
		}

		// Prepare common shader code.
		if (header.empty())
		{
			header += builtInBuffers;
			header += builtInLightingFunctions;
			header += builtInLightingMacros;
		}

		/* Clean up file for easier parsing */
		// Remove comments starting with '//'.
		size_t pos = source.find("//");
		while (pos != std::string::npos)
		{
			size_t endl = source.find('\n', pos);
			source.erase(pos, (endl - pos) + 1);

			pos = source.find("//");
		}

		// Remove multi-line comments.
		pos = source.find("/*");
		while (pos != std::string::npos)
		{
			size_t endl = source.find("*/", pos);
			source.erase(pos, (endl - pos) + 2);

			pos = source.find("/*");
		}

		// Reduce extra whitespace. This will make parsing of the file much easier.
		RemoveRedundantWhitespace(source);

		pos = 0;
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
			// The last non-whitespace character will be the end of the member's name.
			size_t nameEnd = uniformStruct.find_last_not_of(" \t\n", assignmentPos);
			if (nameEnd == std::string::npos)
			{
				Error("Could not resolve assignment of block member.");
				return false;
			}

			// We have to remove the assignment operation because it's our own
			// syntax and OpenGL will not recognize it. We will erase everything between the
			// end of the name and the semicolon.
			size_t semicolon = uniformStruct.find(';', nameEnd);
			if (semicolon == std::string::npos)
			{
				Error("Block member assignment missing trailing semicolon.");
				return false;
			}

			uniformStruct.erase(nameEnd - 1, (semicolon - nameEnd) + 1);

			// We can only safely start searching at 'nameEnd' because some characters
			// after that position have been deleted.
			assignmentPos = uniformStruct.find('=', nameEnd);
		}

		uniformStruct = "layout(std140) uniform Jwl_User_" + std::string(name) + "\n{\n" + uniformStruct + "} " + std::string(name) + ";\n";

		/* Create template and the shader owned buffer */
		if (makeTemplate || makeStatic)
		{
			auto buffer = UniformBuffer::MakeNew();

			struct uniformMember
			{
				std::string name;
				std::string value;
				GLenum type;
				unsigned size;
			};

			std::vector<uniformMember> uniforms;

			// Find all uniforms and parse their information.
			for (unsigned i = 0; i < rawStruct.size(); i++)
			{
				if (!std::isspace(rawStruct[i]))
				{
					uniforms.push_back(uniformMember());

					size_t semicolon = rawStruct.find(';', i) + 1;

					if (semicolon == std::string::npos)
					{
						Error("Uniform member missing trailing semicolon.");
						return false;
					}

					std::string line = rawStruct.substr(i, (semicolon - i) - 1);
					
					// Resolve type.
					#define IfFound(str, t, s) if (line.find((str)) != std::string::npos) { uniforms.back().type = (t); uniforms.back().size = (s); }

					IfFound("float", GL_FLOAT, sizeof(float))
					else IfFound("vec2", GL_FLOAT_VEC2, sizeof(vec2))
					else IfFound("vec3", GL_FLOAT_VEC3, sizeof(vec3))
					else IfFound("vec4", GL_FLOAT_VEC4, sizeof(vec4))
					else IfFound("double", GL_DOUBLE, sizeof(double))
					else IfFound("dvec2", GL_DOUBLE_VEC2, sizeof(double) * 2)
					else IfFound("dvec3", GL_DOUBLE_VEC3, sizeof(double) * 3)
					else IfFound("dvec4", GL_DOUBLE_VEC4, sizeof(double) * 4)
					else IfFound("int", GL_INT, sizeof(int))
					else IfFound("ivec2", GL_INT_VEC2, sizeof(vec2))
					else IfFound("ivec3", GL_INT_VEC3, sizeof(vec3))
					else IfFound("ivec4", GL_INT_VEC4, sizeof(vec4))
					else IfFound("unsigned", GL_UNSIGNED_INT, sizeof(unsigned))
					else IfFound("uvec2", GL_UNSIGNED_INT_VEC2, sizeof(vec2))
					else IfFound("uvec3", GL_UNSIGNED_INT_VEC3, sizeof(vec3))
					else IfFound("uvec4", GL_UNSIGNED_INT_VEC4, sizeof(vec4))
					else IfFound("bool", GL_BOOL, sizeof(int))
					else IfFound("bvec2", GL_BOOL_VEC2, sizeof(vec2))
					else IfFound("bvec3", GL_BOOL_VEC3, sizeof(vec3))
					else IfFound("bvec4", GL_BOOL_VEC4, sizeof(vec4))
					else IfFound("mat2", GL_FLOAT_MAT2, sizeof(float) * 4)
					else IfFound("mat3", GL_FLOAT_MAT3, sizeof(float) * 9)
					else IfFound("mat4", GL_FLOAT_MAT4, sizeof(float) * 16)
					else IfFound("mat2x3", GL_FLOAT_MAT2x3, sizeof(float) * 2 * 3)
					else IfFound("mat2x4", GL_FLOAT_MAT2x4, sizeof(float) * 2 * 4)
					else IfFound("mat3x2", GL_FLOAT_MAT3x2, sizeof(float) * 3 * 2)
					else IfFound("mat3x4", GL_FLOAT_MAT3x4, sizeof(float) * 3 * 4)
					else IfFound("mat4x2", GL_FLOAT_MAT4x2, sizeof(float) * 4 * 2)
					else IfFound("mat4x3", GL_FLOAT_MAT4x3, sizeof(float) * 4 * 3)
					else IfFound("dmat2", GL_DOUBLE_MAT2, sizeof(double) * 4)
					else IfFound("dmat3", GL_DOUBLE_MAT3, sizeof(double) * 9)
					else IfFound("dmat4", GL_DOUBLE_MAT4, sizeof(double) * 16)
					else IfFound("dmat2x3", GL_DOUBLE_MAT2x3, sizeof(double) * 2 * 3)
					else IfFound("dmat2x4", GL_DOUBLE_MAT2x4, sizeof(double) * 2 * 4)
					else IfFound("dmat3x2", GL_DOUBLE_MAT3x2, sizeof(double) * 3 * 2)
					else IfFound("dmat3x4", GL_DOUBLE_MAT3x4, sizeof(double) * 3 * 4)
					else IfFound("dmat4x2", GL_DOUBLE_MAT4x2, sizeof(double) * 4 * 2)
					else IfFound("dmat4x3", GL_DOUBLE_MAT4x3, sizeof(double) * 4 * 3)
					else
					{
						Error("Uniform member is of unknown type.");
						return false;
					}
					#undef IfFound

					/* Resolve Name */
					// If we don't find a '=', the result will be npos, causing the search to simply start at the end of the string.
					size_t assignment = line.find('=');

					// The last non-whitespace character before the '=' will be the end of the member's name.
					size_t nameEnd = line.find_last_not_of(" \t\n=", assignment);
					if (nameEnd == std::string::npos)
					{
						Error("Could not find name of uniform member.");
						return false;
					}

					// Continue to search until we hit more whitespace to isolate the name.
					size_t nameStart = line.find_last_of(" \t\n", nameEnd) + 1;
					if (nameStart == std::string::npos)
					{
						Error("Could not find name of uniform member.");
						return false;
					}

					uniforms.back().name = line.substr(nameStart, nameEnd - nameStart + 1);

					if (assignment != std::string::npos)
					{
						// Extract assignment value.
						uniforms.back().value = line.substr(assignment + 1);
						RemoveWhitespace(uniforms.back().value);

						if (uniforms.back().value.length() == 0)
						{
							Error("Could not resolve assignment of block member ( %s )", uniforms.back().name.c_str());
							return false;
						}
					}

					i += line.size();
				}
			}

			// Add all members to the buffer.
			for (unsigned i = 0; i < uniforms.size(); i++)
			{
				buffer->AddUniform(uniforms[i].name, uniforms[i].size);
			}
			// We finalize the buffer by initializing it so it is ready to start receive values.
			buffer->InitBuffer();

			/* Resolve Default set values */
			for (unsigned i = 0; i < uniforms.size(); i++)
			{
				if (uniforms[i].value.empty())
					continue;

				// Macros to help with switch statement.
				#define EXIT() \
					Error("Assignment of \"%s\" could not be parsed.", uniforms[i].name.c_str()); \
					return false;
				#define SET_UNIFORM(memberName, ...) \
					if (!buffer->IsUniform(memberName)) {										\
						Error("Assignment of \"%s\" could not be parsed.", memberName.c_str()); \
						return false;															\
					} else {																	\
						buffer->SetUniform(memberName, ##__VA_ARGS__);							\
					}

				// Parse value based on type.
				switch (uniforms[i].type)
				{
				case GL_FLOAT:
				{
					float val;
					if (sscanf(uniforms[i].value.c_str(), "%f", &val) != 1)
					{
						EXIT();
					}

					SET_UNIFORM(uniforms[i].name, val);
				} break;
				case GL_FLOAT_VEC2:
				{
					float val1, val2;
					if (sscanf(uniforms[i].value.c_str(), "(%f,%f)", &val1, &val2) != 2)
					{
						EXIT();
					}

					SET_UNIFORM(uniforms[i].name, vec2(val1, val2));
				} break;
				case GL_FLOAT_VEC3:
				{
					float val1, val2, val3;
					if (sscanf(uniforms[i].value.c_str(), "(%f,%f,%f)", &val1, &val2, &val3) != 3)
					{
						EXIT();
					}

					SET_UNIFORM(uniforms[i].name, vec3(val1, val2, val3));
				} break;
				case GL_FLOAT_VEC4:
				{
					float val1, val2, val3, val4;
					if (sscanf(uniforms[i].value.c_str(), "(%f,%f,%f,%f)", &val1, &val2, &val3, &val4) != 4)
					{
						EXIT();
					}

					SET_UNIFORM(uniforms[i].name, vec4(val1, val2, val3, val4));
				} break;
				case GL_INT:
				{
					int val;
					if (sscanf(uniforms[i].value.c_str(), "%i", &val) != 1)
					{
						EXIT();
					}

					SET_UNIFORM(uniforms[i].name, val);
				} break;
				case GL_UNSIGNED_INT:
				{
					unsigned val;
					if (sscanf(uniforms[i].value.c_str(), "%u", &val) != 1)
					{
						EXIT();
					}

					SET_UNIFORM(uniforms[i].name, val);
				} break;
				case GL_BOOL:
				{
					unsigned val = 0;
					if (uniforms[i].value == "true")
					{
						val = 1;
					}
					else if (uniforms[i].value == "false")
					{
						val = 0;
					}
					else
					{
						if (sscanf(uniforms[i].value.c_str(), "%d", &val) != 1)
						{
							EXIT();
						}
					}
					SET_UNIFORM(uniforms[i].name, val == 0 ? false : true);
				} break;

				case GL_UNSIGNED_INT_VEC2:
				case GL_UNSIGNED_INT_VEC3:
				case GL_UNSIGNED_INT_VEC4:
				case GL_INT_VEC2:
				case GL_INT_VEC3:
				case GL_INT_VEC4:
				case GL_BOOL_VEC2:
				case GL_BOOL_VEC3:
				case GL_BOOL_VEC4:
				case GL_DOUBLE:
				case GL_DOUBLE_VEC2:
				case GL_DOUBLE_VEC3:
				case GL_DOUBLE_VEC4:
				case GL_FLOAT_MAT2:
				case GL_FLOAT_MAT3:
				case GL_FLOAT_MAT4:
				case GL_FLOAT_MAT2x3:
				case GL_FLOAT_MAT2x4:
				case GL_FLOAT_MAT3x2:
				case GL_FLOAT_MAT3x4:
				case GL_FLOAT_MAT4x2:
				case GL_FLOAT_MAT4x3:
				case GL_DOUBLE_MAT2:
				case GL_DOUBLE_MAT3:
				case GL_DOUBLE_MAT4:
				case GL_DOUBLE_MAT2x3:
				case GL_DOUBLE_MAT2x4:
				case GL_DOUBLE_MAT3x2:
				case GL_DOUBLE_MAT3x4:
				case GL_DOUBLE_MAT4x2:
				case GL_DOUBLE_MAT4x3:
					Error("Default \"%s\" specifies a uniform type that cannot be default initialized.", uniforms[i].name.c_str());
					return false;
					break;

				default:
					Error("Default \"%s\" specifies a uniform with an unsupported type.", uniforms[i].name.c_str());
					return false;
				}

				// Remove utility macros.
				#undef EXIT
				#undef SET_UNIFORM
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
			if (uniformStruct.find('=') != std::string::npos)
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

			if (!variant.Load(
				version + header + definitions.GetString() + uniformBuffers + samplers,
				attributes + vertexSource,
				geometrySource,
				fragmentSrouce))
			{
				if (!definitions.IsEmpty())
				{
					Error("Active variant definitions:\n%s", definitions.GetString().c_str());
				}

				// Instead of doing nothing, we load a hard-coded pink shader on failure.
				if (!variant.Load(
					version + builtInBuffers,
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
					ASSERT(location != GL_INVALID_INDEX, "Sampler location of ( %s ) for shader variant could not be found.", binding.name.c_str());

					glProgramUniform1i(variant.hProgram, location, binding.unit);
				}

				// Make sure the UniformBuffers are all set to the correct bindings.
				for (auto& binding : bufferBindings)
				{
					unsigned block = glGetUniformBlockIndex(variant.hProgram, ("Jwl_User_" + binding.name).c_str());
					ASSERT(block != GL_INVALID_INDEX, "Block index of ( %s ) for shader variant could not be found.", binding.name.c_str());

					// We offset the binding unit to make room for the engine defined binding points.
					glUniformBlockBinding(variant.hProgram, block, binding.unit);
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
