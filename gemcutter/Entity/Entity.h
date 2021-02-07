// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Math/Matrix.h"
#include "gemcutter/Math/Transform.h"
#include "gemcutter/Resource/Shareable.h"
#include "gemcutter/Utilities/Meta.h"

#include <tuple>
#include <unordered_map>
#include <vector>

namespace Jwl
{
	class Entity;

	class ComponentBase
	{
		friend Entity;
	public:
		ComponentBase() = delete;
		ComponentBase(const ComponentBase&) = delete;
		ComponentBase(Entity& owner, unsigned componentId);
		ComponentBase& operator=(const ComponentBase&) = delete;
		virtual ~ComponentBase() = default;

		// Returns true if the component and its owner are both enabled and visible to queries.
		// Disabled components are still retrievable through entity.Get<>() or entity.Try<>().
		bool IsEnabled() const;
		// Returns true if the component itself is enabled, regardless of the owner's state.
		bool IsComponentEnabled() const;

		// The Entity to which this component is attached.
		Entity& owner;

	protected:
		// Called when the component is removed from queries.
		virtual void OnDisable() {}
		// Called when the component is added from queries.
		virtual void OnEnable() {}

		// Consumes a new component-Id. Used statically by derived components.
		static unsigned GenerateID();

	private:
		// The unique ID used by the derived component.
		const unsigned componentId;

		bool isEnabled = true;
	};

	// Derive from this to create a new component.
	// Your class should pass itself as the template argument.
	// All components must be constructible with just an Entity reference.
	template<class derived>
	class Component : public ComponentBase
	{
	public:
		using StaticComponentType = derived;

		Component(Entity& owner);
		virtual ~Component() = default;

		// Returns the unique Id for the component.
		static unsigned GetComponentId();

	private:
		// A unique Id given to each component type.
		static unsigned componentId;
	};

	template<class derived> unsigned Component<derived>::componentId = ComponentBase::GenerateID();

	struct TagBase {};
	// Base class for all tags. Cannot be instantiated.
	template<class derived> class Tag : public Component<derived>, TagBase {};

	// An Entity is a container for Components.
	// This is the primary object representing an element of a scene.
	// All Entities MUST be created through Entity::MakeNew().
	class Entity : public Transform, public Shareable<Entity>
	{
		friend ShareableAlloc;

		Entity() = default;
		Entity(std::string name);
		Entity(const Transform& pose);

	public:
		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;
		~Entity();

		// Returns a pointer to the new Component.
		template<class T, typename... Args>
		T& Add(Args&&... constructorParams);

		// Returns new specified Components, default constructed.
		template<class T1, class T2, typename... Tx>
		std::tuple<T1&, T2&, Tx&...> Add();

		// Adds the specified Component if it doesn't already exist.
		template<class T>
		T& Require();

		// Adds all the specified Components if any don't already exist.
		template<class T1, class T2, typename... Tx>
		std::tuple<T1&, T2&, Tx&...> Require();

		// Returns the requested component. Asserts if the component does not exist.
		template<class T> const T& Get() const;
		template<class T>       T& Get();

		// Attempt to fetch a component. Returns null if component does not exist.
		template<class T> const T* Try() const;
		template<class T>       T* Try();

		// Removes the instance of the specified component from this Entity if one exists.
		template<class T>
		void Remove();

		// Removes all components from the entity.
		void RemoveAllComponents();

		// Returns true if the component exists on this Entity.
		template<class T>
		bool Has() const;

		// Adds all the specified tags if any don't already exist.
		template<class T, typename... Args>
		void Tag();

		// Removes the specified tag ID from the Entity.
		template<class T>
		void RemoveTag();

		// Returns true if the tag ID exists on the Entity.
		template<class T>
		bool HasTag() const;

		// Removes all Tags from the entity.
		void RemoveAllTags();

		// Efficiently removes all tags of the given type from all entities.
		template<class T>
		static void GlobalRemoveTag();

		// Enabling this Entity is equivalent to individually
		void Enable();

		// Disabling this Entity is equivalent to individually disabling all its components.
		void Disable();

		// Enables the specific component. It will be made visible to queries if this entity is also enabled.
		template<class T>
		void Enable();

		// Disables the specific component. It will no longer be visible to queries.
		template<class T>
		void Disable();

		// Whether or not this Entity is visible to queries.
		bool IsEnabled() const;

		// Creates and returns a new Entity with a Hierarchy component.
		static Entity::Ptr MakeNewRoot();
		static Entity::Ptr MakeNewRoot(std::string name);
		static Entity::Ptr MakeNewRoot(const Transform& pose);

		// Returns the world-space transformation of the Entity,
		// accumulated from the root of the hierarchy if there is one.
		mat4 GetWorldTransform() const;

		// Returns the world-space rotation of the Entity,
		// accumulated from the root of the hierarchy if there is one.
		quat GetWorldRotation() const;

		// Positions the Entity at 'pos' looking towards the 'target' in local space.
		void LookAt(const vec3& pos, const vec3& target, const vec3& up = vec3::Up);

		// Orients the Entity to face the target, with respect to hierarchy.
		void LookAt(const Entity& target, const vec3& up = vec3::Up);

	private:
		template<class T>
		T& GetComponent() const;

		template<class T>
		T* TryComponent() const;

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
