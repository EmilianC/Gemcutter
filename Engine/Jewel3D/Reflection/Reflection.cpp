// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Reflection.h"
#include "Jewel3D/Utilities/String.h"
#include "Jewel3D/Application/Logging.h"

namespace Reflection
{
	Member::Member(const MemberTraits& _traits)
		: traits(_traits)
	{
	}

	const Type* Member::GetType() const
	{
		return traits.type ? Type::Find(traits.type->name) : nullptr;
	}

	Type::Type(const TypeTraits& _traits)
		: traits(_traits)
	{
	}

	Type* Type::Find(const char* name)
	{
		auto& typeMap = GetTypeMap();

		auto itr = typeMap.find(name);
		if (itr != typeMap.end())
		{
			return &itr->second;
		}
		else
		{
			return nullptr;
		}
	};

	const Member& Type::operator[](const std::string& key) const
	{
		auto itr = members.find(key);
		
		ASSERT(itr != members.end(), "Specified member name does not exist.");
		return itr->second;
	}

	Member& Type::operator[](const std::string& key)
	{
		auto itr = members.find(key);

		ASSERT(itr != members.end(), "Specified member name does not exist.");
		return itr->second;
	}

	std::unordered_map<std::string, Type>& Type::GetTypeMap()
	{
		static std::unordered_map<std::string, Type> map;
		return map;
	}

	bool Type::IsBaseOf(const Type& derived) const
	{
		return derived.IsDerivedFrom(*this);
	}

	bool Type::IsDerivedFrom(const Type& base) const
	{
		// We follow the same rules as std::is_base_of<>, so we are our own base.
		if (this == &base)
		{
			return true;
		}

		for (auto baseClass : baseClasses)
		{
			if (baseClass == &base || baseClass->IsDerivedFrom(base))
			{
				return true;
			}
		}

		return false;
	}

	std::string GetLog()
	{
		std::string result;

		for (const auto& type : Type::GetGlobalTypeMap())
		{
			if (type.second.traits.isEnum)
			{
				result += "enum " + type.first + Jwl::FormatString(" [0x%X]\n{\n", type.second.traits.nameHash);

				for (auto val : type.second.GetEnumValues())
				{
					result += '\t' + val.first + " \t= " + std::to_string(val.second) + ",\n";
				}

				result += "}\n\n";
				continue;
			}
			else if (type.second.traits.isFundamental)
			{
				continue;
			}

			if (type.second.traits.isAbstract)
			{
				result += "abstract ";
			}

			if (type.second.traits.isFinal)
			{
				result += "final ";
			}

			result += "class " + type.first + Jwl::FormatString(" [0x%X]", type.second.traits.nameHash);

			for (unsigned i = 0; i < type.second.GetBaseClasses().size(); i++)
			{
				if (i == 0)
				{
					result += " : ";
				}
				else
				{
					result += ", ";
				}

				result += type.second.GetBaseClasses()[i]->traits.name;
			}

			if (type.second.GetMembers().empty())
			{
				result += " {}\n\n";
			}
			else
			{
				result += "\n{\n";

				for (auto& pair : type.second.GetMembers())
				{
					auto& member = pair.second;

					result += "\t" + std::to_string(member.traits.offset) + "\t-> ";

					if (member.traits.isConst) result += "const ";
					if (member.traits.isVolatile) result += "volatile ";

					result += member.traits.type->name;

					if (member.traits.isPointer && !member.traits.isArray)
					{
						result += '*';
					}

					result += ' ';
					result += member.traits.name;

					if (member.traits.isArray)
					{
						result += "[" + std::to_string(member.traits.arraySize) + "]";
					}

					result += '\n';
				}

				result += "}\n\n";
			}
		}

		return result;
	}
}
