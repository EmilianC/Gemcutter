// Copyright (c) 2017 Emilian Cioca
namespace Jwl
{
	template<class T>
	void UniformBuffer::SetUniform(const std::string& name, const T& data)
	{
		static_assert(std::is_standard_layout<T>::value, "Uniforms cannot be complex types.");

		T* dest = reinterpret_cast<T*>(GetBufferLoc(name));
		ASSERT(dest, "Could not find uniform parameter ( %s ).", name.c_str());
		ASSERT(dest + sizeof(T) <= reinterpret_cast<T*>(buffer) + bufferSize, "Setting uniform ( %s ) out of bounds of the buffer.", name.c_str());

		*dest = data;
		dirty = true;
	}

	template<class T>
	void UniformBuffer::SetUniformArray(const std::string& name, unsigned numElements, T* data)
	{
		static_assert(std::is_standard_layout<T>::value, "Uniforms cannot be complex types.");
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
		T* loc = reinterpret_cast<T*>(GetBufferLoc(name));
		ASSERT(loc, "\"%s\" does not match the name of a Uniform.", name.c_str());

		return UniformHandle<T>(*this, *loc);
	}

	template<class T>
	UniformHandle<T>::UniformHandle(UniformBuffer& buff, T& data)
		: buffer(&buff), data(&data)
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
		ASSERT(data, "Uniform handle is not associated with a UniformBuffer.");

		*data = value;
		buffer->dirty = true;
	}

	template<class T>
	const T& UniformHandle<T>::Get() const
	{
		ASSERT(data, "Uniform handle is not associated with a UniformBuffer.");

		return *data;
	}

	template<class T>
	UniformBuffer* UniformHandle<T>::GetBuffer() const
	{
		return buffer;
	}
}
