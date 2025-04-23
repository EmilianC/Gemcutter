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

#if GEM_DEBUG
		if constexpr (std::is_same_v<Type, float>)          { ASSERT(stream.format == VertexFormat::Float,  "Stream requested with mismatched type."); }
		if constexpr (std::is_same_v<Type, double>)         { ASSERT(stream.format == VertexFormat::Double, "Stream requested with mismatched type."); }
		if constexpr (std::is_same_v<Type, vec2>)           { ASSERT(stream.format == VertexFormat::Vec2,   "Stream requested with mismatched type."); }
		if constexpr (std::is_same_v<Type, vec3>)           { ASSERT(stream.format == VertexFormat::Vec3,   "Stream requested with mismatched type."); }
		if constexpr (std::is_same_v<Type, vec4>)           { ASSERT(stream.format == VertexFormat::Vec4,   "Stream requested with mismatched type."); }
		if constexpr (std::is_same_v<Type, mat2>)           { ASSERT(stream.format == VertexFormat::Mat2,   "Stream requested with mismatched type."); }
		if constexpr (std::is_same_v<Type, mat3>)           { ASSERT(stream.format == VertexFormat::Mat3,   "Stream requested with mismatched type."); }
		if constexpr (std::is_same_v<Type, mat4>)           { ASSERT(stream.format == VertexFormat::Mat4,   "Stream requested with mismatched type."); }
		if constexpr (std::is_same_v<Type, int>)            { ASSERT(stream.format == VertexFormat::Int,    "Stream requested with mismatched type."); }
		if constexpr (std::is_same_v<Type, unsigned>)       { ASSERT(stream.format == VertexFormat::uInt,   "Stream requested with mismatched type."); }
		if constexpr (std::is_same_v<Type, short>)          { ASSERT(stream.format == VertexFormat::Short,  "Stream requested with mismatched type."); }
		if constexpr (std::is_same_v<Type, unsigned short>) { ASSERT(stream.format == VertexFormat::uShort, "Stream requested with mismatched type."); }
		if constexpr (std::is_same_v<Type, std::byte>)      { ASSERT(stream.format == VertexFormat::Byte,   "Stream requested with mismatched type."); }
		if constexpr (std::is_same_v<Type, unsigned char>)  { ASSERT(stream.format == VertexFormat::uByte,  "Stream requested with mismatched type."); }
#endif

		return { *stream.buffer, stream, access };
	}
}
