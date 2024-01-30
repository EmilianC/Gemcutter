// Copyright (c) 2017 Emilian Cioca
namespace gem
{
	template<class T>
	bool ComponentBase::IsA() const
	{
		static_assert(std::is_base_of_v<ComponentBase, T>, "Template argument must inherit from ComponentBase.");
		static_assert(!std::is_base_of_v<TagBase, T>, "Template argument cannot be a Tag.");

		return typeId->is_a(ReflectType<T>());
	}

	template<class To> [[nodiscard]]
	auto component_cast(ComponentBase* from)
	{
		using TargetComponent = std::remove_pointer_t<To>;

		static_assert(std::is_pointer_v<To>, "component_cast must cast to a pointer type.");
		static_assert(std::is_base_of_v<ComponentBase, TargetComponent>, "component_cast must cast to a Component type.");

		ASSERT(from != nullptr, "component_cast cannot cast a null Component pointer.");

		if constexpr (std::is_same_v<TargetComponent, typename TargetComponent::StaticComponentType>)
		{
			if (from->componentId == TargetComponent::staticComponentId)
			{
				return static_cast<To>(from);
			}
		}
		else
		{
			if (from->IsA<TargetComponent>())
			{
				return static_cast<To>(from);
			}
		}

		return static_cast<To>(nullptr);
	}

	template<class To> [[nodiscard]]
	auto component_cast(const ComponentBase* from)
	{
		return const_cast<const To>(component_cast<To>(const_cast<ComponentBase*>(from)));
	}

	template<class derived>
	Component<derived>::Component(Entity& owner)
		: ComponentBase(owner, staticComponentId)
	{
	}

	template<class T, typename... Args>
	T& Entity::Add(Args&&... constructorParams)
	{
		static_assert(std::is_base_of_v<ComponentBase, T>, "Template argument must inherit from ComponentBase.");
		static_assert(!std::is_base_of_v<TagBase, T>, "Template argument cannot be a Tag.");
		ASSERT(!Has<typename T::StaticComponentType>(), "The Component (or one sharing a hierarchy) already exists on this entity.");

		T* newComponent = static_cast<T*>(_aligned_malloc(sizeof(T), alignof(T)));
		new (newComponent) T(*this, std::forward<Args>(constructorParams)...);

		newComponent->typeId = &ReflectType<T>();
		components.push_back(newComponent);

		if (IsEnabled())
		{
			if constexpr (std::is_same_v<T, typename T::StaticComponentType>)
			{
				Index(*newComponent, *newComponent->typeId);
			}
			else
			{
				IndexWithBases(*newComponent);
			}
		}

		return *newComponent;
	}

	template<class T1, class T2, typename... Tx>
	std::tuple<T1&, T2&, Tx&...> Entity::Add()
	{
		components.reserve(components.size() + 2 + sizeof...(Tx));

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
		static_assert(std::is_base_of_v<ComponentBase, T>, "Template argument must inherit from ComponentBase.");
		static_assert(!std::is_base_of_v<TagBase, T>, "Template argument cannot be a Tag.");

		for (unsigned i = 0; i < components.size(); ++i)
		{
			auto* comp = components[i];
			if (comp->componentId == T::staticComponentId)
			{
				if (comp->IsA<T>())
				{
					components[i] = components.back();
					components.pop_back();

					if (isEnabled && comp->isEnabled)
					{
						UnindexWithBases(*comp);
					}

					static_cast<T*>(comp)->~T();
					_aligned_free(comp);
				}
				return;
			}
		}
	}

	template<typename T>
	bool Entity::Has() const
	{
		static_assert(std::is_base_of_v<ComponentBase, T>, "Template argument must inherit from either ComponentBase or Tag.");

		if constexpr (std::is_base_of_v<TagBase, T>)
		{
			for (detail::ComponentId tag : tags)
			{
				if (tag == T::staticComponentId)
				{
					return true;
				}
			}

			return false;
		}
		else
		{
			return Try<T>() != nullptr;
		}
	}

	template<typename... Args>
	void Entity::Tag()
	{
		static_assert(sizeof...(Args) >= 1, "Must have at least one template argument.");
		static_assert(meta::all_of_v<std::is_base_of_v<TagBase, Args>...>, "Template arguments must inherit from Tag.");

		( [this]() {
			if (!Has<Args>())
			{
				AddTag(Args::staticComponentId);
			}
		}(), ... );
	}

	template<class T>
	void Entity::RemoveTag()
	{
		static_assert(std::is_base_of_v<TagBase, T>, "Template argument must inherit from Tag.");

		RemoveTag(T::staticComponentId);
	}

	template<class T>
	void Entity::GlobalRemoveTag()
	{
		static_assert(std::is_base_of_v<TagBase, T>, "Template argument must inherit from Tag.");

		std::vector<Entity*>& taggedEntities = detail::tagIndex[T::staticComponentId];
		for (Entity* ent : taggedEntities)
		{
			auto& tags = ent->tags;

			auto itr = std::find(std::begin(tags), std::end(tags), T::staticComponentId);
			*itr = tags.back();
			tags.pop_back();
		}

		taggedEntities.clear();
	}

	template<class T>
	void Entity::Enable()
	{
		static_assert(std::is_base_of_v<ComponentBase, T>, "Template argument must inherit from ComponentBase.");
		static_assert(!std::is_base_of_v<TagBase, T>, "Tags cannot be enabled or disabled. Add or remove them instead.");

		Enable(Get<T>());
	}

	template<class T>
	void Entity::Disable()
	{
		static_assert(std::is_base_of_v<ComponentBase, T>, "Template argument must inherit from ComponentBase.");
		static_assert(!std::is_base_of_v<TagBase, T>, "Tags cannot be enabled or disabled. Add or remove them instead.");

		Disable(Get<T>());
	}

	template<class T>
	T& Entity::GetComponent() const
	{
		static_assert(std::is_base_of_v<ComponentBase, T>, "Template argument must inherit from ComponentBase.");
		static_assert(!std::is_base_of_v<TagBase, T>, "Template argument cannot be a Tag.");

		auto itr = components.begin();
		while (true)
		{
			ASSERT(itr != components.end(), "Entity did not have the expected component.");
			if ((*itr)->componentId == T::staticComponentId)
			{
				ASSERT((*itr)->IsA<T>(), "Entity did not have the expected component.");
				return *static_cast<T*>(*itr);
			}

			++itr;
		}
	}

	template<class T>
	T* Entity::TryComponent() const
	{
		static_assert(std::is_base_of_v<ComponentBase, T>, "Template argument must inherit from ComponentBase.");
		static_assert(!std::is_base_of_v<TagBase, T>, "Template argument cannot be a Tag.");

		for (auto* comp : components)
		{
			if (comp->componentId == T::staticComponentId)
			{
				return component_cast<T*>(comp);
			}
		}

		return nullptr;
	}
}

namespace std
{
	template<>
	struct hash<gem::detail::ComponentId>
	{
		size_t operator()(const gem::detail::ComponentId& id) const noexcept
		{
			return std::hash<gem::detail::ComponentId::ValueType>{}(id.GetValue());
		}
	};
}
