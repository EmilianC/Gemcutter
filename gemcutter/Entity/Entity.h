// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Application/Reflection.h"
#include "gemcutter/Math/Matrix.h"
#include "gemcutter/Math/Transform.h"
#include "gemcutter/Resource/Shareable.h"
#include "gemcutter/Utilities/Identifier.h"
#include "gemcutter/Utilities/Meta.h"

#include <span>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace gem
{
	namespace detail { struct ComponentId : public Identifier<short> {}; }

	class ComponentBase
	{
		friend class Entity;
	public:
		ComponentBase(const ComponentBase&) = delete;
		ComponentBase(Entity& owner, detail::ComponentId);
		ComponentBase& operator=(const ComponentBase&) = delete;
		virtual ~ComponentBase() = default;

		// Returns true if the component and its owner are both enabled and visible to queries.
		// Disabled components are still retrievable through entity.Get<>() or entity.Try<>().
		bool IsEnabled() const;
		// Returns true if the component itself is enabled, regardless of the owning Entity's state.
		bool IsComponentEnabled() const;

		// Returns the full type information reflected for the component.
		const loupe::type& GetType() const;

		// Returns true if this component is the given type, or derives from it.
		template<class T>
		bool IsA() const;
		bool IsA(const loupe::type& baseType) const;

		// The Entity to which this component is attached.
		Entity& owner;

	protected:
		// Called when the component is removed from queries.
		virtual void OnDisable() {}
		// Called when the component is added from queries.
		virtual void OnEnable() {}

	private:
		// The type descriptor of the instantiated component.
		const loupe::type* typeId;

		bool isEnabled = true;

		// The static Component<Derived> Id. Used to speed up casts
		// without having to walk the reflected type's hierarchy chain.
		const detail::ComponentId componentId;

	public:
		// Performs a dynamic cast to the requested type if it is valid to do so. Asserts on nullptr input.
		template<class To> friend auto component_cast(ComponentBase* from);
		template<class To> friend auto component_cast(const ComponentBase* from);

		PRIVATE_MEMBER(ComponentBase, typeId);
		PRIVATE_MEMBER(ComponentBase, isEnabled);
	};

	// Derive from this to create a new component.
	// Your class should pass itself as the template argument.
	// All components must be constructible with just an Entity reference.
	template<class derived>
	class Component : public ComponentBase
	{
	public:
		Component(Entity& owner);
		virtual ~Component() = default;

		// Used to detect if a type is a first-order decedent from Component<>.
		// This enables some optimizations in templated functions.
		using StaticComponentType = derived;

		// A lightweight ID which can represent Tags and also accelerates type-checking between components.
		static inline const detail::ComponentId staticComponentId = GenerateUniqueId<detail::ComponentId>();
	};

	struct TagBase {};
	// Base class for all tags. Cannot be instantiated.
	template<class derived> class Tag : public Component<derived>, public TagBase {};

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

		// Returns a reference to the new component.
		template<class T, typename... Args>
		T& Add(Args&&... constructorParams);

		// Returns the new specified components, default constructed.
		template<class T1, class T2, typename... Tx>
		std::tuple<T1&, T2&, Tx&...> Add();

		// Adds the specified component if it is not already present.
		template<class T>
		T& Require();

		// Adds any of the components which are not already present.
		template<class T1, class T2, typename... Tx>
		std::tuple<T1&, T2&, Tx&...> Require();

		// Returns the requested component. Asserts if the component does not exist.
		template<class T> const T& Get() const;
		template<class T>       T& Get();

		// Attempts to find the specified component. Returns null if the component does not exist.
		template<class T> const T* Try() const;
		template<class T>       T* Try();

		// Returns all the components on the Entity, regardless of whether or not they are enabled.
		std::span<const ComponentBase* const> GetAllComponents() const;
		std::span<      ComponentBase* const> GetAllComponents();

		// Removes the instance of the specified component from this Entity if one exists.
		template<class T>
		void Remove();

		// Removes all components from the Entity.
		void RemoveAllComponents();

		// Returns true if the specified component or tag exists on this Entity.
		template<class T> [[nodiscard]]
		bool Has() const;

		// Adds any of the tags which are not already present.
		template<typename... Args>
		void Tag();

		// Removes the specified tag from the Entity.
		template<class T>
		void RemoveTag();

		// Removes all tags from the Entity.
		void RemoveAllTags();

		// Efficiently removes all tags of the specified type from all Entities globally.
		template<class T>
		static void GlobalRemoveTag();

		// Enabling the Entity allows it and its enabled components/tags to be visible to queries.
		void Enable();

		// Disabling the Entity hides it and its components/tags from queries.
		void Disable();

		// Enables the specific component. It will be made visible to queries if this Entity is also enabled.
		template<class T>
		void Enable();
		void Enable(ComponentBase&);

		// Disables the specific component. It will no longer be visible to queries.
		template<class T>
		void Disable();
		void Disable(ComponentBase&);

		// Whether or not this Entity is visible to queries.
		bool IsEnabled() const;

		// Reflection API equivalent for Add<>().
		ComponentBase& Add(const loupe::type&);

		// Reflection API equivalent for Require<>().
		ComponentBase& Require(const loupe::type&);

		// Reflection API equivalent for Get<>().
		ComponentBase& Get(const loupe::type&);
		const ComponentBase& Get(const loupe::type&) const;

		// Reflection API equivalent for Try<>().
		ComponentBase* Try(const loupe::type&);
		const ComponentBase* Try(const loupe::type&) const;

		// Reflection API equivalent for Has<>() (but for components only).
		bool Has(const loupe::type&) const;

		// Reflection API equivalent for Remove<>().
		void Remove(const loupe::type&);

		// Creates and returns a new Entity with a Hierarchy component.
		[[nodiscard]] static Entity::Ptr MakeNewRoot();
		[[nodiscard]] static Entity::Ptr MakeNewRoot(std::string name);
		[[nodiscard]] static Entity::Ptr MakeNewRoot(const Transform& pose);

		// Returns the full world-space transformation of the Entity
		// accumulated from the root of the hierarchy, if there is one.
		mat4 GetWorldTransform() const;

		// Returns the world-space position of the Entity
		// accumulated from the root of the hierarchy, if there is one.
		vec3 GetWorldPosition() const;

		// Returns the world-space rotation of the Entity
		// accumulated from the root of the hierarchy, if there is one.
		quat GetWorldRotation() const;

		// Positions the Entity at 'pos' looking towards the 'target' in local-space.
		void LookAt(const vec3& pos, const vec3& target, const vec3& up = vec3::Up);

		// Orients the Entity to face the target, with respect to any hierarchy.
		void LookAt(const Entity& target, const vec3& up = vec3::Up);

	private:
		template<class T>
		T& GetComponent() const;
		ComponentBase& GetComponent(const loupe::type&) const;

		template<class T>
		T* TryComponent() const;
		ComponentBase* TryComponent(const loupe::type&) const;

		void AddTag(detail::ComponentId);
		void RemoveTag(detail::ComponentId);

		void IndexTag(detail::ComponentId);
		void UnindexTag(detail::ComponentId);

		// Indexes the component to the specified type's index table.
		void Index(ComponentBase&, const loupe::type&);
		void Unindex(const ComponentBase&, const loupe::type&);

		// Indexes the component along with all base component types.
		void IndexWithBases(ComponentBase&);
		void UnindexWithBases(const ComponentBase&);

		std::vector<ComponentBase*> components;
		std::vector<detail::ComponentId> tags;

		bool isEnabled = true;

	public:
		PRIVATE_MEMBER(Entity, components);
		PRIVATE_MEMBER(Entity, tags);
		PRIVATE_MEMBER(Entity, isEnabled);
	};

	[[nodiscard]] bool operator==(const Entity&, const Entity&);
	[[nodiscard]] bool operator==(const Entity&, const Entity::Ptr&);
	[[nodiscard]] bool operator==(const Entity::Ptr&, const Entity&);
	[[nodiscard]] bool operator!=(const Entity&, const Entity&);
	[[nodiscard]] bool operator!=(const Entity&, const Entity::Ptr&);
	[[nodiscard]] bool operator!=(const Entity::Ptr&, const Entity&);
}

#include "Query.inl"
#include "Entity.inl"
