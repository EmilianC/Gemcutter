// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D\Rendering\Rendering.h"
#include "Jewel3D\Resource\Shareable.h"

#include <vector>

namespace Jwl
{
	// A single OpenGL buffer object.
	// This owns and provides access to the array of data used for rendering.
	class VertexBuffer
	{
	public:
		VertexBuffer(const VertexBuffer&);
		VertexBuffer(VertexBuffer&&);
		VertexBuffer(unsigned size, VertexBufferUsage usage);
		~VertexBuffer();

		VertexBuffer& operator=(VertexBuffer);

		void SetData(unsigned start, unsigned size, void* data);

		// Must be followed by a call to UnmapBuffer after the returned pointer is no longer needed.
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

		friend void swap(VertexBuffer&, VertexBuffer&);
		friend class VertexArray;
	};

	// A single vertex attribute to be streamed to a vertex shader.
	// Defines how to read the attribute from the given VertexBuffer.
	struct VertexStream
	{
		unsigned bufferSource = 0;
		unsigned bindingUnit = 0;

		VertexFormat format = VertexFormat::Float;
		bool normalized = false;
		unsigned numElements = 1;
		unsigned startOffset = 0;
		unsigned stride = 0;
	};

	namespace detail
	{
		template<typename Type>
		class VertexRange;
	}

	// 
	class VertexArray : public Shareable<VertexArray>
	{
	public:
		VertexArray();
		~VertexArray();

		VertexBuffer& CreateBuffer(unsigned size, VertexBufferUsage usage);
		VertexBuffer& GetBuffer(unsigned index);

		void AddStream(const VertexStream& ptr);
		VertexStream& GetStream(unsigned index);

		// 
		template<typename Type>
		detail::VertexRange<Type> GetStream(unsigned index, VertexAccess access = VertexAccess::ReadWrite);

		void Bind() const;
		void UnBind() const;

		void SetVertexCount(unsigned count);
		unsigned GetVertexCount() const;
		const auto& GetBuffers() const { return buffers; }
		const auto& GetStreams() const { return streams; }

	private:
		unsigned VAO = 0;
		unsigned vertexCount = 0;

		std::vector<VertexBuffer> buffers;
		std::vector<VertexStream> streams;
	};
}

#include "VertexArray.inl"
