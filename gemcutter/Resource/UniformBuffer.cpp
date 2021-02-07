// Copyright (c) 2017 Emilian Cioca
#include "UniformBuffer.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Math/Vector.h"

#include <algorithm>
#include <glew/glew.h>

namespace
{
	// Rounds val up to the nearest multiple.
	unsigned round(unsigned val, unsigned multiple)
	{
		unsigned remainder = val % multiple;
		if (remainder != 0)
		{
			val += multiple - remainder;
		}

		return val;
	}
}

namespace Jwl
{
	UniformBuffer::~UniformBuffer()
	{
		UnLoad();
	}

	UniformBuffer& UniformBuffer::operator=(const UniformBuffer& other)
	{
		UnLoad();

		table = other.table;
		bufferSize = other.bufferSize;

		if (bufferSize != 0)
		{
			// We have to create our own UBO.
			InitBuffer();

			memcpy(buffer, other.buffer, bufferSize);
		}

		return *this;
	}

	void UniformBuffer::Copy(const UniformBuffer& other)
	{
		*this = other;
	}

	void UniformBuffer::InitBuffer(VertexBufferUsage usage)
	{
		ASSERT(bufferSize != 0, "At least one uniform must be added to the buffer before it can be initialized.");
		ASSERT(UBO == GL_NONE, "UniformBuffer has already been initialized and locked.");

		// The total size of a buffer is always aligned to the size of a vec4.
		bufferSize = round(bufferSize, sizeof(vec4));

		// GPU buffer.
		glGenBuffers(1, &UBO);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferData(GL_UNIFORM_BUFFER, bufferSize, nullptr, ResolveVertexBufferUsage(usage));
		glBindBuffer(GL_UNIFORM_BUFFER, GL_NONE);

		// RAM buffer.
		buffer = calloc(1, bufferSize);
	}

	void UniformBuffer::UnLoad()
	{
		glDeleteBuffers(1, &UBO);
		UBO = GL_NONE;

		free(buffer);
		buffer = nullptr;

		table.clear();
		bufferSize = 0;
		dirty = true;
	}

	void UniformBuffer::Bind(unsigned slot) const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, slot, UBO);

		if (dirty)
		{
			glBufferSubData(GL_UNIFORM_BUFFER, 0, bufferSize, buffer);
			dirty = false;
		}
	}

	void UniformBuffer::UnBind(unsigned slot)
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, slot, GL_NONE);
	}

	int UniformBuffer::GetByteSize() const
	{
		return bufferSize;
	}

	bool UniformBuffer::IsUniform(std::string_view name) const
	{
		return table.contains(name);
	}

	void UniformBuffer::AddUniform(std::string name, unsigned bytes, unsigned alignment, unsigned count)
	{
		ASSERT(UBO == GL_NONE, "UniformBuffer has already been initialized and locked.");
		ASSERT(count != 0, "Must add at least one element.");
		ASSERT(!IsUniform(name), "A member with the same name has already been added.");

		// Since we are using std140 layout, we must follow the rules in the OpenGL specification:
		// https://www.opengl.org/registry/doc/glspec45.core.pdf#page=159
		if (count == 1)
		{
			// Start of a single element is aligned to its own size.
			bufferSize = round(bufferSize, alignment);

			table.emplace(std::move(name), bufferSize);
			bufferSize += bytes;
		}
		else
		{
			// An array must start on a vec4 boundary.
			bufferSize = round(bufferSize, sizeof(vec4));

			// The size of an individual element is aligned to the nearest vec4.
			bytes = round(bytes, sizeof(vec4));

			table.emplace(std::move(name), bufferSize);
			bufferSize += bytes * count;

			// There must be padding after an array to the next vec4.
			bufferSize = round(bufferSize, sizeof(vec4));
		}
	}

	void* UniformBuffer::GetBufferLoc(std::string_view name) const
	{
		auto loc = table.find(name);
		if (loc != table.end())
		{
			return static_cast<char*>(buffer) + loc->second;
		}
		else
		{
			return nullptr;
		}
	}

	template<>
	void UniformBuffer::SetUniform<mat2>(std::string_view name, const mat2& data)
	{
		vec4* dest = static_cast<vec4*>(GetBufferLoc(name));
		ASSERT(dest, "Could not find uniform parameter ( %s ).", name.data());
		ASSERT(reinterpret_cast<char*>(dest) + sizeof(vec4) * 2 <= static_cast<char*>(buffer) + bufferSize,
			"Setting uniform ( %s ) out of bounds of the buffer.", name.data());

		dest[0].x = data[0];
		dest[0].y = data[1];
		dest[1].x = data[2];
		dest[1].y = data[3];
		dirty = true;
	}

	template<>
	void UniformBuffer::SetUniform<mat3>(std::string_view name, const mat3& data)
	{
		vec4* dest = static_cast<vec4*>(GetBufferLoc(name));
		ASSERT(dest, "Could not find uniform parameter ( %s ).", name.data());
		ASSERT(reinterpret_cast<char*>(dest) + sizeof(vec4) * 3 <= static_cast<char*>(buffer) + bufferSize,
			"Setting uniform ( %s ) out of bounds of the buffer.", name.data());

		dest[0].x = data[0];
		dest[0].y = data[1];
		dest[0].z = data[2];

		dest[1].x = data[3];
		dest[1].y = data[4];
		dest[1].z = data[5];

		dest[2].x = data[6];
		dest[2].y = data[7];
		dest[2].z = data[8];

		dirty = true;
	}

	//-----------------------------------------------------------------------------------------------------

	BufferSlot::BufferSlot(UniformBuffer::Ptr buffer, unsigned unit)
		: buffer(std::move(buffer)), unit(unit)
	{
	}

	void BufferSlot::Bind() const
	{
		buffer->Bind(unit);
	}

	void BufferSlot::UnBind() const
	{
		UniformBuffer::UnBind(unit);
	}

	//-----------------------------------------------------------------------------------------------------

	void BufferList::Bind() const
	{
		for (auto& slot : buffers)
		{
			slot.Bind();
		}
	}

	void BufferList::UnBind() const
	{
		for (auto& slot : buffers)
		{
			slot.UnBind();
		}
	}

	void BufferList::Add(UniformBuffer::Ptr buff, unsigned unit)
	{
		ASSERT(buff, "'buff' cannot be null.");

		for (auto& slot : buffers)
		{
			if (slot.unit == unit)
			{
				slot.buffer = std::move(buff);
				return;
			}
		}

		buffers.emplace_back(std::move(buff), unit);
	}

	void BufferList::Remove(unsigned unit)
	{
		for (auto itr = buffers.begin(); itr < buffers.end(); ++itr)
		{
			if (itr->unit == unit)
			{
				buffers.erase(itr);
				return;
			}
		}
	}

	void BufferList::Clear()
	{
		buffers.clear();
	}

	UniformBuffer::Ptr& BufferList::operator[](unsigned unit)
	{
		auto bufferSlot = std::find_if(buffers.begin(), buffers.end(), [unit](BufferSlot& slot) {
			return slot.unit == unit;
		});

		ASSERT(bufferSlot != buffers.end(), "No matching buffer found.");
		return bufferSlot->buffer;
	}

	template<>
	void UniformHandle<mat2>::Set(const mat2& value)
	{
		ASSERT(uniformBuffer, "Uniform handle is not associated with a UniformBuffer.");
		ASSERT(uniformBuffer->buffer, "The associated UniformBuffer has not been initialized yet.");

		vec4* ptr = reinterpret_cast<vec4*>(static_cast<char*>(uniformBuffer->buffer) + offset);
		ptr[0].x = value[0];
		ptr[0].y = value[1];
		ptr[1].x = value[2];
		ptr[1].y = value[3];

		uniformBuffer->dirty = true;
	}

	template<>
	void UniformHandle<mat3>::Set(const mat3& value)
	{
		ASSERT(uniformBuffer, "Uniform handle is not associated with a UniformBuffer.");
		ASSERT(uniformBuffer->buffer, "The associated UniformBuffer has not been initialized yet.");

		vec4* ptr = reinterpret_cast<vec4*>(static_cast<char*>(uniformBuffer->buffer) + offset);

		ptr[0].x = value[0];
		ptr[0].y = value[1];
		ptr[0].z = value[2];

		ptr[1].x = value[3];
		ptr[1].y = value[4];
		ptr[1].z = value[5];

		ptr[2].x = value[6];
		ptr[2].y = value[7];
		ptr[2].z = value[8];

		uniformBuffer->dirty = true;
	}

	template<>
	mat2 UniformHandle<mat2>::Get() const
	{
		ASSERT(uniformBuffer, "Uniform handle is not associated with a UniformBuffer.");
		ASSERT(uniformBuffer->buffer, "The associated UniformBuffer has not been initialized yet.");

		const vec4* data = reinterpret_cast<vec4*>(static_cast<char*>(uniformBuffer->buffer) + offset);

		return mat2(data[0].x, data[0].y, data[1].x, data[1].y);
	}

	template<>
	mat3 UniformHandle<mat3>::Get() const
	{
		ASSERT(uniformBuffer, "Uniform handle is not associated with a UniformBuffer.");
		ASSERT(uniformBuffer->buffer, "The associated UniformBuffer has not been initialized yet.");

		const mat4* data = reinterpret_cast<mat4*>(static_cast<char*>(uniformBuffer->buffer) + offset);

		return mat3(*data);
	}
}
