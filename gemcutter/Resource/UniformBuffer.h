// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Math/Matrix.h"
#include "gemcutter/Rendering/Rendering.h"
#include "gemcutter/Resource/Shareable.h"
#include "gemcutter/Utilities/Container.h"

#include <string_view>
#include <string>
#include <unordered_map>
#include <vector>

namespace Jwl
{
	// A collection of parameters that are used by a shader.
	// To be used by a shader, an instance must either be attached to the
	// shader directly, or to a Material component.
	class UniformBuffer : public Shareable<UniformBuffer>
	{
		template<class T> friend class UniformHandle;
	public:
		UniformBuffer() = default;
		~UniformBuffer();

		UniformBuffer& operator=(const UniformBuffer&);
		void Copy(const UniformBuffer& other);

		// During the initialization phase, appends a new variable to the buffer.
		// Returns a handle to the newly added uniform.
		template<class T>
		UniformHandle<T> AddUniform(std::string_view name, unsigned count = 1);

		// Once this is called, no more uniforms can be added and the buffer is ready for use.
		void InitBuffer(VertexBufferUsage usage = VertexBufferUsage::Static);
		void UnLoad();

		void Bind(unsigned slot) const;
		static void UnBind(unsigned slot);

		// Once initialized, this sets the value of a uniform.
		// * If used regularly, consider caching a UniformHandle instead. It will be faster *
		template<class T>
		void SetUniform(std::string_view name, const T& data);
		template<class T>
		void SetUniformArray(std::string_view name, unsigned numElements, T* data);

		// Returns a handle that can be used to directly modify the value of a uniform.
		// This is more performant then SetUniform() or SetUniformArray().
		template<class T>
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
		BufferSlot() = default;
		BufferSlot(UniformBuffer::Ptr buffer, unsigned unit);

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
		// Removes all Buffers.
		void Clear();

		// Returns the buffer bound at the specified unit.
		UniformBuffer::Ptr& operator[](unsigned unit);

		const auto& GetAll() const { return buffers; }

	private:
		std::vector<BufferSlot> buffers;
	};

	// Provides direct and performant access to a uniform member value.
	// * No type checking is done. It is your responsibility to ensure the type is correct *
	// * Usage is unsafe after the source uniform buffer is deleted *
	template<class T>
	class UniformHandle
	{
		static_assert(std::is_standard_layout_v<T>, "Uniforms cannot be complex types.");
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
