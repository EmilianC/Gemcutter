// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Shareable.h"
#include "Jewel3D/Application/Logging.h"

#include <unordered_map>
#include <vector>

namespace Jwl
{
	// A collection of parameters that are used by a shader.
	// To be used by a shader, an instance must either be attached to the 
	// shader directly, or to a Material component.
	class UniformBuffer : public Shareable<UniformBuffer>
	{
		template<class T>
		friend class UniformHandle;
	public:
		UniformBuffer() = default;
		~UniformBuffer();

		UniformBuffer& operator=(const UniformBuffer&);
		void Copy(const UniformBuffer& other);

		// During the initialization phase, appends a new variable to the buffer.
		void AddUniform(const std::string& name, unsigned bytes, unsigned count = 1);

		// Once this is called, no more uniforms can be added and the buffer is ready for use.
		void InitBuffer();
		void UnLoad();

		void Bind(unsigned slot) const;
		static void UnBind(unsigned slot);

		// Once initialized, this sets the value of a uniform.
		// * If used regularly, consider caching a UniformHandle instead. It will be faster *
		template<class T>
		void SetUniform(const std::string& name, const T& data);
		template<class T>
		void SetUniformArray(const std::string& name, unsigned numElements, T* data);

		// Returns a handle that can be used to directly modify the value of a uniform.
		// This is more performant then SetUniform() or SetUniformArray().
		template<class T>
		UniformHandle<T> MakeHandle(const std::string& name);

		int GetByteSize() const;
		bool IsUniform(const std::string& name) const;

	private:
		void* GetBufferLoc(const std::string& name) const;

		mutable bool dirty	= true;
		unsigned UBO		= 0;
		void* buffer		= nullptr;
		unsigned bufferSize = 0;

		std::unordered_map<std::string, int> table;
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

		void Add(UniformBuffer::Ptr buffer, unsigned unit);
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
	public:
		UniformHandle<T>() = default;
		UniformHandle<T>(UniformBuffer& buff, T& data);

		// Set the value of the uniform.
		UniformHandle<T>& operator=(const T& value);

		// Set the value of the uniform.
		void Set(const T& value);
		// Read the current value of the uniform.
		const T& Get() const;

		// Gets a pointer to the associated UniformBuffer.
		UniformBuffer* GetBuffer() const;

	private:
		UniformBuffer* buffer = nullptr;
		T* data;
	};
}

#include "UniformBuffer.inl"
