// Copyright (c) 2017 Emilian Cioca
namespace gem
{
	namespace detail
	{
		// Enumerates a VertexBuffer with respect to a VertexStream while also performing a cast and a dereference.
		template<typename Type>
		class VertexIterator
		{
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type        = Type;
			using difference_type   = std::ptrdiff_t;
			using pointer           = Type*;
			using reference         = Type&;

			VertexIterator(const VertexIterator&) = default;
			VertexIterator(unsigned char* _itr, unsigned _stride)
				: itr(_itr), stride(_stride) {}

			VertexIterator& operator=(const VertexIterator&) = default;

			bool operator==(const VertexIterator& other) const { return itr == other.itr; }
			bool operator!=(const VertexIterator& other) const { return itr != other.itr; }
			bool operator<(const VertexIterator& other)  const { return itr < other.itr; }
			bool operator>(const VertexIterator& other)  const { return itr > other.itr; }
			bool operator<=(const VertexIterator& other) const { return itr <= other.itr; }
			bool operator>=(const VertexIterator& other) const { return itr >= other.itr; }

			std::ptrdiff_t operator-(const VertexIterator& other) const { return (itr - other.itr) / stride; }

			Type& operator[](std::ptrdiff_t diff) const { return *(*this + diff); }

			VertexIterator operator+(std::ptrdiff_t diff) const { return {itr + stride * diff, stride}; }
			VertexIterator operator-(std::ptrdiff_t diff) const { return {itr - stride * diff, stride}; }
			VertexIterator& operator+=(std::ptrdiff_t diff) { itr += stride * diff; return *this; }
			VertexIterator& operator-=(std::ptrdiff_t diff) { itr -= stride * diff; return *this; }

			VertexIterator& operator--() { itr -= stride; return *this; }
			VertexIterator& operator++() { itr += stride; return *this; }

			VertexIterator operator++(int)
			{
				VertexIterator result = *this;
				++(*this);
				return result;
			}

			VertexIterator operator--(int)
			{
				VertexIterator result = *this;
				--(*this);
				return result;
			}

			Type& operator*()  const { return *reinterpret_cast<Type*>(itr); }
			Type* operator->() const { return *reinterpret_cast<Type*>(itr); }

		private:
			unsigned char* itr;
			unsigned stride;
		};

		// Enumerable range of a specific VertexBuffer stream/element.
		template<typename Type>
		class VertexRange
		{
		public:
			VertexRange(VertexBuffer& _buffer, const VertexStream& _stream, VertexAccess access)
				: buffer(_buffer), stream(_stream)
			{
				data = static_cast<unsigned char*>(buffer.MapBuffer(access));
			}

			~VertexRange()
			{
				buffer.UnmapBuffer();
			}

			VertexIterator<Type> begin() const
			{
				return {data + stream.startOffset, stream.stride};
			}

			VertexIterator<Type> end() const
			{
				return {data + stream.startOffset + buffer.GetSize(), stream.stride};
			}

		private:
			unsigned char* data;
			VertexBuffer& buffer;
			const VertexStream& stream;
		};
	}

	template<typename Type>
	detail::VertexRange<Type> VertexArray::GetStream(unsigned bindingUnit, VertexAccess access)
	{
		auto& stream = GetStream(bindingUnit);

		return detail::VertexRange<Type>(*stream.buffer, stream, access);
	}
}
