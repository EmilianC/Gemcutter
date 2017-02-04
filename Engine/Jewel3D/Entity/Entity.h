// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Math/Matrix.h"
#include "Jewel3D/Math/Transform.h"
#include "Jewel3D/Reflection/Reflection.h"
#include "Jewel3D/Utilities/Hierarchy.h"
#include "Jewel3D/Utilities/Meta.h"

#include <iterator>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace Reflection
{
	//- Marks a class as being a component which can be attached to an entity.
	struct Component : MetaTagBase {};
	struct Tag : MetaTagBase {};
}

namespace Jwl
{
	class Entity;

	class ComponentBase
	{
		friend Entity;
		REFLECT_PRIVATE;
	public:
		ComponentBase() = delete;
		ComponentBase(const ComponentBase&) = delete;
		ComponentBase(Entity& owner, unsigned componentId);
		ComponentBase& operator=(const ComponentBase&);
		virtual ~ComponentBase() = default;

		//- Returns true if the component and its owner are both enabled and visible to queries.
		//- Disabled components are still retrievable through entity.Get<>() or entity.Try<>().
		bool IsEnabled() const;
		//- Returns true if the component itself is enabled, regardless of the owner's state.
		bool IsComponentEnabled() const;

		//- The Entity to which this component is attached.
		Entity& owner;
		//- The unique ID used by the derived component.
		const unsigned componentId;

	protected:
		//- Called when the component is removed from queries.
		virtual void OnDisable() {};
		//- Called when the component is added from queries.
		virtual void OnEnable() {};

		//- Consumes a new component-Id. Used statically by derived components.
		static unsigned GenerateID();

	private:
		virtual void Copy(Entity& newOwner) const = 0;

		bool isEnabled = true;
	};

	//- For serialization, all components must be constructible with just an Entity reference.
	//- Derive from this to create a new component. Your class should pass itself as the template:
	//	class NewComponent : public Component<NewComponent> { /* */ };
	template<class derived>
	class Component : public ComponentBase
	{
	public:
		using StaticComponentType = derived;

		Component(Entity& owner);
		virtual ~Component() = default;

		//- Returns the unique Id for the component.
		static unsigned GetComponentId();

	private:
		virtual void Copy(Entity& newOwner) const final override;

		//- A unique Id given to each component type.
		static unsigned componentId;
	};

	template<class derived> unsigned Component<derived>::componentId = ComponentBase::GenerateID();

	struct TagBase {};
	//- Base class for all tags. Cannot be instantiated.
	template<class derived> class Tag : public Component<derived>, TagBase {};

	//- An Entity is a container for Components.
	//- This is the primary object representing an element of a scene.
	//- All Entities must be created through Entity::MakeNew().
	class Entity : public Hierarchy<Entity>, public Transform
	{
		REFLECT_PRIVATE;
		friend ShareableAlloc;

		Entity() = default;
		Entity(const std::string& name);
		Entity(const Transform& pose);

		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;
		
	public:
		~Entity();

		//- Returns a pointer to the new Component.
		template<class T, typename... Args>
		T& Add(Args&&... constructorParams);

		//- Adds all specified Components to the entity, default constructed.
		template<class T, typename... Args>
		void AddComponents();

		//- Adds the specified component if it doesn't already exist and returns a pointer to it.
		template<class T>
		T& Require();

		//- Adds the specified components if any don't already exist.
		template<class T, typename... Args>
		void RequireComponents();

		//- Returns the requested component. Asserts if the component does not exist.
		template<class T>
		T& Get() const;

		//- Attempt to fetch a component. Returns null if component does not exist.
		template<class T>
		T* Try() const;

		//- Removes the instance of the specified component from this Entity if one exists.
		template<class T>
		void RemoveComponent();

		//- Removes all components from the entity.
		void RemoveAllComponents();

		//- Returns true if the component exists on this Entity.
		template<class T>
		bool Has() const;

		//- Adds the specified tag ID if the Entity doesn't already have it.
		template<class T>
		void Tag();

		//- Removes the specified tag ID from the Entity.
		template<class T>
		void RemoveTag();

		//- Returns true if the tag ID exists on the Entity.
		template<class T>
		bool HasTag() const;

		//- Removes all Tags from the entity.
		void RemoveAllTags();

		//- Enabling this Entity is equivalent to individually
		void Enable();

		//- Disabling this Entity is equivalent to individually disabling all its components.
		void Disable();

		//- Enables the specific component. It will be made visible to queries if this entity is also enabled.
		template<class T>
		void Enable();

		//- Disables the specific component. It will no longer be visible to queries.
		template<class T>
		void Disable();

		//- Whether or not this Entity is visible to queries.
		bool IsEnabled() const;

		//- Creates and returns a new child entity.
		Entity::Ptr CreateChild();

		//- Creates and returns a new Entity that is a copy of this one.
		Entity::Ptr Duplicate() const;

		//- Copies the component onto this Entity.
		//- It is added if an instance does not already exist on this Entity.
		void CopyComponent(const ComponentBase& source);

		//- Returns the true transformation of the Entity, accumulated from the root of the hierarchy.
		mat4 GetWorldTransform() const;

		//- Positions the Entity at 'pos' looking towards the 'target' in local space.
		void LookAt(const vec3& pos, const vec3& target, const vec3& up = vec3::Y);

		//- Orients the Entity to face the target, with respect to hierarchy.
		void LookAt(const Entity& target, const vec3& up = vec3::Y);

	private:
		void Tag(unsigned tagId);
		void RemoveTag(unsigned tagId);

		void IndexTag(unsigned tagId);
		void UnindexTag(unsigned tagId);

		void Index(ComponentBase& comp);
		void Unindex(ComponentBase& comp);

		std::vector<ComponentBase*> components;
		std::vector<unsigned> tags;

		bool isEnabled = true;
	};
}

#include "Query.inl"
#include "Entity.inl"

REFLECT(Jwl::ComponentBase)<>,
	MEMBERS <
		REF_MEMBER(isEnabled)<>
	>
REF_END;

REFLECT_SHAREABLE(Jwl::Entity)
REFLECT_BASIC(std::vector<Jwl::ComponentBase*>)
REFLECT_BASIC(Jwl::Hierarchy<Jwl::Entity>)

REFLECT(Jwl::Entity)<>,
	BASES <
		Jwl::Hierarchy<Jwl::Entity>,
		Jwl::Transform
	>,
	MEMBERS <
		REF_MEMBER(isEnabled)<>,
		REF_MEMBER(components)<>,
		REF_MEMBER(tags)<>
	>
REF_END;

#define REFLECT_TAG(Class) \
	static_assert(std::is_base_of<Jwl::TagBase, Class>::value, "REFLECT_TAG must be used on a Tag."); \
	REFLECT(Class)< Tag > \
	REF_END;
