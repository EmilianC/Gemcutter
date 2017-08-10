// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Resource.h"
#include "Texture.h"
#include "UniformBuffer.h"

#include <unordered_map>
#include <vector>

namespace Jwl
{
	// Manages a set of defines used to control shaders.
	class ShaderVariantControl
	{
	public:
		// Adds a new define, or updates its value.
		template<typename T>
		void Define(const std::string& name, T value);
		void Define(const std::string& name);

		// Either Define()'s or Undefine()'s the property, based on state.
		void Switch(const std::string& name, bool state);

		// Defines the property if it does not already exist. If it does, it is undefined.
		void Toggle(const std::string& name);

		// Returns true if 'name' is defined.
		bool IsDefined(const std::string& name) const;

		// Returns true if this object contains no defines.
		bool IsEmpty() const;

		// Removes a define.
		void Undefine(const std::string& name);

		// Clears all defined values.
		void Reset();

		// Returns the complete list of Defines.
		std::string GetString() const;

		// Returns a hash value generated from all internal defines.
		unsigned GetHash() const;

		bool operator==(const ShaderVariantControl&) const;
		bool operator!=(const ShaderVariantControl&) const;

	private:
		void UpdateHash();

		std::vector<std::string> defines;
		unsigned hash = 0;
	};
}

namespace std
{
	// Custom hash functor for the variant control.
	template<>
	struct hash<Jwl::ShaderVariantControl>
	{
		size_t operator()(const Jwl::ShaderVariantControl& svc) const
		{
			return svc.GetHash();
		}
	};
}

namespace Jwl
{
	// Used internally to expose Uniform buffers from the shader to BufferSlots.
	struct BufferBinding
	{
		BufferBinding(const std::string& name, unsigned unit, UniformBuffer::Ptr buff)
			: name(name), unit(unit), templateBuff(buff) {}

		// The name of the uniform buffer in the shader.
		const std::string name;
		// The unit connecting this buffer to a BufferSlot of the same unit.
		const unsigned unit;
		// Holds a template buffer that can be duplicated.
		// Created only for buffers that are defined with the "template" keyword in the shader.
		const UniformBuffer::Ptr templateBuff;
	};

	// Used internally to expose Samplers from the shader to TextureSlots.
	struct TextureBinding
	{
		TextureBinding(const std::string& name, unsigned unit)
			: name(name), unit(unit) {}

		// The name of the sampler in the shader.
		const std::string name;
		// The unit connecting this texture sampler to a TextureSlot of the same unit.
		const unsigned unit;
	};

	// Manages a shader and all its variants.
	// For global use, Textures and UniformBuffers can be attached directly to the shader.
	class Shader : public Resource<Shader>
	{
	public:
		Shader() = default;
		Shader(const Shader&) = delete;
		~Shader();

		bool Load(std::string filePath);
		bool LoadFromSource(const std::string& source);
		bool LoadPassThrough();
		static Shader::Ptr MakeNewPassThrough();
		
		// Unloads all GPU-side memory and cleans the object.
		void Unload();

		void Bind();
		// Binds the shader compiled with the provided variant definitions.
		void Bind(const ShaderVariantControl& definitions);
		void UnBind();

		bool IsLoaded() const;

		// These textures will be bound whenever the shader is used in rendering.
		TextureList textures;
		// These buffers will be bound whenever the shader is used in rendering.
		BufferList buffers;

		// Creates a new UniformBuffer from the specified buffer slot.
		// The slot must be marked as 'template' in the shader's source.
		UniformBuffer::Ptr CreateBufferFromTemplate(unsigned unit) const;

		const std::vector<BufferBinding>& GetBufferBindings() const;

	private:
		// Holds one compiled shader variation.
		struct ShaderVariant
		{
			~ShaderVariant();

			bool Load(const std::string& header, const std::string& vertSource, const std::string& geomSource, const std::string& fragSource);
			void Unload();
			void Bind() const;

			unsigned hProgram	 = 0;
			unsigned hVertShader = 0;
			unsigned hFragShader = 0;
			unsigned hGeomShader = 0;
		};

		// Used to identify the content of a Program block.
		enum class BlockType : unsigned
		{
			Attributes	= 1,
			Uniforms	= 2,
			Samplers	= 4,
			Vertex		= 8,
			Geometry	= 16,
			Fragment	= 32
		};

		// Describes a Program block found in the shader file.
		struct Block
		{
			Block(const std::string& str) : source(str) {}

			BlockType type;
			const std::string& source;
			size_t start = 0;
			size_t end = 0;
		};

		// Finds the relevant sections of the shader code and distributes them to the Parse* functions.
		bool LoadInternal(std::string source);
		bool ParseAttributes(const Block& block);
		bool ParseShader(const Block& block);
		bool ParseUniforms(const Block& block);
		bool ParseUniformBlock(const Block& block, const char* name, unsigned Id, bool makeTemplate, bool makeStatic);
		bool ParseSamplers(const Block& block);

		bool loaded = false;

		std::unordered_map<ShaderVariantControl, ShaderVariant> variants;

		std::vector<TextureBinding> textureBindings;
		std::vector<BufferBinding> bufferBindings;

		// Various shader source code snippets.
		static std::string version;
		static std::string header;
		std::string attributes;
		std::string samplers;
		std::string uniformBuffers;
		std::string vertexSource;
		std::string geometrySource;
		std::string fragmentSrouce;
	};

	template<typename T>
	void ShaderVariantControl::Define(const std::string& name, T value)
	{
		std::string entry = name + ' ' + std::to_string(value);

		// Search for and update the define if it already exists.
		for (auto& define : defines)
		{
			if (define == name)
			{
				define = entry;
				UpdateHash();

				return;
			}
		}
		
		// Add as a new define.
		defines.push_back(entry);
		UpdateHash();
	}
}
