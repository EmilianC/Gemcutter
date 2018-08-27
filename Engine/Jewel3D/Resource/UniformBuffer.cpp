// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "UniformBuffer.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Math/Vector.h"

#include <algorithm>
#include <GLEW/GL/glew.h>

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

	void UniformBuffer::InitBuffer()
	{
		ASSERT(bufferSize != 0, "At least one uniform must be added to the buffer before it can be initialized.");
		ASSERT(UBO == GL_NONE, "UniformBuffer has already been initialized and locked.");

		// The total size of a buffer is always aligned to the size of a vec4.
		bufferSize = round(bufferSize, sizeof(vec4));

		// GPU buffer.
		glGenBuffers(1, &UBO);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferData(GL_UNIFORM_BUFFER, bufferSize, nullptr, GL_STREAM_DRAW);
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

	bool UniformBuffer::IsUniform(const std::string& name) const
	{
		return table.find(name) != table.end();
	}

	void UniformBuffer::AddUniform(const std::string& name, unsigned bytes, unsigned alignment, unsigned count)
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

			table[name] = bufferSize;
			bufferSize += bytes;
		}
		else
		{
			// An array must start on a vec4 boundary.
			bufferSize = round(bufferSize, sizeof(vec4));

			// The size of an individual element is aligned to the nearest vec4.
			bytes = round(bytes, sizeof(vec4));

			table[name] = bufferSize;
			bufferSize += bytes * count;

			// There must be padding after an array to the next vec4.
			bufferSize = round(bufferSize, sizeof(vec4));
		}
	}

	void* UniformBuffer::GetBufferLoc(const std::string& name) const
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

	void BufferList::Add(UniformBuffer::Ptr buffer, unsigned unit)
	{
		ASSERT(buffer, "'buffer' cannot be null.");

		Remove(unit);
		buffers.emplace_back(std::move(buffer), unit);
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
	void UniformHandle<mat3>::Set(const mat3& value)
	{
		ASSERT(uniformBuffer, "Uniform handle is not associated with a UniformBuffer.");
		ASSERT(uniformBuffer->buffer, "The associated UniformBuffer has not been initialized yet.");

		mat4* ptr = reinterpret_cast<mat4*>(static_cast<char*>(uniformBuffer->buffer) + offset);
		*ptr = mat4(value);

		uniformBuffer->dirty = true;
	}

	template<>
	mat3 UniformHandle<mat3>::Get() const
	{
		ASSERT(uniformBuffer, "Uniform handle is not associated with a UniformBuffer.");
		ASSERT(uniformBuffer->buffer, "The associated UniformBuffer has not been initialized yet.");

		return mat3(*reinterpret_cast<mat4*>(static_cast<char*>(uniformBuffer->buffer) + offset));
	}
}
