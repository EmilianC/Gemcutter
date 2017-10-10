// Copyright (c) 2017 Emilian Cioca
namespace Jwl
{
	template<class T>
	UniformHandle<T> UniformBuffer::AddUniform(const std::string& name, unsigned count)
	{
		static_assert(std::is_standard_layout_v<T>, "Uniforms cannot be complex types.");

		// Due to alignment rules, some types are padded up.
		unsigned alignment;
		if constexpr (std::is_same_v<T, vec3>)
		{
			alignment = sizeof(vec4);
		}
		else if constexpr (std::is_same_v<T, mat3>)
		{
			alignment = sizeof(mat4);
		}
		else
		{
			alignment = sizeof(T);
		}

		AddUniform(name, sizeof(T), alignment, count);

		return MakeHandle<T>(name);
	}

	template<class T>
	void UniformBuffer::SetUniform(const std::string& name, const T& data)
	{
		static_assert(std::is_standard_layout_v<T>, "Uniforms cannot be complex types.");

		T* dest = reinterpret_cast<T*>(GetBufferLoc(name));
		ASSERT(dest, "Could not find uniform parameter ( %s ).", name.c_str());
		ASSERT(dest + sizeof(T) <= reinterpret_cast<T*>(buffer) + bufferSize, "Setting uniform ( %s ) out of bounds of the buffer.", name.c_str());

		*dest = data;
		dirty = true;
	}

	template<class T>
	void UniformBuffer::SetUniformArray(const std::string& name, unsigned numElements, T* data)
	{
		static_assert(std::is_standard_layout_v<T>, "Uniforms cannot be complex types.");
		ASSERT(data != nullptr, "Source data cannot be null.");

		void* dest = GetBufferLoc(name);
		ASSERT(dest, "Could not find uniform parameter ( %s ).", name.c_str());
		ASSERT(dest + sizeof(T) * numElements <= reinterpret_cast<char*>(buffer) + bufferSize, "Setting uniform ( %s ) out of bounds of the buffer.", name.c_str());

		memcpy(dest, data, sizeof(T) * numElements);
		dirty = true;
	}

	template<class T>
	UniformHandle<T> UniformBuffer::MakeHandle(const std::string& name)
	{
		auto itr = table.find(name);
		ASSERT(itr != table.end(), "\"%s\" does not match the name of a Uniform.", name.c_str());

		return UniformHandle<T>(*this, itr->second);
	}

	template<class T>
	UniformHandle<T>::UniformHandle(UniformBuffer& buff, unsigned _offset)
		: uniformBuffer(&buff), offset(_offset)
	{
	}

	template<class T>
	void UniformHandle<T>::operator=(const T& value)
	{
		Set(value);
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

	template<> void UniformHandle<mat3>::Set(const mat3& value);
	template<> mat3 UniformHandle<mat3>::Get() const;
}
