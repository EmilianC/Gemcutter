// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Math/Matrix.h"
#include "gemcutter/Rendering/Rendering.h"
#include "gemcutter/Resource/Shareable.h"
#include "gemcutter/Utilities/Meta.h"
#include "gemcutter/Utilities/StdExt.h"

#include <span>
#include <string_view>
#include <string>
#include <unordered_map>
#include <vector>

namespace gem
{
	template<typename T>
	concept shader_uniform = meta::is_any_of_v<T,
		bool, unsigned, int, float, double, vec2, vec3, vec4, mat2, mat3, mat4, quat
	>;

	// A collection of parameters that are used by a shader.
	// To be used by a shader, an instance must either be attached to the
	// shader directly, or to a Material component.
	class UniformBuffer : public Shareable<UniformBuffer>
	{
		template<shader_uniform T> friend class UniformHandle;
	public:
		UniformBuffer() = default;
		~UniformBuffer();

		UniformBuffer& operator=(const UniformBuffer&);
		void Copy(const UniformBuffer& other);

		// During the initialization phase, appends a new variable to the buffer.
		// Returns a handle to the newly added uniform.
		template<shader_uniform T>
		UniformHandle<T> AddUniform(std::string_view name, unsigned count = 1);

		// Once this is called, no more uniforms can be added and the buffer is ready for use.
		void InitBuffer(BufferUsage usage = BufferUsage::Static);
		void UnLoad();

		void Bind(unsigned slot) const;
		static void UnBind(unsigned slot);

		// Once initialized, this sets the value of a uniform.
		// * If used regularly, consider caching a UniformHandle instead. It will be faster *
		template<shader_uniform T>
		void SetUniform(std::string_view name, const T& data);
		template<shader_uniform T>
		void SetUniformArray(std::string_view name, unsigned numElements, T* data);

		// Returns a handle that can be used to directly modify the value of a uniform.
		// This is more performant then SetUniform() or SetUniformArray().
		template<shader_uniform T> [[nodiscard]]
		UniformHandle<T> MakeHandle(std::string_view name);

		int GetByteSize() const;
		bool IsUniform(std::string_view name) const;

	private:
		void AddUniform(std::string name, unsigned bytes, unsigned alignment, unsigned count);
		void* GetBufferLoc(std::string_view name) const;

		mutable bool dirty  = true;
		unsigned UBO        = 0;
		void* buffer        = nullptr;
		unsigned bufferSize = 0;

		std::unordered_map<std::string, unsigned, string_hash, std::equal_to<>> table;
	};

	// Used to associate a UniformBuffer with a particular binding point.
	struct BufferSlot
	{
		void Bind() const;
		void UnBind() const;

		UniformBuffer::Ptr buffer;
		unsigned unit = 0;
	};

	// A group of UniformBuffers that are bound and unbound together.
	class BufferList
	{
	public:
		void Bind() const;
		void UnBind() const;

		void Add(UniformBuffer::Ptr buff, unsigned unit);
		void Remove(unsigned unit);
		void Clear();

		unsigned Size() const;

		// Returns the buffer bound at the specified unit.
		UniformBuffer& operator[](unsigned unit);

		std::span<const BufferSlot> GetAll() const { return buffers; }

	private:
		std::vector<BufferSlot> buffers;
	};

	// Provides direct and performant access to a uniform member value.
	// * No type checking is done. It is your responsibility to ensure the type is correct *
	// * Usage is unsafe after the source uniform buffer is deleted *
	template<shader_uniform T>
	class UniformHandle
	{
	public:
		UniformHandle<T>() = default;
		UniformHandle<T>(UniformBuffer& buff, unsigned offset);

		// Set the value of the uniform.
		UniformHandle<T>& operator=(const T& value);

		// Set the value of the uniform.
		void Set(const T& value);
		// Read the current value of the uniform.
		T Get() const;

	private:
		UniformBuffer* uniformBuffer = nullptr;
		unsigned offset = 0;
	};
}

#include "UniformBuffer.inl"
