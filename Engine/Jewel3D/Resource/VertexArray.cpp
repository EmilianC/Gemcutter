// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "VertexArray.h"
#include "Jewel3D/Application/Logging.h"

#include <GLEW/GL/glew.h>

namespace Jwl
{
	VertexBuffer::VertexBuffer(const VertexBuffer& other)
		: VertexBuffer(other.size, other.usage)
	{
	}

	VertexBuffer::VertexBuffer(VertexBuffer&& other)
	{
		swap(*this, other);
	}

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

	VertexBuffer& VertexBuffer::operator=(VertexBuffer other)
	{
		swap(*this, other);
		return *this;
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

	void swap(VertexBuffer& left, VertexBuffer& right)
	{
		using std::swap;

		swap(left.VBO, right.VBO);
		swap(left.size, right.size);
		swap(left.usage, right.usage);
	}

	VertexArray::VertexArray()
	{
		glGenVertexArrays(1, &VAO);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &VAO);
	}

	VertexBuffer& VertexArray::CreateBuffer(unsigned size, VertexBufferUsage usage)
	{
		return buffers.emplace_back(size, usage);
	}

	VertexBuffer& VertexArray::GetBuffer(unsigned index)
	{
		ASSERT(index < buffers.size(), "'index' is out of bounds.");

		return buffers[index];
	}

	void VertexArray::AddStream(const VertexStream& ptr)
	{
		ASSERT(ptr.bufferSource < buffers.size(), "'ptr.bufferSource' does not refer to an attached VertexBuffer.");
		const auto& buffer = buffers[ptr.bufferSource];

		if ((ptr.startOffset % 4) != 0)
		{
			Warning("VertexStream's startOffset does not start on a 4-byte boundary. This may lead to degraded performance.");
		}

		ASSERT(ptr.startOffset < buffer.GetSize(), "'ptr.startOffset' cannot be greater than the size of the VertexBuffer.");
		streams.push_back(ptr);

		glBindVertexArray(VAO);
		buffer.Bind();
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

		buffers[ptr.bufferSource].UnBind();
		glBindVertexArray(GL_NONE);
	}

	VertexStream& VertexArray::GetStream(unsigned index)
	{
		ASSERT(index < buffers.size(), "'index' is out of bounds.");

		return streams[index];
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
				const unsigned bufferSize = buffers[stream.bufferSource].GetSize();
				const unsigned end = stream.startOffset + (count - 1) * stream.stride;

				ASSERT(end < bufferSize, "'count' would cause a buffer overrun in VertexBuffer( %d ) read with Stream( %d ).", stream.bufferSource, i);
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
