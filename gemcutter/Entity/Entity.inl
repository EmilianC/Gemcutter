// Copyright (c) 2017 Emilian Cioca
namespace gem
{
	namespace detail
	{
		// Allows for compile-time decision of whether or not a dynamic cast is required.
		// A dynamic cast is required when a class inherits from Component indirectly, and
		// as such, doesn't compile its own unique component type ID.
		template<class T>
		T* safe_cast(ComponentBase* comp)
		{
			if constexpr (std::is_same_v<T, typename T::StaticComponentType>)
			{
				return static_cast<T*>(comp);
			}
			else
			{
				return dynamic_cast<T*>(comp);
			}
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

	template<class T, typename... Args>
	T& Entity::Add(Args&&... constructorParams)
	{
		static_assert(std::is_base_of_v<ComponentBase, T>, "Template argument must inherit from Component.");
		static_assert(!std::is_base_of_v<TagBase, T>, "Template argument cannot be a Tag.");
		ASSERT(!Has<T>(), "Component already exists on this entity.");

		auto* newComponent = new T(*this, std::forward<Args>(constructorParams)...);
		components.push_back(newComponent);

		if (IsEnabled())
		{
			Index(*newComponent);
		}

		return *newComponent;
	}

	template<class T1, class T2, typename... Tx>
	std::tuple<T1&, T2&, Tx&...> Entity::Add()
	{
		return std::tie(Add<T1>(), Add<T2>(), Add<Tx>()...);
	}

	template<class T>
	T& Entity::Require()
	{
		auto* comp = Try<T>();
		return comp ? *comp : Add<T>();
	}

	template<class T1, class T2, typename... Tx>
	std::tuple<T1&, T2&, Tx&...> Entity::Require()
	{
		return std::tie(Require<T1>(), Require<T2>(), Require<Tx>()...);
	}

	template<class T> const T& Entity::Get() const { return GetComponent<T>(); }
	template<class T>       T& Entity::Get()       { return GetComponent<T>(); }

	template<class T> const T* Entity::Try() const { return TryComponent<T>(); }
	template<class T>       T* Entity::Try()       { return TryComponent<T>(); }

	template<class T>
	void Entity::Remove()
	{
		using namespace detail;
		static_assert(std::is_base_of_v<ComponentBase, T>, "Template argument must inherit from Component.");
		static_assert(!std::is_base_of_v<TagBase, T>, "Template argument cannot be a Tag.");

		for (unsigned i = 0; i < components.size(); ++i)
		{
			if (components[i]->componentId == T::GetComponentId())
			{
				if (safe_cast<T>(components[i]))
				{
					auto* comp = components[i];
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
		static_assert(std::is_base_of_v<ComponentBase, T>, "Template argument must inherit from Component.");
		static_assert(!std::is_base_of_v<TagBase, T>, "Template argument cannot be a Tag.");

		return Try<T>() != nullptr;
	}

	template<class T, typename... Args>
	void Entity::Tag()
	{
		if constexpr (sizeof...(Args))
		{
			Tag<T>();
			Tag<Args...>();
		}
		else
		{
			static_assert(std::is_base_of_v<TagBase, T>, "Template argument must inherit from Tag.");

			if (!HasTag<T>())
			{
				Tag(T::GetComponentId());
			}
		}
	}

	template<class T>
	void Entity::RemoveTag()
	{
		static_assert(std::is_base_of_v<TagBase, T>, "Template argument must inherit from Tag.");

		RemoveTag(T::GetComponentId());
	}

	template<class T>
	bool Entity::HasTag() const
	{
		static_assert(std::is_base_of_v<TagBase, T>, "Template argument must inherit from Tag.");

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
	void Entity::GlobalRemoveTag()
	{
		static_assert(std::is_base_of_v<TagBase, T>, "Template argument must inherit from Tag.");

		std::vector<Entity*>& taggedEntities = detail::entityIndex[T::GetComponentId()];
		for (Entity* ent : taggedEntities)
		{
			auto& tags = ent->tags;

			auto itr = std::find(tags.begin(), tags.end(), T::GetComponentId());
			*itr = tags.back();
			tags.pop_back();
		}

		taggedEntities.clear();
	}

	template<class T>
	void Entity::Enable()
	{
		static_assert(std::is_base_of_v<ComponentBase, T>, "Template argument must inherit from Component.");
		static_assert(!std::is_base_of_v<TagBase, T>, "Tags cannot be enabled or disabled. Add or remove them instead.");

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
		static_assert(std::is_base_of_v<ComponentBase, T>, "Template argument must inherit from Component.");
		static_assert(!std::is_base_of_v<TagBase, T>, "Tags cannot be enabled or disabled. Add or remove them instead.");

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

	template<class T>
	T& Entity::GetComponent() const
	{
		using namespace detail;
		static_assert(std::is_base_of_v<ComponentBase, T>, "Template argument must inherit from Component.");
		static_assert(!std::is_base_of_v<TagBase, T>, "Template argument cannot be a Tag.");

		auto itr = components.begin();
		while (true)
		{
			ASSERT(itr != components.end(), "Entity did not have the expected component.");
			if ((*itr)->componentId == T::GetComponentId())
			{
				ASSERT(safe_cast<T>(*itr), "Entity did not have the expected component.");
				return *static_cast<T*>(*itr);
			}

			++itr;
		}
	}

	template<class T>
	T* Entity::TryComponent() const
	{
		using namespace detail;
		static_assert(std::is_base_of_v<ComponentBase, T>, "Template argument must inherit from Component.");
		static_assert(!std::is_base_of_v<TagBase, T>, "Template argument cannot be a Tag.");

		for (auto* comp : components)
		{
			if (comp->componentId == T::GetComponentId())
			{
				return safe_cast<T>(comp);
			}
		}

		return nullptr;
	}
}
