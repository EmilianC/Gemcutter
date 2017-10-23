// Copyright (c) 2017 Emilian Cioca
namespace Reflection
{
	template<typename MetaTag>
	bool Member::HasMetaTag() const
	{
		return GetMetaTag<MetaTag>() != nullptr;
	}

	template<typename MetaTag>
	const MetaTag* Member::GetMetaTag() const
	{
		for (auto metaTag : metaTags)
		{
			if (auto result = dynamic_cast<const MetaTag*>(metaTag))
			{
				return result;
			}
		}

		return nullptr;
	}

	template<typename MetaTag>
	void Member::AddMetaTag()
	{
		static_assert(std::is_default_constructible_v<MetaTag>, "MetaTag must be default constructible.");
		static_assert(std::is_base_of_v<MetaTagBase, MetaTag>, "MetaTag must inherit from MetaTagBase.");

		metaTags.push_back(new MetaTag());
	}

	template<typename ClassType, typename MemberType>
	void Member::set(ClassType& instance, MemberType& value)
	{
		if (GetMetaTag<ReadOnly>())
		{
			return;
		}

		if (auto setter = GetMetaTag<detail::Setter_Impl>())
		{
			setter->set(reinterpret_cast<void*>(&instance), reinterpret_cast<void*>(&value));
		}
		else
		{
			*reinterpret_cast<MemberType*>((char*)&instance + traits.offset) = value;
		}
	}

	template<typename MemberType, typename ClassType>
	MemberType& Member::get(ClassType& instance)
	{
		if (auto getter = GetMetaTag<detail::Getter_Impl>())
		{
			return *reinterpret_cast<MemberType*>(getter->get(reinterpret_cast<void*>(&instance)));
		}
		else
		{
			return *reinterpret_cast<MemberType*>((char*)&instance + traits.offset);
		}
	}

	template<typename MetaTag>
	bool Type::HasMetaTag() const
	{
		return GetMetaTag<MetaTag>() != nullptr;
	}

	template<typename MetaTag>
	const MetaTag* Type::GetMetaTag() const
	{
		for (auto metaTag : metaTags)
		{
			if (auto result = dynamic_cast<const MetaTag*>(metaTag))
			{
				return result;
			}
		}

		return nullptr;
	}

	template<typename T>
	static Type& Type::Create()
	{
		using Inspector = detail::TypeInspector<T>::Internal;

		auto& typeMap = GetTypeMap();

		auto itr = typeMap.find(Inspector::Traits.name);
		if (itr != typeMap.end())
		{
			return itr->second;
		}

		return typeMap.emplace(Inspector::Traits.name, Type(Inspector::Traits)).first->second;
	}

	template<typename T>
	static Type* Type::Find()
	{
		using Inspector = detail::TypeInspector<T>::Internal;

		auto& typeMap = GetTypeMap();

		auto itr = typeMap.find(Inspector::Traits.name);
		if (itr != typeMap.end())
		{
			return &itr->second;
		}
		else
		{
			return nullptr;
		}
	};

	template<typename MetaTag>
	void Type::AddMetaTag()
	{
		static_assert(std::is_default_constructible_v<MetaTag>, "MetaTag must be default constructible.");
		static_assert(std::is_base_of_v<MetaTagBase, MetaTag>, "MetaTag must inherit from MetaTagBase.");

		metaTags.push_back(new MetaTag());
	}

	template<typename Base>
	void Type::AddBase()
	{
		if (auto type = Find(detail::TypeInspector<Base>::Internal::Traits.name))
		{
			baseClasses.push_back(type);
		}
	}

	template<typename MemberInspector>
	void Type::AddMember()
	{
		auto itr = members.emplace(MemberInspector::Internal::Traits.name, Member(MemberInspector::Internal::Traits));

		MemberInspector::Internal::MetaTagCollector::AddMetaTags(itr.first->second);
	}

	template<typename EnumValue>
	void Type::AddEnumValue()
	{
		enumValues[EnumValue::Name::c_str()] = EnumValue::value;
	}
}

/* Reflection metadata for fundamental C++ types. */
REFLECT_BASIC(int)
REFLECT_BASIC(float)
REFLECT_BASIC(double)
REFLECT_BASIC(bool)
REFLECT_BASIC(char)
REFLECT_BASIC(short int)
REFLECT_BASIC(long int)
REFLECT_BASIC(long long int)
REFLECT_BASIC(unsigned char)
REFLECT_BASIC(unsigned short int)
REFLECT_BASIC(unsigned int)
REFLECT_BASIC(unsigned long int)
REFLECT_BASIC(unsigned long long int)
REFLECT_BASIC(long double)

/* Reflection metadata for common STL types. */
REFLECT_BASIC(std::string)
REFLECT_BASIC(std::vector<std::string>)
REFLECT_BASIC(std::vector<int>)
REFLECT_BASIC(std::vector<unsigned int>)
REFLECT_BASIC(std::vector<float>)
REFLECT_BASIC(std::vector<bool>)
