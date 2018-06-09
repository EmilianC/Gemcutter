// Copyright (c) 2017 Emilian Cioca
namespace Jwl
{
	namespace detail
	{
		struct VertexRangeEndSentinel {};

		// 
		template<typename Type>
		class VertexIterator : public std::iterator<std::bidirectional_iterator_tag, Type>
		{
		public:
			VertexIterator(unsigned char* _data, unsigned char* _end, const unsigned _stride)
				: data(_data), end(_end), stride(_stride)
			{
			}

			VertexIterator& operator=(const VertexIterator&) = default;

			VertexIterator& operator++()
			{
				data += stride;
				return *this;
			}

			VertexIterator& operator--()
			{
				data -= stride;
				return *this;
			}

			Type& operator*() const
			{
				return *reinterpret_cast<Type*>(data);
			}

			bool operator!=(VertexRangeEndSentinel) const
			{
				return data < end;
			}

		private:
			unsigned char* data;
			unsigned char* end;
			const unsigned stride;
		};

		template<typename Type>
		class VertexRange
		{
		public:
			VertexRange(VertexBuffer& _buffer, VertexStream& _stream, VertexAccess _access)
				: buffer(_buffer), stream(_stream), access(_access)
			{
			}

			~VertexRange()
			{
				buffer.UnmapBuffer();
			}

			VertexIterator<Type> begin()
			{
				auto* data = static_cast<unsigned char*>(buffer.MapBuffer(access));
				auto* end = data + buffer.GetSize();

				return VertexIterator<Type>(data + stream.startOffset, end, stream.stride);
			}

			VertexRangeEndSentinel end() { return {}; }

		private:
			VertexBuffer& buffer;
			VertexStream& stream;
			VertexAccess access;
		};
	}

	template<typename Type>
	detail::VertexRange<Type> VertexArray::GetStream(unsigned index, VertexAccess access)
	{
		auto& stream = GetStream(index);
		auto& buffer = GetBuffer(stream.bufferSource);

		return detail::VertexRange<Type>(buffer, stream, access);
	}
}
