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

	void VertexBuffer::ClearData()
	{
		Bind();
		glClearBufferData(GL_ARRAY_BUFFER, GL_R8, GL_RED, GL_UNSIGNED_BYTE, nullptr);
		UnBind();
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
		glVertexAttribDivisor(ptr.bindingUnit, ptr.divisor);

		switch (ptr.format)
		{
		case VertexFormat::Float:
			glVertexAttribPointer(ptr.bindingUnit, 1, GL_FLOAT, ptr.normalized, ptr.stride, reinterpret_cast<void*>(ptr.startOffset));
			break;
		case VertexFormat::Double:
			glVertexAttribLPointer(ptr.bindingUnit, 1, GL_DOUBLE, ptr.stride, reinterpret_cast<void*>(ptr.startOffset));
			break;

		case VertexFormat::Vec2:
			glVertexAttribPointer(ptr.bindingUnit, 2, GL_FLOAT, ptr.normalized, ptr.stride, reinterpret_cast<void*>(ptr.startOffset));
			break;
		case VertexFormat::Vec3:
			glVertexAttribPointer(ptr.bindingUnit, 3, GL_FLOAT, ptr.normalized, ptr.stride, reinterpret_cast<void*>(ptr.startOffset));
			break;
		case VertexFormat::Vec4:
			glVertexAttribPointer(ptr.bindingUnit, 4, GL_FLOAT, ptr.normalized, ptr.stride, reinterpret_cast<void*>(ptr.startOffset));
			break;

		case VertexFormat::Mat2:
			glVertexAttribPointer(ptr.bindingUnit, 4, GL_FLOAT, ptr.normalized, ptr.stride, reinterpret_cast<void*>(ptr.startOffset));
			break;
		case VertexFormat::Mat3:
		{
			const unsigned unit0 = ptr.bindingUnit;
			const unsigned unit1 = ptr.bindingUnit + 1;
			const unsigned unit2 = ptr.bindingUnit + 2;
			ASSERT(!HasStream(unit1) && !HasStream(unit2),
				"mat3 vertex attribute requires streams [ %d, %d, %d ] to be available.", unit0, unit1, unit2);

			glEnableVertexAttribArray(unit1);
			glEnableVertexAttribArray(unit2);
			glVertexAttribDivisor(unit1, ptr.divisor);
			glVertexAttribDivisor(unit2, ptr.divisor);
			glVertexAttribPointer(unit0, 3, GL_FLOAT, ptr.normalized, ptr.stride, reinterpret_cast<void*>(ptr.startOffset));
			glVertexAttribPointer(unit1, 3, GL_FLOAT, ptr.normalized, ptr.stride, reinterpret_cast<void*>(ptr.startOffset + 12));
			glVertexAttribPointer(unit2, 3, GL_FLOAT, ptr.normalized, ptr.stride, reinterpret_cast<void*>(ptr.startOffset + 24));
			break;
		}
		case VertexFormat::Mat4:
		{
			const unsigned unit0 = ptr.bindingUnit;
			const unsigned unit1 = ptr.bindingUnit + 1;
			const unsigned unit2 = ptr.bindingUnit + 2;
			const unsigned unit3 = ptr.bindingUnit + 3;
			ASSERT(!HasStream(unit1) && !HasStream(unit2) && !HasStream(unit3),
				"mat4 vertex attribute requires streams [ %d, %d, %d, %d ] to be available.", unit0, unit1, unit2, unit3);

			glEnableVertexAttribArray(unit1);
			glEnableVertexAttribArray(unit2);
			glEnableVertexAttribArray(unit3);
			glVertexAttribDivisor(unit1, ptr.divisor);
			glVertexAttribDivisor(unit2, ptr.divisor);
			glVertexAttribDivisor(unit3, ptr.divisor);
			glVertexAttribPointer(unit0, 4, GL_FLOAT, ptr.normalized, ptr.stride, reinterpret_cast<void*>(ptr.startOffset));
			glVertexAttribPointer(unit1, 4, GL_FLOAT, ptr.normalized, ptr.stride, reinterpret_cast<void*>(ptr.startOffset + 16));
			glVertexAttribPointer(unit2, 4, GL_FLOAT, ptr.normalized, ptr.stride, reinterpret_cast<void*>(ptr.startOffset + 32));
			glVertexAttribPointer(unit3, 4, GL_FLOAT, ptr.normalized, ptr.stride, reinterpret_cast<void*>(ptr.startOffset + 48));
			break;
		}

		case VertexFormat::Int:
		case VertexFormat::uInt:
		case VertexFormat::Short:
		case VertexFormat::uShort:
		case VertexFormat::Byte:
		case VertexFormat::uByte:
			glVertexAttribIPointer(ptr.bindingUnit, 1, ResolveVertexFormat(ptr.format), ptr.stride, reinterpret_cast<void*>(ptr.startOffset));
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
			auto& stream = streams[i];

			bool found = false;
			switch (stream.format)
			{
			case VertexFormat::Mat4:
				found = stream.bindingUnit + 3 == bindingUnit;
			case VertexFormat::Mat3:
				found = found || stream.bindingUnit + 2 == bindingUnit;
				found = found || stream.bindingUnit + 1 == bindingUnit;
			default:
				found = found || stream.bindingUnit == bindingUnit;
			}

			if (found)
			{
				glBindVertexArray(VAO);
				switch (stream.format)
				{
				case VertexFormat::Mat4:
					glDisableVertexAttribArray(stream.bindingUnit + 3);
				case VertexFormat::Mat3:
					glDisableVertexAttribArray(stream.bindingUnit + 2);
					glDisableVertexAttribArray(stream.bindingUnit + 1);
				default:
					glDisableVertexAttribArray(stream.bindingUnit);
				}
				glBindVertexArray(GL_NONE);

				streams.erase(streams.begin() + i);
				return;
			}
		}
	}

	bool VertexArray::HasStream(unsigned bindingUnit) const
	{
		for (auto& stream : streams)
		{
			switch (stream.format)
			{
			case VertexFormat::Mat4:
				if (stream.bindingUnit + 3 == bindingUnit) return true;
			case VertexFormat::Mat3:
				if (stream.bindingUnit + 2 == bindingUnit) return true;
				if (stream.bindingUnit + 1 == bindingUnit) return true;
			default:
				if (stream.bindingUnit == bindingUnit) return true;
			}
		}

		return false;
	}

	const VertexStream& VertexArray::GetStream(unsigned bindingUnit) const
	{
		for (auto& stream : streams)
		{
			switch (stream.format)
			{
			case VertexFormat::Mat4:
				if (stream.bindingUnit + 3 == bindingUnit) return stream;
			case VertexFormat::Mat3:
				if (stream.bindingUnit + 2 == bindingUnit) return stream;
				if (stream.bindingUnit + 1 == bindingUnit) return stream;
			default:
				if (stream.bindingUnit == bindingUnit) return stream;
			}
		}

		ASSERT(false, "'bindingUnit' ( %d ) is not associated with a VertexStream.", bindingUnit);
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
