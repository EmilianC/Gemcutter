// Copyright (c) 2017 Emilian Cioca
#include "Shader.h"
#include "gemcutter/Application/FileSystem.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Math/Vector.h"
#include "gemcutter/Utilities/ScopeGuard.h"
#include "gemcutter/Utilities/String.h"

#include <cctype>
#include <functional>
#include <GL/glew.h>

namespace
{
	// Built in shaders.
	constexpr std::string_view passThroughVertex = R"(
		layout(location = 0) in vec4 a_vert;
		layout(location = 1) in vec2 a_uv;
		out vec2 texcoord;
		void main()
		{
			gl_Position = Gem_MVP * a_vert;
			texcoord = a_uv;
		}
	)";

	constexpr std::string_view passThroughFragment = R"(
		uniform sampler2D sTex;
		in vec2 texcoord;
		out vec4 outColor;
		void main()
		{
			outColor = texture(sTex, texcoord);
		}
	)";

	constexpr std::string_view fallbackVertex = R"(
		layout(location = 0) in vec4 a_vert;
		void main()
		{
			gl_Position = Gem_MVP * a_vert;
		}
	)";

	constexpr std::string_view fallbackFragment = R"(
		out vec4 outColor;
		void main()
		{
			outColor = vec4(1.0f, 0.5f, 0.7f, 1.0f);
		}
	)";

	constexpr std::string_view passThroughProgram = R"(
		Attributes{
			vec4 a_vert : 0;
			vec2 a_uv : 1;
		}
		Vertex{
			out vec2 texcoord;
			void main()
			{
				gl_Position = Gem_MVP * a_vert;
				texcoord = a_uv;
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
				outColor = texture(sTex, texcoord);
			}
		}
	)";

	constexpr std::string_view header = R"(
		#define M_PI 3.14159265358979323846
		#define M_E 2.71828182845904523536
		#define M_LOG2E 1.44269504088896340736
		#define M_LOG10E 0.434294481903251827651
		#define M_LN2 0.693147180559945309417
		#define M_LN10 2.30258509299404568402

		layout(std140) uniform Gem_Camera_Uniforms{
			mat4 Gem_View;
			mat4 Gem_Proj;
			mat4 Gem_ViewProj;
			mat4 Gem_InvView;
			mat4 Gem_InvProj;
			vec3 Gem_CameraPosition;
		};

		layout(std140) uniform Gem_Model_Uniforms{
			mat4 Gem_MVP;
			mat4 Gem_ModelView;
			mat4 Gem_Model;
			mat4 Gem_InvModel;
			mat3 Gem_NormalToWorld;
		};

		layout(std140) uniform Gem_Engine_Uniforms{
			vec4 ScreenParams;
		};

		layout(std140) uniform Gem_Time_Uniforms{
			vec4 Gem_Time;
			vec4 Gem_Sin;
			vec4 Gem_Cos;
			float Gem_DeltaTime;
		};

		// Normal mapping helper.
		mat3 make_TBN(vec3 normal, vec3 tangent, float handedness){
			vec3 N = Gem_NormalToWorld * normal;
			vec3 T = Gem_NormalToWorld * tangent;
			vec3 B = cross(T, N) * handedness;
			return mat3(T, B, N);
		}

		float linear_to_sRGB(float x)
		{
			if (x <= 0.00031308) return 12.92 * x;
			else return 1.055 * pow(x, (1.0 / 2.4)) - 0.055;
		}
		vec3 linear_to_sRGB(vec3 v) { return vec3(linear_to_sRGB(v.x), linear_to_sRGB(v.y), linear_to_sRGB(v.z)); }

		float sRGB_to_linear(float x)
		{
			if (x <= 0.040449936) return x / 12.92;
			else return pow((x + 0.055) / 1.055, 2.4);
		}
		vec3 sRGB_to_linear(vec3 v) { return vec3(sRGB_to_linear(v.x), sRGB_to_linear(v.y), sRGB_to_linear(v.z)); }

		// These are enum values from gem::Light::Type.
		bool GEM_IS_POINT_LIGHT(uint type) { return type == 0u; }
		bool GEM_IS_DIRECTIONAL_LIGHT(uint type) { return type == 1u; }
		bool GEM_IS_SPOT_LIGHT(uint type) { return type == 2u; }

		vec3 GEM_COMPUTE_LIGHT(vec3 normal, vec3 surfacePos, vec3 color, vec3 lightPos, vec3 direction, float attenLinear, float attenQuadratic, float angle, uint type)
		{
			if (GEM_IS_POINT_LIGHT(type))
			{
				vec3 lightDir = lightPos - surfacePos;
				float dist = length(lightDir);
				lightDir /= dist;
				float NdotL = dot(normal, lightDir);

				if (NdotL > 0.0)
				{
					float attenuation = 1.0 / (0.75 + attenLinear * dist + attenQuadratic * dist * dist);
					return color * NdotL * attenuation;
				}
			}
			else if (GEM_IS_DIRECTIONAL_LIGHT(type))
			{
				float NdotL = dot(normal, -direction);
				return color * max(NdotL, 0.0);
			}
			else // Spot light is assumed if the other cases fail.
			{
				vec3 lightDir = lightPos - surfacePos;
				float dist = length(lightDir);
				lightDir /= dist;
				float NdotL = dot(normal, lightDir);

				if (NdotL > 0.0)
				{
					float attenuation = 1.0 / (0.75 + attenLinear * dist + attenQuadratic * dist * dist);
					float coneCos = dot(lightDir, -direction);
					if (coneCos > angle)
					{
						float coneFactor = (coneCos - angle) / (1.0 - angle);
						return color * NdotL * attenuation * min(coneFactor, 1.0);
					}
				}
			}
			return vec3(0.0);
		}

		#define is_point_light(light) GEM_IS_POINT_LIGHT(light.Type)
		#define is_directional_light(light) GEM_IS_DIRECTIONAL_LIGHT(light.Type)
		#define is_spot_light(light) GEM_IS_SPOT_LIGHT(light.Type)
		#define compute_light(light, normal, pos) GEM_COMPUTE_LIGHT(normal, pos, light.Color, light.Position, light.Direction, light.AttenuationLinear, light.AttenuationQuadratic, light.Angle, light.Type)
	)";

	unsigned CompileShader(unsigned program, unsigned type, std::string_view _header, std::string_view body)
	{
		unsigned shader = glCreateShader(type);
		defer { glDeleteShader(shader); };

		const char* sources[] = { _header.data(), body.data() };
		const int lengths[] = { static_cast<int>(_header.size()), static_cast<int>(body.size()) };

		glShaderSource(shader, 2, sources, lengths);
		glCompileShader(shader);

		GLint success = GL_FALSE;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			ASSERT(infoLen > 0, "Could not retrieve shader compilation log.");

			char* infoLog = static_cast<char*>(malloc(sizeof(char) * infoLen));
			defer { free(infoLog); };

			glGetShaderInfoLog(shader, sizeof(char) * infoLen, nullptr, infoLog);

			// Avoid duplicate newline characters.
			if (infoLen >= 2 && (infoLog[infoLen - 2] == '\n'))
			{
				infoLog[infoLen - 2] = '\0';
			}

			gem::Error(infoLog);

			return GL_NONE;
		}

		glAttachShader(program, shader);
		return shader;
	}

	bool LinkProgram(unsigned program)
	{
		GLint success = GL_FALSE;
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &success);

		// Output GL error to log on failure.
		if (success == GL_FALSE)
		{
			GLint infoLen = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
			ASSERT(infoLen > 0, "Could not retrieve program compilation log.");

			char* infoLog = static_cast<char*>(malloc(sizeof(char) * infoLen));
			defer { free(infoLog); };

			glGetProgramInfoLog(program, sizeof(char) * infoLen, nullptr, infoLog);

			// Avoid duplicate newline characters.
			if (infoLog[infoLen - 2] == '\n')
			{
				infoLog[infoLen - 2] = '\0';
			}

			gem::Error(infoLog);

			return false;
		}

		return true;
	}
}

namespace gem
{
	void ShaderVariantControl::Define(std::string_view name, std::string_view value)
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
		defines.emplace_back(std::string(name), std::string(value));
	}

	void ShaderVariantControl::Switch(std::string_view name, bool state)
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

	void ShaderVariantControl::Toggle(std::string_view name)
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

		defines.emplace_back(std::string(name), "");
	}

	bool ShaderVariantControl::IsDefined(std::string_view name) const
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

	void ShaderVariantControl::Undefine(std::string_view name)
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

	std::size_t ShaderVariantControl::GetHash() const
	{
		return hash;
	}

	bool ShaderVariantControl::operator==(const ShaderVariantControl& other) const
	{
		return hash == other.hash;
	}

	bool ShaderVariantControl::operator!=(const ShaderVariantControl& other) const
	{
		return hash != other.hash;
	}

	void ShaderVariantControl::UpdateHash()
	{
		std::hash<std::string> str_hash;
		hash = 0x0;

		for (auto& define : defines)
		{
			hash += str_hash(define.name) + str_hash(define.value);
		}
	}

	//-----------------------------------------------------------------------------------------------------

	std::string Shader::commonHeader;

	Shader::~Shader()
	{
		Unload();
	}

	bool Shader::Load(std::string_view filePath)
	{
		ASSERT(!IsLoaded(), "ShaderData already has a Shader loaded.");

		std::string source;
		if (!LoadFileAsString(filePath, source))
		{
			Error("Shader: ( %s )\nUnable to open file.", filePath.data());
			return false;
		}

		if (!LoadInternal(source))
		{
			Error("Shader: ( %s )", filePath.data());
			return false;
		}

		return true;
	}

	bool Shader::LoadFromSource(std::string_view source)
	{
		ASSERT(!IsLoaded(), "ShaderData already has a Shader loaded.");

		if (!LoadInternal(std::string(source)))
		{
			Error("Shader: ( From Source ).");
			return false;
		}

		return true;
	}

	bool Shader::LoadPassThrough()
	{
		ASSERT(!IsLoaded(), "ShaderData already has a Shader loaded.");

		if (!LoadInternal(std::string(passThroughProgram)))
		{
			Error("Shader: ( PassThrough ).");
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

				// Save the starting position after the '{'. It is likely there is also a
				// newline after the brace, so we skip it for simplicity and line tracking (#line).
				++pos;
				if (pos < source.size() && std::isspace(source[pos]))
				{
					++pos;
				}
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

					++pos;
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
					if (memchr(block.source.data() + block.start, '#', block.end - block.start) != nullptr)
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
					if (memchr(block.source.data() + block.start, '#', block.end - block.start) != nullptr)
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
					if (memchr(block.source.data() + block.start, '#', block.end - block.start) != nullptr)
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
			++pos;
		}

		// We must have at least one shader.
		if (vertexSource.empty() && geometrySource.empty() && fragmentSource.empty())
		{
			Error("Must specify at least one shader.");
			return false;
		}

		// If a shader block was left empty, we substitute a pass-through.
		if (vertexSource.empty())
		{
			vertexSource = passThroughVertex;
		}
		else if (fragmentSource.empty())
		{
			fragmentSource = passThroughFragment;
		}

		loaded = true;
		return true;
	}

	bool Shader::ParseAttributes(const Block& block)
	{
		ASSERT(block.type == BlockType::Attributes, "Expected an Attribute block type.");

		size_t pos = block.start;

		char type[16] = {};
		char name[128] = {};
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

				attributes += FormatString("layout(location = %u) in %s %s;\n", Id, type, name);

				// Jump past the line we just read.
				pos = block.source.find(';', pos);
			}

			pos++;
		}

		return true;
	}

	bool Shader::ParseShader(const Block& block)
	{
		std::string* output = nullptr;
		switch (block.type)
		{
		case BlockType::Vertex:
			output = &vertexSource;
			break;
		case BlockType::Geometry:
			output = &geometrySource;
			break;
		case BlockType::Fragment:
			output = &fragmentSource;
			break;
		default:
			ASSERT(false, "Expected a Shader block type.");
		}

		// The first keyword in the block might be an external reference.
		size_t pos = block.start;
		char includePath[256] = {};
		while (pos < block.end)
		{
			if (!std::isspace(block.source[pos]))
			{
				if (sscanf(&block.source[pos], "#include \"%255s\"", includePath) == 1)
				{
					// Remove invalid characters.
					while (char* chr = strchr(includePath, '\"'))
					{
						*chr = '\0';
					}

					if (IsPathRelative(includePath))
					{
						std::string fullPath = ExtractPath(GetPath());
						fullPath += '/';
						fullPath += includePath;
						if (!LoadFileAsString(fullPath, *output))
						{
							Error("Shader include ( %s ) failed to load.", fullPath.c_str());
							return false;
						}
					}
					else
					{
						if (!LoadFileAsString(includePath, *output))
						{
							Error("Shader include ( %s ) failed to load.", includePath);
							return false;
						}
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

		output->insert(0, "#line 1\n");

		return true;
	}

	bool Shader::ParseUniforms(const Block& block)
	{
		ASSERT(block.type == BlockType::Uniforms, "Expected a Uniform block type.");

		size_t pos = block.start;

		char modifer[32] = {};
		char name[128] = {};
		unsigned Id = 0;

		while (pos < block.end)
		{
			if (!std::isspace(block.source[pos]))
			{
				bool isStatic = false;
				bool isInstance = false;

				if (sscanf(&block.source[pos], "%31s %127s : %u;", modifer, name, &Id) == 3)
				{
					if (strcmp(modifer, "static") == 0)
					{
						isStatic = true;
					}
					else if (strcmp(modifer, "instance") == 0)
					{
						isInstance = true;
					}
					else
					{
						Error("Unrecognized modifier keyword on Uniform buffer.");
						return false;
					}
				}
				else if (sscanf(&block.source[pos], "%127s : %u;", name, &Id) != 2)
				{
					Error("Failed to parse Uniform buffer.");
					return false;
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

				if (!ParseUniformBlock(buffer, name, Id, isInstance, isStatic))
				{
					return false;
				}
			}

			pos++;
		}

		return true;
	}

	bool Shader::ParseUniformBlock(const Block& block, std::string_view name, unsigned Id, bool isInstance, bool isStatic)
	{
		ASSERT(block.type == BlockType::Uniforms, "Expected a Uniform block type.");
		ASSERT(!name.empty(), "Must provide a name.");

		// The struct with all default assignments intact.
		std::string rawStruct(block.source.substr(block.start, block.end - block.start));

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

		uniformStruct = FormatString("layout(std140) uniform Gem_User_%s\n{%s} %s;\n", name.data(), uniformStruct.c_str(), name.data());

		/* Create template and the shader owned buffer */
		if (isInstance || isStatic)
		{
			auto buffer = UniformBuffer::MakeNew();

			struct UniformMember
			{
				std::string name;
				std::string value;
				GLenum type = GL_NONE;
			};

			std::vector<UniformMember> uniforms;

			// Find all uniforms and parse their information.
			for (size_t i = 0; i < rawStruct.size(); ++i)
			{
				if (std::isspace(rawStruct[i]))
					continue;

				UniformMember& uniform = uniforms.emplace_back();

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

				if (line.starts_with("float"))       uniform.type = GL_FLOAT;
				else if (line.starts_with("vec2"))   uniform.type = GL_FLOAT_VEC2;
				else if (line.starts_with("vec3"))   uniform.type = GL_FLOAT_VEC3;
				else if (line.starts_with("vec4"))   uniform.type = GL_FLOAT_VEC4;
				else if (line.starts_with("mat4"))   uniform.type = GL_FLOAT_MAT4;
				else if (line.starts_with("mat3"))   uniform.type = GL_FLOAT_MAT3;
				else if (line.starts_with("mat2"))   uniform.type = GL_FLOAT_MAT2;
				else if (line.starts_with("int"))    uniform.type = GL_INT;
				else if (line.starts_with("uint"))   uniform.type = GL_UNSIGNED_INT;
				else if (line.starts_with("bool"))   uniform.type = GL_BOOL;
				else if (line.starts_with("double")) uniform.type = GL_DOUBLE;
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
				case GL_FLOAT:        buffer->AddUniform<float>(uniform.name); break;
				case GL_FLOAT_VEC2:   buffer->AddUniform<vec2>(uniform.name); break;
				case GL_FLOAT_VEC3:   buffer->AddUniform<vec3>(uniform.name); break;
				case GL_FLOAT_VEC4:   buffer->AddUniform<vec4>(uniform.name); break;
				case GL_FLOAT_MAT4:   buffer->AddUniform<mat4>(uniform.name); break;
				case GL_FLOAT_MAT3:   buffer->AddUniform<mat3>(uniform.name); break;
				case GL_FLOAT_MAT2:   buffer->AddUniform<mat2>(uniform.name); break;
				case GL_INT:          buffer->AddUniform<int>(uniform.name); break;
				case GL_UNSIGNED_INT: buffer->AddUniform<unsigned>(uniform.name); break;
				case GL_BOOL:         buffer->AddUniform<bool>(uniform.name); break;
				case GL_DOUBLE:       buffer->AddUniform<double>(uniform.name); break;
				}
			}
			buffer->InitBuffer();

			/* Parse default values */
			for (auto& uniform : uniforms)
			{
				if (uniform.value.empty())
				{
					switch (uniform.type)
					{
					case GL_FLOAT_MAT4: buffer->SetUniform(uniform.name, mat4()); break;
					case GL_FLOAT_MAT3: buffer->SetUniform(uniform.name, mat3()); break;
					case GL_FLOAT_MAT2: buffer->SetUniform(uniform.name, mat2()); break;
					}
					continue;
				}

				ScopeGuard guard = [&]() {
					Error("Default value assignment of ( %s ) could not be parsed.", uniform.name.c_str());
				};

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
					if (CompareLowercase(uniform.value, "true"))
					{
						u = 1;
					}
					else if (CompareLowercase(uniform.value, "false"))
					{
						u = 0;
					}
					else if (sscanf(uniform.value.c_str(), "%u", &u) != 1)
					{
						return false;
					}

					buffer->SetUniform(uniform.name, !!u);
					break;
				}

				guard.Dismiss();
			}

			if (isStatic)
			{
				buffers.Add(std::move(buffer), Id);
			}

			bufferBindings.emplace_back(std::string(name), Id, isStatic ? nullptr : std::move(buffer));
		}
		else
		{
			if (rawStruct.find('=') != std::string::npos)
			{
				Error("Default uniform buffer member assignment is only available on buffers that are marked as 'instance' or 'static'.");
				return false;
			}

			bufferBindings.emplace_back(std::string(name), Id, nullptr);
		}

		uniformBuffers += uniformStruct;

		return true;
	}

	bool Shader::ParseSamplers(const Block& block)
	{
		ASSERT(block.type == BlockType::Samplers, "Expected a Sampler block type.");

		size_t pos = block.start;

		char type[32] = {};
		char name[128] = {};
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
				samplers += FormatString("uniform %s %s;\n", type, name);

				// Jump past the line we just read.
				pos = block.source.find(';', pos);
			}

			pos++;
		}

		return true;
	}

	void Shader::BuildCommonHeader()
	{
		int major = 0;
		int minor = 0;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);

		// Our minimum supported version is 3.3, where the format of the GLSL
		// version identifier begins to be symmetrical with the GL version.
		commonHeader = "#version " + std::to_string(major) + std::to_string(minor) + "0\n" + std::string(header);
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

		variants.clear();

		textureBindings.clear();
		bufferBindings.clear();

		attributes.clear();
		samplers.clear();
		uniformBuffers.clear();
		vertexSource.clear();
		geometrySource.clear();
		fragmentSource.clear();
	}

	void Shader::Bind()
	{
		Bind(ShaderVariantControl());
	}

	void Shader::Bind(const ShaderVariantControl& definitions)
	{
		ASSERT(IsLoaded(), "Must have a shader loaded to call this function.");

		auto itr = variants.find(definitions);
		if (itr == variants.end()) [[unlikely]]
		{
			// This shader variant is new and needs to be created.
			auto& variant = variants.emplace(definitions, ShaderVariant()).first->second;

			const std::string defines = definitions.GetString();

			if (!variant.Load(
				commonHeader + uniformBuffers + samplers + defines,
				attributes + vertexSource,
				geometrySource,
				fragmentSource))
			{
				if (!defines.empty())
				{
					Error("With variant definitions:\n%s", defines.c_str());
				}

				// Instead of doing nothing, we load a hard-coded pink shader on failure.
				if (!variant.Load(commonHeader, fallbackVertex, "", fallbackFragment))
				{
					ASSERT(false, "Fallback pink-shader failed to compile.");
				}
			}
			else
			{
				// Make sure the samplers are all set to the correct bindings.
				for (const auto& binding : textureBindings)
				{
					unsigned location = glGetUniformLocation(variant.program, binding.name.c_str());
					// Not finding a location is not an error.
					// It is most likely because a uniform has been optimized away.
					if (location != GL_INVALID_INDEX)
					{
						glProgramUniform1i(variant.program, location, binding.unit);
					}
				}

				// Make sure the UniformBuffers are all set to the correct bindings.
				for (const auto& binding : bufferBindings)
				{
					unsigned block = glGetUniformBlockIndex(variant.program, ("Gem_User_" + binding.name).c_str());
					if (block != GL_INVALID_INDEX)
					{
						glUniformBlockBinding(variant.program, block, binding.unit);
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

	std::span<const TextureBinding> Shader::GetTextureBindings() const
	{
		return textureBindings;
	}

	std::span<const BufferBinding> Shader::GetBufferBindings() const
	{
		return bufferBindings;
	}

	//-----------------------------------------------------------------------------------------------------

	Shader::ShaderVariant::~ShaderVariant()
	{
		Unload();
	}

	bool Shader::ShaderVariant::Load(std::string_view _header, std::string_view vertSource, std::string_view geomSource, std::string_view fragSource)
	{
		program = glCreateProgram();
		unsigned vertShader = GL_NONE;
		unsigned geomShader = GL_NONE;
		unsigned fragShader = GL_NONE;

		if (!vertSource.empty())
		{
			vertShader = CompileShader(program, GL_VERTEX_SHADER, _header, vertSource);
			if (vertShader == GL_NONE)
			{
				Error("Shader variant's vertex stage failed to compile.");
				Unload();
				return false;
			}
		}

		if (!geomSource.empty())
		{
			geomShader = CompileShader(program, GL_GEOMETRY_SHADER, _header, geomSource);
			if (geomShader == GL_NONE)
			{
				Error("Shader variant's geometry stage failed to compile.");
				Unload();
				return false;
			}
		}

		if (!fragSource.empty())
		{
			fragShader = CompileShader(program, GL_FRAGMENT_SHADER, _header, fragSource);
			if (fragShader == GL_NONE)
			{
				Error("Shader variant's fragment stage failed to compile.");
				Unload();
				return false;
			}
		}

		if (!LinkProgram(program))
		{
			Error("Shader variant failed to link.");
			Unload();
			return false;
		}

		if (vertShader != GL_NONE) glDetachShader(program, vertShader);
		if (geomShader != GL_NONE) glDetachShader(program, geomShader);
		if (fragShader != GL_NONE) glDetachShader(program, fragShader);

		/* Initialize built-in uniform blocks */
		unsigned cameraBlock = glGetUniformBlockIndex(program, "Gem_Camera_Uniforms");
		unsigned modelBlock  = glGetUniformBlockIndex(program, "Gem_Model_Uniforms");
		unsigned engineBlock = glGetUniformBlockIndex(program, "Gem_Engine_Uniforms");
		unsigned timeBlock   = glGetUniformBlockIndex(program, "Gem_Time_Uniforms");

		if (cameraBlock != GL_INVALID_INDEX) glUniformBlockBinding(program, cameraBlock, (GLuint)UniformBufferSlot::Camera);
		if (modelBlock  != GL_INVALID_INDEX) glUniformBlockBinding(program, modelBlock,  (GLuint)UniformBufferSlot::Model);
		if (engineBlock != GL_INVALID_INDEX) glUniformBlockBinding(program, engineBlock, (GLuint)UniformBufferSlot::Engine);
		if (timeBlock   != GL_INVALID_INDEX) glUniformBlockBinding(program, timeBlock,   (GLuint)UniformBufferSlot::Time);

		return true;
	}

	void Shader::ShaderVariant::Unload()
	{
		if (program != GL_NONE)
		{
			glDeleteProgram(program);
			program = GL_NONE;
		}
	}

	void Shader::ShaderVariant::Bind() const
	{
		ASSERT(program != GL_NONE, "ShaderVariant cannot be bound because it is not loaded.");

		glUseProgram(program);
	}
}

REFLECT_RESOURCE(gem::Shader) REF_END;
