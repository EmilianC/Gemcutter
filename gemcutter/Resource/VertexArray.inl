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
			VertexIterator(std::byte* _itr, unsigned _stride)
				: itr(_itr), stride(_stride) {}

			VertexIterator& operator=(const VertexIterator&) = default;

			[[nodiscard]] bool operator==(const VertexIterator& other) const { return itr == other.itr; }
			[[nodiscard]] bool operator!=(const VertexIterator& other) const { return itr != other.itr; }
			[[nodiscard]] bool operator<(const VertexIterator& other)  const { return itr < other.itr; }
			[[nodiscard]] bool operator>(const VertexIterator& other)  const { return itr > other.itr; }
			[[nodiscard]] bool operator<=(const VertexIterator& other) const { return itr <= other.itr; }
			[[nodiscard]] bool operator>=(const VertexIterator& other) const { return itr >= other.itr; }

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
			std::byte* itr;
			unsigned stride;
		};

		// Enumerable range of a specific VertexBuffer stream/element.
		template<typename Type>
		class [[nodiscard]] VertexRange
		{
		public:
			VertexRange(VertexBuffer& buffer, const VertexStream& _stream, VertexAccess access)
				: mapping(buffer.MapBuffer(access))
				, stream(&_stream)
			{
			}

			VertexIterator<Type> begin()
			{
				return { mapping.GetPtr() + stream->startOffset, stream->stride };
			}

			VertexIterator<Type> end()
			{
				return { mapping.GetPtr() + stream->startOffset + mapping.GetBuffer().GetSize(), stream->stride };
			}

		private:
			BufferMapping mapping;
			const VertexStream* stream;
		};
	}

	template<typename T>
	void BufferMapping::SetElement(unsigned offset, const T& element)
	{
		ASSERT(offset + sizeof(T) <= buffer.size, "Out of bounds.");

		T* dest = reinterpret_cast<T*>(data + offset);
		*dest = element;
	}

	template<typename T>
	void BufferMapping::Fill(unsigned start, unsigned count, const T& element)
	{
		ASSERT(start + count * sizeof(T) <= buffer.size, "Out of bounds.");

		T* dest = reinterpret_cast<T*>(data + start);
		for (; count > 0; --count)
		{
			*dest = element;
			++dest;
		}
	}

	template<typename Type>
	detail::VertexRange<Type> VertexArray::GetStream(unsigned bindingUnit, VertexAccess access)
	{
		auto& stream = GetStream(bindingUnit);

		return { *stream.buffer, stream, access };
	}
}
