// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Resource.h"
#include "Shareable.h"
#include "Texture.h"
#include "UniformBuffer.h"
#include "Jewel3D/Reflection/Reflection.h"

#include <string_view>
#include <unordered_map>
#include <vector>

namespace Jwl
{
	// Manages a set of defines used to control shaders.
	class ShaderVariantControl
	{
		REFLECT_PRIVATE;
	public:
		// Adds a new define, or updates its value.
		void Define(std::string_view name, std::string_view value = "");

		// Either Define()'s or Undefine()'s the property, based on state.
		void Switch(std::string_view name, bool state);

		// Defines the property if it does not already exist. If it does, it is undefined.
		void Toggle(std::string_view name);

		// Returns true if 'name' is defined.
		bool IsDefined(std::string_view name) const;

		// Returns true if this object contains no defines.
		bool IsEmpty() const;

		// Removes a define.
		void Undefine(std::string_view name);

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

		struct Entry
		{
			std::string name;
			std::string value;
		};

		std::vector<Entry> defines;
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
		BufferBinding(std::string name, unsigned unit, UniformBuffer::Ptr buff)
			: name(std::move(name)), unit(unit), templateBuff(std::move(buff)) {}

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
		TextureBinding(std::string name, unsigned unit)
			: name(std::move(name)), unit(unit) {}

		// The name of the sampler in the shader.
		const std::string name;
		// The unit connecting this texture sampler to a TextureSlot of the same unit.
		const unsigned unit;
	};

	// Manages a shader and all its variants.
	// For global use, Textures and UniformBuffers can be attached directly to the shader.
	class Shader : public Resource<Shader>, public Shareable<Shader>
	{
	public:
		Shader() = default;
		Shader(const Shader&) = delete;
		~Shader();

		bool Load(std::string filePath);
		bool LoadFromSource(std::string_view source);
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

			bool Load(std::string_view header, std::string_view vertSource, std::string_view geomSource, std::string_view fragSource);
			void Unload();
			void Bind() const;

			unsigned program = 0;
		};

		// Used to identify the content of a Program block.
		enum class BlockType : unsigned
		{
			None       = 0,
			Attributes = 1,
			Uniforms   = 2,
			Samplers   = 4,
			Vertex     = 8,
			Geometry   = 16,
			Fragment   = 32
		};

		// Describes a Program block found in the shader file.
		struct Block
		{
			Block(std::string_view str) : source(str) {}

			BlockType type = BlockType::None;
			std::string_view source;
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
		static std::string commonHeader;
		std::string attributes;
		std::string samplers;
		std::string uniformBuffers;
		std::string vertexSource;
		std::string geometrySource;
		std::string fragmentSrouce;
	};
}

REFLECT(Jwl::ShaderVariantControl::Entry)<>,
	MEMBERS <
		REF_MEMBER(name)<>,
		REF_MEMBER(value)<>
	>
REF_END;

REFLECT_BASIC(std::vector<Jwl::ShaderVariantControl::Entry>)

REFLECT(Jwl::ShaderVariantControl)<>,
	MEMBERS <
		REF_MEMBER(defines)<>
	>
REF_END;

REFLECT_SHAREABLE(Jwl::Shader)
REFLECT(Jwl::Shader) < Resource >
REF_END;
