// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Rendering/Rendering.h"
#include "Jewel3D/Resource/Shareable.h"

#include <vector>

namespace Jwl
{
	// A single OpenGL buffer object.
	// This owns and provides access to the raw data used for rendering.
	class VertexBuffer : public Shareable<VertexBuffer>
	{
		friend class VertexArray;
	public:
		VertexBuffer(unsigned size, VertexBufferUsage usage);
		~VertexBuffer();

		void ClearData();
		void SetData(unsigned start, unsigned size, void* data);

		// Must be followed by a call to UnmapBuffer() before rendering with the buffer.
		void* MapBuffer(VertexAccess accessMode);
		void UnmapBuffer();

		unsigned GetSize() const;
		VertexBufferUsage GetBufferUsage() const;

	private:
		void Bind() const;
		void UnBind() const;

		unsigned VBO = 0;
		unsigned size = 0;
		VertexBufferUsage usage;
	};

	// A single vertex attribute to be streamed to a vertex shader.
	// Defines how to read the attribute from the given VertexBuffer.
	struct VertexStream
	{
		VertexBuffer::Ptr buffer;
		unsigned bindingUnit = 0;

		VertexFormat format = VertexFormat::Float;
		bool normalized = false;
		unsigned startOffset = 0;
		unsigned stride = 0;
		unsigned divisor = 0;
	};

	namespace detail
	{
		template<typename Type>
		class VertexRange;
	}

	// A renderable collection of VertexStreams.
	class VertexArray : public Shareable<VertexArray>
	{
	public:
		VertexArray();
		~VertexArray();

		void AddStream(VertexStream ptr);
		void RemoveStream(unsigned bindingUnit);
		bool HasStream(unsigned bindingUnit) const;
		const VertexStream& GetStream(unsigned bindingUnit) const;
		const VertexBuffer& GetBuffer(unsigned bindingUnit) const;
		VertexBuffer& GetBuffer(unsigned bindingUnit);

		// Returns an enumerable range over the specific elements pointed to by a VertexStream.
		// Elements will be casted to 'Type'. You must ensure that this cast is safe for the contents of the VertexBuffer.
		// The full buffer will be enumerated, not just 'VertexCount' number of elements.
		template<typename Type>
		detail::VertexRange<Type> GetStream(unsigned bindingUnit, VertexAccess access = VertexAccess::ReadWrite);

		void Bind() const;
		void UnBind() const;

		void SetVertexCount(unsigned count);
		unsigned GetVertexCount() const;
		const auto& GetStreams() const { return streams; }

	private:
		unsigned VAO = 0;
		unsigned vertexCount = 0;

		std::vector<VertexStream> streams;
	};
}

#include "VertexArray.inl"
