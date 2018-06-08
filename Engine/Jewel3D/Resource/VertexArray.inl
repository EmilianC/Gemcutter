// Copyright (c) 2017 Emilian Cioca
namespace Jwl
{
	template<typename Type>
	const Type* VertexBuffer::ReadData() const
	{
		return reinterpret_cast<const Type*>(data);
	}
		
	template<typename Type>
	Type* VertexBuffer::Data()
	{
		dirty = true;
		return reinterpret_cast<Type*>(data);
	}
}