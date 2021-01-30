// Copyright (c) 2017 Emilian Cioca
namespace Jwl
{
	template<class T>
	UniformHandle<T> UniformBuffer::AddUniform(std::string_view name, unsigned count)
	{
		static_assert(std::is_standard_layout_v<T>, "Uniforms cannot be complex types.");

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

	template<class T>
	void UniformBuffer::SetUniform(std::string_view name, const T& data)
	{
		static_assert(std::is_standard_layout_v<T>, "Uniforms cannot be complex types.");

		T* dest = static_cast<T*>(GetBufferLoc(name));
		ASSERT(dest, "Could not find uniform parameter ( %s ).", name.data());
		ASSERT(reinterpret_cast<char*>(dest) + sizeof(T) <= static_cast<char*>(buffer) + bufferSize,
			"Setting uniform ( %s ) out of bounds of the buffer.", name.data());

		*dest = data;
		dirty = true;
	}

	template<class T>
	void UniformBuffer::SetUniformArray(std::string_view name, unsigned numElements, T* data)
	{
		static_assert(std::is_standard_layout_v<T>, "Uniforms cannot be complex types.");
		ASSERT(data != nullptr, "Source data cannot be null.");

		void* dest = GetBufferLoc(name);
		ASSERT(dest, "Could not find uniform parameter ( %s ).", name.data());
		ASSERT(reinterpret_cast<char*>(dest) + sizeof(T) * numElements <= static_cast<char*>(buffer) + bufferSize,
			"Setting uniform ( %s ) out of bounds of the buffer.", name.data());

		memcpy(dest, data, sizeof(T) * numElements);
		dirty = true;
	}

	template<class T>
	UniformHandle<T> UniformBuffer::MakeHandle(std::string_view name)
	{
		auto itr = table.find(name);
		ASSERT(itr != table.end(), "\"%s\" does not match the name of a Uniform.", name.data());

		return UniformHandle<T>(*this, itr->second);
	}

	template<> void UniformBuffer::SetUniform<mat2>(std::string_view name, const mat2& data);
	template<> void UniformBuffer::SetUniform<mat3>(std::string_view name, const mat3& data);

	template<class T>
	UniformHandle<T>::UniformHandle(UniformBuffer& buff, unsigned _offset)
		: uniformBuffer(&buff), offset(_offset)
	{
	}

	template<class T>
	UniformHandle<T>& UniformHandle<T>::operator=(const T& value)
	{
		Set(value);
		return *this;
	}

	template<class T>
	void UniformHandle<T>::Set(const T& value)
	{
		ASSERT(uniformBuffer, "Uniform handle is not associated with a UniformBuffer.");
		ASSERT(uniformBuffer->buffer, "The associated UniformBuffer has not been initialized yet.");

		T* ptr = reinterpret_cast<T*>(static_cast<char*>(uniformBuffer->buffer) + offset);
		*ptr = value;

		uniformBuffer->dirty = true;
	}

	template<class T>
	T UniformHandle<T>::Get() const
	{
		ASSERT(uniformBuffer, "Uniform handle is not associated with a UniformBuffer.");
		ASSERT(uniformBuffer->buffer, "The associated UniformBuffer has not been initialized yet.");

		return *reinterpret_cast<T*>(static_cast<char*>(uniformBuffer->buffer) + offset);
	}

	template<> void UniformHandle<mat2>::Set(const mat2& value);
	template<> void UniformHandle<mat3>::Set(const mat3& value);
	template<> mat2 UniformHandle<mat2>::Get() const;
	template<> mat3 UniformHandle<mat3>::Get() const;
}
