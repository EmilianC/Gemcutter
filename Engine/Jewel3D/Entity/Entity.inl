// Copyright (c) 2017 Emilian Cioca
namespace Jwl
{
	namespace detail
	{
		template<class T, class U>
		struct _safe_cast
		{
			static T* Cast(ComponentBase* comp) { return dynamic_cast<T*>(comp); }
		};

		template<class T>
		struct _safe_cast<T, T>
		{
			static T* Cast(ComponentBase* comp) { return static_cast<T*>(comp); }
		};

		// Allows for compile-time decision of whether or not a dynamic cast is required.
		// A dynamic cast is required when a class inherits from Component indirectly, and
		// as such, doesn't compile its own unique component type ID.
		template<class T>
		T* safe_cast(ComponentBase* comp)
		{
			static_assert(std::is_base_of<ComponentBase, T>::value, "Template argument must inherit from Component.");
			static_assert(!std::is_base_of<TagBase, T>::value, "Template argument cannot be a Tag");

			return _safe_cast<T, typename T::StaticComponentType>::Cast(comp);
		}

		// Copies a component using direct assignment. Compiles to a warning if direct assignment is invalid.
		template<class Component>
		std::enable_if_t<!std::is_base_of<TagBase, Component>::value && std::is_copy_assignable<Component>::value>
		copy_component(Entity& destination, const Component& source)
		{
			auto& dest = destination.Require<Component>();
			dest = source;

			if (!source.IsEnabled())
			{
				destination.Disable<Component>();
			}
		}

		// Copies a component using direct assignment. Compiles to a warning if direct assignment is invalid.
		template<class Component>
		std::enable_if_t<!std::is_base_of<TagBase, Component>::value && !std::is_copy_assignable<Component>::value>
		copy_component(Entity& destination, const Component& source)
		{
			Warning("Component could not be copied. It does not have a valid assignment operator.");
		}

		// The copy operation is disabled for tags.
		template<class Component>
		std::enable_if_t<std::is_base_of<TagBase, Component>::value>
		copy_component(Entity& destination, const Component& source)
		{
			Error("Tag was attempted to be copied as if it was a Component.");
		}
	}

	template<class derived>
	Component<derived>::Component(Entity& owner) 
		: ComponentBase(owner, componentId)
	{}

	template<class derived>
	unsigned Component<derived>::GetComponentId()
	{
		return componentId;
	}

	template<class derived>
	void Component<derived>::Copy(Entity& newOwner) const
	{
		detail::copy_component(newOwner, *static_cast<const derived*>(this));
	}

	template<class T, typename... Args>
	T& Entity::Add(Args&&... constructorParams)
	{
		static_assert(std::is_base_of<ComponentBase, T>::value, "Template argument must inherit from Component.");
		static_assert(!std::is_base_of<TagBase, T>::value, "Template argument cannot be a Tag");
		ASSERT(!Has<T>(), "Component already exists on this entity.");

		T* newComponent = new T(*this, std::forward<Args>(constructorParams)...);
		components.push_back(newComponent);

		if (IsEnabled())
		{
			Index(*newComponent);
		}

		return *newComponent;
	}

	template<class T, typename... Args>
	void Entity::AddComponents()
	{
		ComponentBase* ptr[] =
		{
			&Add<T>(),
			&Add<Args>()...
		};
	}

	template<class T>
	T& Entity::Require()
	{
		static_assert(std::is_base_of<ComponentBase, T>::value, "Template argument must inherit from Component.");
		static_assert(!std::is_base_of<TagBase, T>::value, "Template argument cannot be a Tag");

		if (T* comp = Try<T>())
		{
			return *comp;
		}
		else
		{
			return Add<T>();
		}
	}

	template<class T, typename... Args>
	void Entity::RequireComponents()
	{
		ComponentBase* ptr[] =
		{
			&Require<T>(),
			&Require<Args>()...
		};
	}

	template<class T>
	T& Entity::Get() const
	{
		using namespace detail;
		static_assert(std::is_base_of<ComponentBase, T>::value, "Template argument must inherit from Component.");
		static_assert(!std::is_base_of<TagBase, T>::value, "Template argument cannot be a Tag");

		auto itr = components.begin();
		for (; itr != components.end(); ++itr)
		{
			if ((*itr)->componentId == T::GetComponentId())
			{
				ASSERT(safe_cast<T>(*itr), "Entity did not have the expected component.");
				break;
			}
		}

		ASSERT(itr != components.end(), "Entity did not have the expected component.");
		return *static_cast<T*>(*itr);
	}

	template<class T>
	T* Entity::Try() const
	{
		using namespace detail;
		static_assert(std::is_base_of<ComponentBase, T>::value, "Template argument must inherit from Component.");
		static_assert(!std::is_base_of<TagBase, T>::value, "Template argument cannot be a Tag");

		for (auto comp : components)
		{
			if (comp->componentId == T::GetComponentId())
			{
				return safe_cast<T>(comp);
			}
		}

		return nullptr;
	}

	template<class T>
	void Entity::RemoveComponent()
	{
		using namespace detail;
		static_assert(std::is_base_of<ComponentBase, T>::value, "Template argument must inherit from Component.");
		static_assert(!std::is_base_of<TagBase, T>::value, "Template argument cannot be a Tag");

		for (unsigned i = 0; i < components.size(); ++i)
		{
			if (components[i]->componentId == T::GetComponentId())
			{
				if (safe_cast<T>(components[i]))
				{
					auto comp = components[i];
					components.erase(components.begin() + i);

					if (comp->IsEnabled())
					{
						Unindex(*comp);
					}

					delete comp;
				}

				return;
			}
		}
	}

	template<class T>
	bool Entity::Has() const
	{
		static_assert(std::is_base_of<ComponentBase, T>::value, "Template argument must inherit from Component.");
		static_assert(!std::is_base_of<TagBase, T>::value, "Template argument cannot be a Tag");

		return Try<T>() != nullptr;
	}

	template<class T>
	void Entity::Tag()
	{
		static_assert(std::is_base_of<TagBase, T>::value, "Template argument must inherit from Tag.");
		if (HasTag<T>())
		{
			return;
		}

		Tag(T::GetComponentId());
	}

	template<class T>
	void Entity::RemoveTag()
	{
		static_assert(std::is_base_of<TagBase, T>::value, "Template argument must inherit from Tag.");

		RemoveTag(T::GetComponentId());
	}

	template<class T>
	bool Entity::HasTag() const
	{
		static_assert(std::is_base_of<TagBase, T>::value, "Template argument must inherit from Tag.");

		for (auto tag : tags)
		{
			if (tag == T::GetComponentId())
			{
				return true;
			}
		}

		return false;
	}

	template<class T>
	void Entity::Enable()
	{
		static_assert(std::is_base_of<ComponentBase, T>::value, "Template argument must inherit from Component.");
		static_assert(!std::is_base_of<TagBase, T>::value, "Tags cannot be enabled or disabled. Add or remove them instead.");
		
		auto& comp = Get<T>();

		// The component state changes either way, but it only gets indexed if the Entity is also enabled.
		bool wasEnabled = comp.isEnabled;
		comp.isEnabled = true;
		
		if (!this->IsEnabled())
		{
			return;
		}

		if (!wasEnabled)
		{
			Index(comp);
			static_cast<ComponentBase&>(comp).OnEnable();
		}
	}

	template<class T>
	void Entity::Disable()
	{
		static_assert(std::is_base_of<ComponentBase, T>::value, "Template argument must inherit from Component.");
		static_assert(!std::is_base_of<TagBase, T>::value, "Tags cannot be enabled or disabled. Add or remove them instead.");
		
		auto& comp = Get<T>();

		// The component state changes either way, but it only gets removed if the Entity is enabled.
		bool wasEnabled = comp.isEnabled;
		comp.isEnabled = false;

		if (!this->IsEnabled())
		{
			return;
		}

		if (wasEnabled)
		{
			Unindex(comp);
			static_cast<ComponentBase&>(comp).OnDisable();
		}
	}
}
