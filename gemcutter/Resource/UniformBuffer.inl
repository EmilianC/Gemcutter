// Copyright (c) 2017 Emilian Cioca
namespace gem
{
	template<shader_uniform T>
	UniformHandle<T> UniformBuffer::AddUniform(std::string_view name, unsigned count)
	{
		unsigned alignment = sizeof(T);
		unsigned size = sizeof(T);

		// Due to alignment rules, some types are adjusted.
		if constexpr (std::is_same_v<T, vec3> || std::is_same_v<T, mat4>)
		{
			alignment = sizeof(vec4);
		}
		else if constexpr (std::is_same_v<T, mat2>)
		{
			alignment = sizeof(vec4);
			size = sizeof(vec4) * 2;
		}
		else if constexpr (std::is_same_v<T, mat3>)
		{
			alignment = sizeof(vec4);
			size = sizeof(vec4) * 3;
		}

		AddUniform(std::string(name), size, alignment, count);

		return MakeHandle<T>(name);
	}

	template<shader_uniform T>
	void UniformBuffer::SetUniform(std::string_view name, const T& data)
	{
		T* dest = static_cast<T*>(GetBufferLoc(name));
		ASSERT(dest, "Could not find uniform parameter ( %s ).", name.data());
		ASSERT(reinterpret_cast<std::byte*>(dest) + sizeof(T) <= static_cast<std::byte*>(buffer) + bufferSize,
			"Setting uniform ( %s ) out of bounds of the buffer.", name.data());

		*dest = data;
		dirty = true;
	}

	template<shader_uniform T>
	void UniformBuffer::SetUniformArray(std::string_view name, unsigned numElements, T* data)
	{
		ASSERT(data != nullptr, "Source data cannot be null.");

		void* dest = GetBufferLoc(name);
		ASSERT(dest, "Could not find uniform parameter ( %s ).", name.data());
		ASSERT(static_cast<std::byte*>(dest) + sizeof(T) * numElements <= static_cast<std::byte*>(buffer) + bufferSize,
			"Setting uniform ( %s ) out of bounds of the buffer.", name.data());

		memcpy(dest, data, sizeof(T) * numElements);
		dirty = true;
	}

	template<shader_uniform T>
	UniformHandle<T> UniformBuffer::MakeHandle(std::string_view name)
	{
		auto itr = table.find(name);
		ASSERT(itr != table.end(), "\"%s\" does not match the name of a Uniform.", name.data());

		return UniformHandle<T>(*this, itr->second);
	}

	extern template void UniformBuffer::SetUniform<mat2>(std::string_view name, const mat2& data);
	extern template void UniformBuffer::SetUniform<mat3>(std::string_view name, const mat3& data);

	template<shader_uniform T>
	UniformHandle<T>::UniformHandle(UniformBuffer& buff, unsigned _offset)
		: uniformBuffer(&buff), offset(_offset)
	{
	}

	template<shader_uniform T>
	UniformHandle<T>& UniformHandle<T>::operator=(const T& value)
	{
		Set(value);
		return *this;
	}

	template<shader_uniform T>
	void UniformHandle<T>::Set(const T& value)
	{
		ASSERT(uniformBuffer, "Uniform handle is not associated with a UniformBuffer.");
		ASSERT(uniformBuffer->buffer, "The associated UniformBuffer has not been initialized yet.");

		T* ptr = reinterpret_cast<T*>(static_cast<std::byte*>(uniformBuffer->buffer) + offset);
		*ptr = value;

		uniformBuffer->dirty = true;
	}

	template<shader_uniform T>
	T UniformHandle<T>::Get() const
	{
		ASSERT(uniformBuffer, "Uniform handle is not associated with a UniformBuffer.");
		ASSERT(uniformBuffer->buffer, "The associated UniformBuffer has not been initialized yet.");

		return *reinterpret_cast<T*>(static_cast<std::byte*>(uniformBuffer->buffer) + offset);
	}

	extern template void UniformHandle<mat2>::Set(const mat2& value);
	extern template void UniformHandle<mat3>::Set(const mat3& value);
	extern template mat2 UniformHandle<mat2>::Get() const;
	extern template mat3 UniformHandle<mat3>::Get() const;
}
