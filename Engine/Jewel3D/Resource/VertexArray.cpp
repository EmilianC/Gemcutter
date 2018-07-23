// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "VertexArray.h"
#include "Jewel3D/Application/Logging.h"

#include <GLEW/GL/glew.h>

namespace Jwl
{
	VertexBuffer::VertexBuffer(unsigned _size, VertexBufferUsage _usage)
		: size(_size)
		, usage(_usage)
	{
		ASSERT(size, "A VertexBuffer must have a non-zero size.");

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, ResolveVertexBufferUsage(usage));
		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &VBO);
	}

	void VertexBuffer::SetData(unsigned start, unsigned _size, void* data)
	{
		ASSERT(start + _size <= size, "Out of bounds.");

		Bind();
		glBufferSubData(GL_ARRAY_BUFFER, start, _size, data);
		UnBind();
	}

	void* VertexBuffer::MapBuffer(VertexAccess accessMode)
	{
		Bind();
		return glMapBuffer(GL_ARRAY_BUFFER, ResolveVertexAccess(accessMode));
	}

	void VertexBuffer::UnmapBuffer()
	{
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	}

	unsigned VertexBuffer::GetSize() const
	{
		return size;
	}

	VertexBufferUsage VertexBuffer::GetBufferUsage() const
	{
		return usage;
	}

	void VertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
	}

	void VertexBuffer::UnBind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	}

	VertexArray::VertexArray()
	{
		glGenVertexArrays(1, &VAO);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &VAO);
	}

	void VertexArray::AddStream(VertexStream ptr)
	{
		ASSERT(ptr.buffer, "'ptr.buffer' cannot be nullptr.");
		ASSERT(!HasStream(ptr.bindingUnit), "'ptr.bindingUnit' ( %d ) is already a stream.", ptr.bindingUnit);
		ASSERT(ptr.startOffset < ptr.buffer->GetSize(), "'ptr.startOffset' cannot be greater than the size of the VertexBuffer.");

		if ((ptr.startOffset % 4) != 0)
		{
			Warning("'ptr.startOffset' ( %d ) does not start on a 4-byte boundary. This may lead to degraded performance.", ptr.startOffset);
		}

		if (ptr.stride == 0)
		{
			// Even though OpenGL will correctly interpret a zero as stream of tightly 
			// packed data, we might need the correct stride for our own use later.
			ptr.stride = CountBytes(ptr.format);
		}

		glBindVertexArray(VAO);
		ptr.buffer->Bind();
		glEnableVertexAttribArray(ptr.bindingUnit);

		switch (ptr.format)
		{
		case VertexFormat::Float:
			glVertexAttribPointer(ptr.bindingUnit, ptr.numElements, ResolveVertexFormat(ptr.format), ptr.normalized, ptr.stride, reinterpret_cast<void*>(ptr.startOffset));
			break;

		case VertexFormat::Int:
		case VertexFormat::uInt:
		case VertexFormat::Short:
		case VertexFormat::uShort:
		case VertexFormat::Byte:
		case VertexFormat::uByte:
			glVertexAttribIPointer(ptr.bindingUnit, ptr.numElements, ResolveVertexFormat(ptr.format), ptr.stride, reinterpret_cast<void*>(ptr.startOffset));
			break;

		case VertexFormat::Double:
			glVertexAttribLPointer(ptr.bindingUnit, ptr.numElements, ResolveVertexFormat(ptr.format), ptr.stride, reinterpret_cast<void*>(ptr.startOffset));
			break;
		}

		ptr.buffer->UnBind();
		glBindVertexArray(GL_NONE);

		streams.push_back(std::move(ptr));
	}

	void VertexArray::RemoveStream(unsigned bindingUnit)
	{
		for (unsigned i = 0; i < streams.size(); ++i)
		{
			if (streams[i].bindingUnit == bindingUnit)
			{
				streams.erase(streams.begin() + i);
				return;
			}
		}
	}

	bool VertexArray::HasStream(unsigned bindingUnit) const
	{
		for (auto& stream : streams)
		{
			if (stream.bindingUnit == bindingUnit)
			{
				return true;
			}
		}

		return false;
	}

	const VertexStream& VertexArray::GetStream(unsigned bindingUnit) const
	{
		for (auto& stream : streams)
		{
			if (stream.bindingUnit == bindingUnit)
			{
				return stream;
			}
		}

		ASSERT(false, "'bindingUnit' ( %d ) is out of bounds.", bindingUnit);
		return streams[0];
	}

	const VertexBuffer& VertexArray::GetBuffer(unsigned bindingUnit) const
	{
		return *GetStream(bindingUnit).buffer;
	}

	VertexBuffer& VertexArray::GetBuffer(unsigned bindingUnit)
	{
		return *GetStream(bindingUnit).buffer;
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(VAO);
	}

	void VertexArray::UnBind() const
	{
		glBindVertexArray(GL_NONE);
	}

	void VertexArray::SetVertexCount(unsigned count)
	{
#ifdef _DEBUG
		if (count > 0)
		{
			// Check to ensure that the specified count would not cause us to read past
			// any of the active buffers.
			for (unsigned i = 0; i < streams.size(); ++i)
			{
				const auto& stream = streams[i];
				const auto& buffer = stream.buffer;
				const unsigned bufferSize = buffer->GetSize();
				const unsigned end = stream.startOffset + (count - 1) * stream.stride;

				ASSERT(end < bufferSize, "'count' would cause a buffer overrun in Stream( %d ).", i);
			}
		}
#endif

		vertexCount = count;
	}

	unsigned VertexArray::GetVertexCount() const
	{
		return vertexCount;
	}
}
