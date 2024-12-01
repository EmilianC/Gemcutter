// Copyright (c) 2017 Emilian Cioca
#include "VertexArray.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Math/Math.h"
#include "gemcutter/Utilities/ScopeGuard.h"

#include <glew/glew.h>
#include <loupe/loupe.h>
#include <numeric>

namespace gem
{
	BufferMapping::BufferMapping(VertexBuffer& _buffer, VertexAccess accessMode)
		: buffer(&_buffer)
	{
		buffer->Bind();
		data = static_cast<unsigned char*>(glMapBuffer(buffer->target, ResolveVertexAccess(accessMode)));
		buffer->UnBind();

		ASSERT(data, "Failed to map VertexBuffer.");
	}

	BufferMapping::~BufferMapping()
	{
		buffer->Bind();
		glUnmapBuffer(buffer->target);
		buffer->UnBind();
	}

	void BufferMapping::ZeroData(unsigned start, unsigned size)
	{
		ASSERT(start + size <= buffer->size, "Out of bounds.");

		std::memset(data + start, 0, size);
	}

	void BufferMapping::SetData(unsigned start, unsigned size, const void* source)
	{
		ASSERT(start + size <= buffer->size, "Out of bounds.");

		std::memcpy(data + start, source, size);
	}

	unsigned char* BufferMapping::GetPtr()
	{
		return data;
	}

	const VertexBuffer& BufferMapping::GetBuffer() const
	{
		return *buffer;
	}

	void BufferMapping::FillIndexSequence(unsigned start, unsigned count, unsigned short firstIndex)
	{
		ASSERT(buffer->target == GL_ELEMENT_ARRAY_BUFFER, "Index sequences can only used for index buffers.");
		ASSERT(start + count * sizeof(unsigned short) <= buffer->size, "Out of bounds.");

		auto* begin = reinterpret_cast<unsigned short*>(data + start);
		auto* end   = begin + count;
		std::iota(begin, end, firstIndex);
	}

	void BufferMapping::SetRestartIndex(unsigned offset)
	{
		ASSERT(buffer->target == GL_ELEMENT_ARRAY_BUFFER, "Restart indices can only be set inside index buffers.");
		ASSERT(offset <= buffer->size, "Out of bounds.");

		auto* dest = reinterpret_cast<unsigned short*>(data + offset);
		*dest = VertexBuffer::RESTART_INDEX;
	}

	VertexBuffer::VertexBuffer(unsigned _size, BufferUsage _usage, VertexBufferType type)
		: VertexBuffer(_size, nullptr, _usage, type)
	{
	}

	VertexBuffer::VertexBuffer(unsigned _size, const void* source, BufferUsage _usage, VertexBufferType type)
		: size(_size)
		, usage(_usage)
		, target(type == VertexBufferType::Data ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER)
	{
		ASSERT(size, "A VertexBuffer must have a non-zero size.");

		glGenBuffers(1, &VBO);
		glBindBuffer(target, VBO);
		glBufferData(target, size, source, ResolveBufferUsage(usage));
		glBindBuffer(target, GL_NONE);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &VBO);
	}

	void VertexBuffer::ClearData()
	{
		Bind();
		glClearBufferData(target, GL_R8, GL_RED, GL_UNSIGNED_BYTE, nullptr);
		UnBind();
	}

	void VertexBuffer::ClearData(unsigned start, unsigned _size)
	{
		ASSERT(start + _size <= size, "Out of bounds.");

		Bind();
		glClearBufferSubData(target, GL_R8, start, _size, GL_RED, GL_UNSIGNED_BYTE, nullptr);
		UnBind();
	}

	void VertexBuffer::SetData(unsigned start, unsigned _size, const void* source)
	{
		ASSERT(start + _size <= size, "Out of bounds.");

		Bind();
		glBufferSubData(target, start, _size, source);
		UnBind();
	}

	void VertexBuffer::Resize(unsigned newSize, bool transferData)
	{
		if (newSize == size)
			return;

		Bind();
		if (transferData && size != 0)
		{
			const unsigned copySize = Min(size, newSize);
			auto* copy = malloc(copySize);
			defer { free(copy); };

			const void* data = glMapBuffer(target, GL_READ_ONLY);
			memcpy(copy, data, copySize);
			glUnmapBuffer(target);

			glBufferData(target, newSize, nullptr, ResolveBufferUsage(usage));
			glBufferSubData(target, 0, copySize, copy);
		}
		else
		{
			glBufferData(target, newSize, nullptr, ResolveBufferUsage(usage));
		}

		UnBind();

		size = newSize;
	}

	BufferMapping VertexBuffer::MapBuffer(VertexAccess accessMode)
	{
		return { *this, accessMode };
	}

	unsigned VertexBuffer::GetSize() const
	{
		return size;
	}

	BufferUsage VertexBuffer::GetBufferUsage() const
	{
		return usage;
	}

	VertexBufferType VertexBuffer::GetBufferType() const
	{
		return (target == GL_ARRAY_BUFFER) ? VertexBufferType::Data : VertexBufferType::Index;
	}

	void VertexBuffer::Bind() const
	{
		glBindBuffer(target, VBO);
	}

	void VertexBuffer::UnBind() const
	{
		glBindBuffer(target, GL_NONE);
	}

	VertexArray::VertexArray()
	{
		glGenVertexArrays(1, &VAO);
	}

	VertexArray::VertexArray(VertexArrayFormat _format)
	{
		glGenVertexArrays(1, &VAO);
		format = _format;
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &VAO);
	}

	void VertexArray::SetIndexBuffer(VertexBuffer::Ptr buffer)
	{
		indexBuffer = std::move(buffer);
		if (indexBuffer)
		{
			ASSERT(indexBuffer->GetBufferType() == VertexBufferType::Index, "'buffer' must contain indices.");
		}
	}

	const VertexBuffer* VertexArray::GetIndexBuffer() const
	{
		return indexBuffer.get();
	}

	VertexBuffer* VertexArray::GetIndexBuffer()
	{
		return indexBuffer.get();
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
			// Even though OpenGL will correctly interpret a zero as a stream of tightly
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

		case VertexFormat::Int:    [[fallthrough]];
		case VertexFormat::uInt:   [[fallthrough]];
		case VertexFormat::Short:  [[fallthrough]];
		case VertexFormat::uShort: [[fallthrough]];
		case VertexFormat::Byte:   [[fallthrough]];
		case VertexFormat::uByte:
			glVertexAttribIPointer(ptr.bindingUnit, 1, ResolveVertexFormat(ptr.format), ptr.stride, reinterpret_cast<void*>(ptr.startOffset));
			break;
		}

		ptr.buffer->UnBind();
		glBindVertexArray(GL_NONE);

		streams.push_back(std::move(ptr));
	}

	bool VertexArray::HasStream(unsigned bindingUnit) const
	{
		for (auto& stream : streams)
		{
			switch (stream.format)
			{
			case VertexFormat::Mat4:
				if (stream.bindingUnit + 3 == bindingUnit) return true;
				[[fallthrough]];
			case VertexFormat::Mat3:
				if (stream.bindingUnit + 2 == bindingUnit) return true;
				if (stream.bindingUnit + 1 == bindingUnit) return true;
				[[fallthrough]];
			default:
				if (stream.bindingUnit == bindingUnit) return true;
			}
		}

		return false;
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
				[[fallthrough]];
			case VertexFormat::Mat3:
				found = found || stream.bindingUnit + 2 == bindingUnit;
				found = found || stream.bindingUnit + 1 == bindingUnit;
				[[fallthrough]];
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
					[[fallthrough]];
				case VertexFormat::Mat3:
					glDisableVertexAttribArray(stream.bindingUnit + 2);
					glDisableVertexAttribArray(stream.bindingUnit + 1);
					[[fallthrough]];
				default:
					glDisableVertexAttribArray(stream.bindingUnit);
				}
				glBindVertexArray(GL_NONE);

				streams.erase(streams.begin() + i);
				return;
			}
		}
	}

	void VertexArray::RemoveStreams()
	{
		glBindVertexArray(VAO);
		for (const VertexStream& stream : streams)
		{
			switch (stream.format)
			{
			case VertexFormat::Mat4:
				glDisableVertexAttribArray(stream.bindingUnit + 3);
				[[fallthrough]];
			case VertexFormat::Mat3:
				glDisableVertexAttribArray(stream.bindingUnit + 2);
				glDisableVertexAttribArray(stream.bindingUnit + 1);
				[[fallthrough]];
			default:
				glDisableVertexAttribArray(stream.bindingUnit);
			}
		}

		glBindVertexArray(GL_NONE);
		streams.clear();
	}

	const VertexStream& VertexArray::GetStream(unsigned bindingUnit) const
	{
		for (auto& stream : streams)
		{
			switch (stream.format)
			{
			case VertexFormat::Mat4:
				if (stream.bindingUnit + 3 == bindingUnit) return stream;
				[[fallthrough]];
			case VertexFormat::Mat3:
				if (stream.bindingUnit + 2 == bindingUnit) return stream;
				if (stream.bindingUnit + 1 == bindingUnit) return stream;
				[[fallthrough]];
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

	void VertexArray::Draw() const
	{
		Draw(0, format);
	}

	void VertexArray::Draw(unsigned firstIndex) const
	{
		Draw(firstIndex, format);
	}

	void VertexArray::Draw(unsigned firstIndex, VertexArrayFormat formatOverride) const
	{
		if (indexBuffer)
		{
			indexBuffer->Bind();
			glDrawElementsInstanced(ResolveVertexArrayFormat(formatOverride), vertexCount, GL_UNSIGNED_SHORT, reinterpret_cast<void*>(firstIndex * sizeof(unsigned short)), instanceCount);
		}
		else
		{
			glDrawArraysInstanced(ResolveVertexArrayFormat(formatOverride), firstIndex, vertexCount, instanceCount);
		}
	}

	void VertexArray::SetVertexCount(unsigned count)
	{
#ifdef GEM_DEBUG
		if (count > 0)
		{
			// Check to ensure that the specified count would not cause us to read past any of the buffers.
			for (unsigned i = 0; i < streams.size(); ++i)
			{
				const auto& stream = streams[i];
				const unsigned bufferSize = stream.buffer->GetSize();
				const unsigned last = (stream.divisor == 0)
					? stream.startOffset + CountBytes(stream.format) + (count - 1) * stream.stride
					: stream.startOffset + CountBytes(stream.format);

				ASSERT(last <= bufferSize, "Rendering %d vertices would cause a buffer overrun in Stream( %d ).", count, i);
			}
		}
#endif
		vertexCount = count;
	}

	unsigned VertexArray::GetVertexCount() const
	{
		return vertexCount;
	}

	void VertexArray::SetInstanceCount(unsigned count)
	{
#ifdef GEM_DEBUG
		if (count > 0)
		{
			// Check to ensure that the specified number of instances would not cause us to read past any of the instanced streams.
			for (unsigned i = 0; i < streams.size(); ++i)
			{
				const auto& stream = streams[i];
				if (stream.divisor == 0)
					continue;

				const unsigned bufferSize = stream.buffer->GetSize();
				const unsigned numSteps = (count - 1) / stream.divisor;
				const unsigned last = stream.startOffset + CountBytes(stream.format) + numSteps * stream.stride;

				ASSERT(last <= bufferSize, "Rendering %d instances would cause a buffer overrun in Stream( %d ).", count, i);
			}
		}
#endif
		instanceCount = count;
	}

	unsigned VertexArray::GetInstanceCount() const
	{
		return instanceCount;
	}
}

REFLECT_SIMPLE(gem::VertexBuffer);

REFLECT(gem::VertexStream)
	MEMBERS {
		REF_MEMBER(buffer)
		REF_MEMBER(bindingUnit)
		REF_MEMBER(format)
		REF_MEMBER(normalized)
		REF_MEMBER(startOffset)
		REF_MEMBER(stride)
		REF_MEMBER(divisor)
	}
REF_END;
