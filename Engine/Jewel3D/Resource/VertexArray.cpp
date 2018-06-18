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

	void VertexBuffer::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
	}

	void VertexBuffer::UnBind()
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
		ASSERT(ptr.bufferSource < buffers.size(), "'ptr.bufferSource' is out of bounds.");

		if ((ptr.startOffset % 4) != 0)
		{
			Warning("VertexStream's startOffset does not start on a 4-byte boundary. This may lead to degraded performance.");
		}

		streams.push_back(ptr);

		glBindVertexArray(VAO);
		buffers[ptr.bufferSource].Bind();
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
		ASSERT(index < buffers.size(), "'ptr.bufferSource' is out of bounds.");

		return streams[index];
	}

	void VertexArray::Bind()
	{
		glBindVertexArray(VAO);
	}

	void VertexArray::UnBind()
	{
		glBindVertexArray(GL_NONE);
	}

	unsigned VertexArray::GetVertexCount() const
	{
		return numVertices;
	}
}
