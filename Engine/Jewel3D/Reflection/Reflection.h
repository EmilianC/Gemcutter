// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Utilities/Meta.h"

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace Reflection
{
	//- All meta-data tags must inherit from this type.
	struct MetaTagBase
	{
		// Generates a vtable to allow for runtime type checking.
		virtual ~MetaTagBase() = default;
	};

	/* Some default Meta-Tags. */
	//- Hidden objects do not appear in the editor.
	struct Hidden : MetaTagBase {};
	//- The marked member will not be saved or loaded from file.
	struct NoSerialize : MetaTagBase {};
	//- ReadOnly members cannot be changed by the editor.
	struct ReadOnly : MetaTagBase {};
	//- Marks an enum as defining flags. It's values will no longer be mutually exclusive.
	struct BitField : MetaTagBase {};

	class Type;
	class Member;

	struct TypeTraits
	{
		const char* name;
		const unsigned nameLen;
		const unsigned nameHash;
		const unsigned size;
		const unsigned alignment;
		const bool isFundamental;
		const bool isEnum;
		const bool isAbstract;
		const bool isFinal;
		const bool isAssignable;
		const bool isIntegral;
		const bool isFloatingPoint;
		const bool isUnsigned;
		const bool isPolymorhpic;
		const bool isEmpty;
	};

	struct MemberTraits
	{
		const char* name;
		const unsigned nameLen;
		const unsigned offset;
		const bool isArray;
		const bool isPointer;
		const bool isConst;
		const bool isReference;
		const bool isVolatile;
		const unsigned arraySize;
		const TypeTraits* type;
	};

	namespace detail
	{
		// TypeInspectors are created using REFLECT and REFLECT_BASIC.
		template<typename T> struct TypeInspector {};

		// TypeCreator is a singleton base class that instantiates all derived TypeInspectors.
		// This gives us the opportunity to bring static reflection into runtime structures before execution of main().
		template<typename T> class TypeCreator
		{
		protected:
			static TypeInspector<T> initializer;
		};
		template<typename T> TypeInspector<T> TypeCreator<T>::initializer;

		// These help keep track of which collectors have been inherited from.
		struct BaseCollectorTag {};
		struct MemberCollectorTag {};
		struct EnumValueCollectorTag {};

		template<typename... MetaTagPack>
		struct MetaTagCollector
		{
			static_assert(Jwl::Meta::all_of_v<std::is_default_constructible<MetaTagPack>::value...>, "MetaTag must be default constructible.");
			static_assert(Jwl::Meta::all_of_v<std::is_base_of<MetaTagBase, MetaTagPack>::value...>, "MetaTag must inherit from MetaTagBase.");

			static void AddMetaTags(Type& t)
			{
				(t.AddMetaTag<MetaTagPack>(), ...);
			}

			static void AddMetaTags(Member& m)
			{
				(m.AddMetaTag<MetaTagPack>(), ...);
			}
		};

		template<typename... BasePack>
		struct BaseCollector : BaseCollectorTag
		{
			static void AddBases(Type& t)
			{
				(t.AddBase<BasePack>(), ...);
			}
		};

		template<typename Type, typename Name, unsigned offset>
		struct MemberInspector
		{
			using type = Type;
			using decayType = std::decay_t<type>;
			using coreType = std::remove_cv_t<std::remove_pointer_t<decayType>>;

			//- Use of an internal object allows us to expose consistent syntax through the macros.
			template<typename... MetaTags>
			struct Internal
			{
				using MetaTagCollector = MetaTagCollector<MetaTags...>;

				static constexpr MemberTraits Traits = MemberTraits {
					Name::c_str()
					, Name::length()
					, offset
					, std::is_array_v<type>
					, std::is_pointer_v<decayType>
					, std::is_const_v<type>
					, std::is_reference_v<type>
					, std::is_volatile_v<type>
					, sizeof(type) / sizeof(coreType)
					, &TypeInspector<coreType>::Internal::Traits
				};
			};
		};

		template<typename... MemberPack>
		struct MemberCollector : MemberCollectorTag
		{
			static void AddMembers(Type& t)
			{
				(t.AddMember<MemberPack>(), ...);
			}
		};

		template<typename StringName, unsigned Value>
		struct EnumValue
		{
			using Name = StringName;
			static constexpr unsigned value = Value;
		};

		template<typename... EnumValues>
		struct EnumValueCollector : EnumValueCollectorTag
		{
			static void AddEnumValues(Type& t)
			{
				(t.AddEnumValue<EnumValues>(), ...);
			}
		};

		//- Provides a common base type so that a setter can be queried.
		struct Setter_Impl : MetaTagBase
		{
			virtual void set(void* instance, void* data) const = 0;
		};

		//- Provides a common base type so that a getter can be queried.
		struct Getter_Impl : MetaTagBase
		{
			virtual void* get(void* instance) const = 0;
		};
	}

	//- Registers a custom setter function for a class member.
	template<typename Class, typename Type, typename void (Class::*setFunc)(Type)>
	struct Setter final : detail::Setter_Impl
	{
		virtual void set(void* instance, void* data) const final override
		{
			(static_cast<Class*>(instance)->*setFunc)(*reinterpret_cast<Type*>(data));
		}
	};

	//- Registers a custom getter function for a class member.
	template<typename Class, typename Type, typename Type& (Class::*getFunc)()>
	struct Getter final : detail::Getter_Impl
	{
		virtual void* get(void* instance) const final override
		{
			return reinterpret_cast<void*>(&(static_cast<Class*>(instance)->*getFunc)());
		}
	};

	//- Allows access to reflected information of a C++ type's member.
	class Member
	{
		template<typename...> friend struct detail::MetaTagCollector;
	public:
		Member(const MemberTraits& traits);

		//- The reflected properties of the member.
		const MemberTraits& traits;

		//- Returns true if the Tag exists on the member.
		template<typename MetaTag>
		bool HasMetaTag() const;

		//- Returns the specified Tag, if it exists.
		template<typename MetaTag>
		const MetaTag* GetMetaTag() const;

		//- Gets the reflected type of the member.
		const Type* GetType() const;

		//- Uses the Setter, if one exists. Otherwise writes the value with the raw offset.
		template<typename ClassType, typename MemberType>
		void set(ClassType& instance, MemberType& value);

		//- Uses the Getter, if one exists. Otherwise reads the value with the raw offset.
		template<typename MemberType, typename ClassType>
		MemberType& get(ClassType& instance);

		const auto& getMetaTags() const { return metaTags; }

	private:
		template<typename MetaTag>
		void AddMetaTag();

		std::vector<const MetaTagBase*> metaTags;
	};

	//- Allows access to reflected information of a C++ type.
	class Type
	{
		template<typename> friend struct detail::TypeInspector;
		template<typename...> friend struct detail::BaseCollector;
		template<typename...> friend struct detail::MemberCollector;
		template<typename...> friend struct detail::MetaTagCollector;
		template<typename...> friend struct detail::EnumValueCollector;
	public:
		Type(const TypeTraits& traits);

		//- The reflected properties of the type.
		const TypeTraits& traits;

		//- Returns true if the specified type is derived from this type.
		bool IsBaseOf(const Type& derived) const;

		//- Returns true if this type is derived from the specified type.
		bool IsDerivedFrom(const Type& base) const;

		//- Returns true if the Tag exists on the type.
		template<typename MetaTag>
		bool HasMetaTag() const;

		//- Returns the specified Tag, if it exists.
		template<typename MetaTag>
		const MetaTag* GetMetaTag() const;

		//- Fetches a member by name. Asserts on failure.
		const Member& operator[](const std::string&) const;
		Member& operator[](const std::string&);

		//- Returns the reflection data for the specified type, if it exists.
		static Type* Find(const char* name);

		//- Returns the reflection data for the specified type, if it exists.
		template<typename type>
		static Type* Find();

		//- Returns the map of all reflected types and their names.
		static const auto& GetGlobalTypeMap() { return GetTypeMap(); }

		const auto& GetMetaTags() const { return metaTags; }
		const auto& GetBaseClasses() const { return baseClasses; }
		const auto& GetMembers() const { return members; }
		const auto& GetEnumValues() const { return enumValues; }

	private:
		template<typename type>
		static Type& Create();

		template<typename MetaTag>
		void AddMetaTag();

		template<typename Base>
		void AddBase();

		template<typename MemberInspector>
		void AddMember();

		template<typename EnumValue>
		void AddEnumValue();

		std::vector<const MetaTagBase*> metaTags;
		std::vector<const Type*> baseClasses;
		std::map<std::string, Member> members;
		std::map<std::string, unsigned> enumValues;

		static std::unordered_map<std::string, Type>& GetTypeMap();
	};

	//- Returns a string of all reflected types.
	//- MetaTags are not stringified.
	std::string GetLog();

	static_assert(std::is_move_constructible_v<Type>, "Type should be move-constructible for performance reasons.");
	static_assert(std::is_move_constructible_v<Member>, "Member should be move-constructible for performance reasons.");
}

//- Allows the reflection system to inspect elements in the private section of a class.
#define REFLECT_PRIVATE \
	template<typename, typename, unsigned> friend struct Reflection::detail::MemberInspector; \
	template<typename> friend struct Reflection::detail::TypeInspector;

//- One macro for quick and easy reflection. Useful for fundamental types or when you don't need tags/bases/members.
#define REFLECT_BASIC(Class) \
	namespace Reflection::detail {																			\
		template<> struct TypeInspector<Class> : TypeCreator<Class>											\
		{																									\
			static_assert(std::is_same_v<Class, std::decay_t<Class>>, "Must provide non-decorated type.");	\
			static_assert(!std::is_union_v<Class>, "Cannot reflect unions.");								\
			static_assert(std::is_default_constructible_v<Class>,											\
				"Objects reflected with REFLECT_BASIC must be default constructible.");						\
			TypeInspector() {																				\
				/* Force static to be compiled-in by referencing itself. */									\
				(void)(TypeCreator<Class>::initializer);													\
				/* Fetch constexpr data and move it to our runtime structures. */							\
				Type::Create<Class>();																		\
			}																								\
			struct Internal																					\
			{																								\
				static constexpr TypeTraits Traits = TypeTraits {											\
					#Class																					\
					, sizeof(#Class)																		\
					, Jwl::Meta::HashCRC(#Class)															\
					, sizeof(Class)																			\
					, std::alignment_of_v<Class>															\
					, std::is_fundamental_v<Class>															\
					, false																					\
					, std::is_abstract_v<Class>																\
					, std::is_final_v<Class>																\
					, std::is_assignable_v<Class, Class>													\
					, std::is_integral_v<Class>																\
					, std::is_floating_point_v<Class>														\
					, std::is_unsigned_v<Class>																\
					, std::is_polymorphic_v<Class>															\
					, std::is_empty_v<Class>																\
				};																							\
			};																								\
		};																									\
	}

//- The main entry point for reflection.
#define REFLECT(Class) \
	namespace Reflection::detail {																			\
		template<> struct TypeInspector<Class> : TypeCreator<Class>											\
		{																									\
			static_assert(std::is_same_v<Class, std::decay_t<Class>>, "Must provide non-decorated type.");	\
			static_assert(!std::is_union_v<Class>, "Cannot reflect unions.");								\
			/* These hold names and types so that we don't have to respecify them for each macro. */		\
			using ClassType = Class;																		\
			using Name = STRING_48(#Class);																	\
			static constexpr unsigned NameHash = Jwl::Meta::HashCRC(#Class);								\
			/* The main reflection starts here. */															\
			struct Internal : MetaTagCollector

//- Opens a new reflection section for collecting base classes.
#define BASES Reflection::detail::BaseCollector
//- Opens a new reflection section for collecting the members of a class.
#define MEMBERS Reflection::detail::MemberCollector
//- Opens a new reflection section for collecting the values of an enum.
#define VALUES Reflection::detail::EnumValueCollector
//- Gathers information for a class member.
#define REF_MEMBER(Member) Reflection::detail::MemberInspector<decltype(ClassType::Member), STRING_48(#Member), offsetof(ClassType, Member)>::Internal
//- Gathers information for an enum value.
#define REF_VALUE(val) Reflection::detail::EnumValue<STRING_48(#val), static_cast<unsigned>(ClassType::val)>

//- Required when you are finished using reflection.
#define REF_END \
			{																				\
				template<typename = ClassType>												\
				static void AddData() {														\
					/* Fetch constexpr data and move it to our runtime structures. */		\
					auto& type = Type::Create<ClassType>();									\
					AddMetaTags(type);														\
					if constexpr (std::is_base_of_v<BaseCollectorTag, Internal>)			\
						AddBases(type);														\
					if constexpr (std::is_base_of_v<MemberCollectorTag, Internal>)			\
						AddMembers(type);													\
					if constexpr (std::is_base_of_v<EnumValueCollectorTag, Internal>)		\
						AddEnumValues(type);												\
				}																			\
				static constexpr TypeTraits Traits = TypeTraits {							\
					Name::c_str()															\
					, Name::length()														\
					, NameHash																\
					, sizeof(ClassType)														\
					, std::alignment_of_v<ClassType>										\
					, std::is_fundamental_v<ClassType>										\
					, std::is_enum_v<ClassType>												\
					, std::is_abstract_v<ClassType>											\
					, std::is_final_v<ClassType>											\
					, std::is_assignable_v<ClassType, ClassType>							\
					, std::is_integral_v<ClassType>											\
					, std::is_floating_point_v<ClassType>									\
					, std::is_unsigned_v<ClassType>											\
					, std::is_polymorphic_v<ClassType>										\
					, std::is_empty_v<ClassType>											\
				};																			\
			};																				\
			TypeInspector() {																\
				/* Force static to be compiled-in by referencing itself. */					\
				(void)(TypeCreator<ClassType>::initializer);								\
				Internal::AddData();														\
			}																				\
		};																					\
	}

#include "Reflection.inl"
