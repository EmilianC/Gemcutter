// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Shareable.h"
#include "Jewel3D/Application/Logging.h"

#include <unordered_map>
#include <vector>

namespace Jwl
{
	template<class T> class UniformHandle;

	class UniformBuffer : public Shareable<UniformBuffer>
	{
	public:
		UniformBuffer() = default;
		~UniformBuffer();

		UniformBuffer& operator=(const UniformBuffer&);
		void Copy(const UniformBuffer& other);

		void AddUniform(const std::string& name, u32 bytes, u32 count = 1);

		void InitBuffer();
		void UnLoad();

		void Bind(u32 slot) const;
		static void UnBind(u32 slot);

		template<class T>
		void SetUniform(const std::string& name, const T& data);
		template<class T>
		void SetUniformArray(const std::string& name, u32 numElements, T* data);

		template<class T>
		UniformHandle<T> MakeHandle(const std::string& name);

		s32 GetByteSize();
		bool IsUniform(const std::string& name);

		//- Force the uniform buffer to re-upload it's data the next time it's bound. Used internally.
		void SetDirty();

	private:
		void* GetBufferLoc(const std::string& name) const;

		mutable bool dirty	= true;
		u32 UBO		= 0;
		void* buffer		= nullptr;
		u32 bufferSize = 0;

		std::unordered_map<std::string, s32> table;
	};

	template<class T>
	void UniformBuffer::SetUniform(const std::string& name, const T& data)
	{
		T* dest = reinterpret_cast<T*>(GetBufferLoc(name));
		ASSERT(dest, "Could not find uniform parameter ( %s ).", name.c_str());
		ASSERT(dest + sizeof(T) <= reinterpret_cast<T*>(buffer) + bufferSize, "Setting uniform ( %s ) out of bounds of the buffer.", name.c_str());

		*dest = data;
		dirty = true;
	}

	template<class T>
	void UniformBuffer::SetUniformArray(const std::string& name, u32 numElements, T* data)
	{
		ASSERT(data != nullptr, "Source data cannot be null.");

		void* dest = GetBufferLoc(name);
		ASSERT(dest, "Could not find uniform parameter ( %s ).", name.c_str());
		ASSERT(dest + sizeof(T) * numElements <= reinterpret_cast<char*>(buffer) + bufferSize, "Setting uniform ( %s ) out of bounds of the buffer.", name.c_str());

		memcpy(dest, data, sizeof(T) * numElements);
		dirty = true;
	}

	//- Used to associate a UniformBuffer with a particular binding point.
	struct BufferSlot
	{
		BufferSlot() = default;
		BufferSlot(UniformBuffer::Ptr buffer, u32 unit);

		void Bind() const;
		void UnBind() const;

		UniformBuffer::Ptr buffer;
		u32 unit = 0;
	};

	//- A group of UniformBuffers that are bound and unbound together.
	class BufferList
	{
	public:
		void Bind() const;
		void UnBind() const;

		void Add(UniformBuffer::Ptr buffer, u32 unit);
		void Remove(u32 unit);
		//- Removes all Buffers.
		void Clear();

		//- Returns the buffer bound at the specified unit.
		UniformBuffer::Ptr& operator[](u32 unit);

		const auto& GetAll() const { return buffers; }

	private:
		std::vector<BufferSlot> buffers;
	};

	//- Provides direct and performant access to a uniform member value.
	//- No type checking is done. It is your responsibility to ensure the type is correct.
	template<class T> class UniformHandle
	{
	public:
		UniformHandle<T>() = default;
		UniformHandle<T>(UniformBuffer& buff, T& data)
			: buffer(&buff), data(&data)
		{
		}

		//- Set the value of the uniform. Implicitly dirties the UniformBuffer.
		UniformHandle<T>& operator=(const T& value)
		{
			Set(value);
			return *this;
		}

		//- Set the value of the uniform. Implicitly dirties the UniformBuffer.
		void Set(const T& value)
		{
			ASSERT(data, "Uniform handle is not associated with a UniformBuffer.");

			*data = value;
			buffer->SetDirty();
		}

		//- Read the current value. Asserts if the buffer has been deleted.
		const T& Get() const
		{
			ASSERT(data, "Uniform handle is not associated with a UniformBuffer.");

			return *data;
		}

		//- Gets a pointer to the associated UniformBuffer.
		UniformBuffer* GetBuffer() const
		{
			return buffer;
		}

	private:
		UniformBuffer* buffer = nullptr;
		T* data;
	};

	template<class T>
	UniformHandle<T> UniformBuffer::MakeHandle(const std::string& name)
	{
		T* loc = reinterpret_cast<T*>(GetBufferLoc(name));
		ASSERT(loc, "\"%s\" does not match the name of a Uniform.", name.c_str());

		return UniformHandle<T>(*this, *loc);
	}
}
