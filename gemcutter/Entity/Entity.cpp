// Copyright (c) 2017 Emilian Cioca
#include "Entity.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Entity/Hierarchy.h"
#include "gemcutter/Entity/Name.h"

#include <algorithm>

namespace gem
{
	namespace detail
	{
		std::unordered_map<ComponentId, std::vector<Entity*>> tagIndex;
		std::unordered_map<const loupe::type*, std::vector<Entity*>> typeIndex;
		std::unordered_map<const loupe::type*, std::vector<ComponentBase*>> componentLists;
	}

	ComponentBase::ComponentBase(Entity& _owner, detail::ComponentId id)
		: owner(_owner)
		, componentId(id)
	{
	}

	bool ComponentBase::IsEnabled() const
	{
		return isEnabled && owner.IsEnabled();
	}

	bool ComponentBase::IsComponentEnabled() const
	{
		return isEnabled;
	}

	const loupe::type& ComponentBase::GetType() const
	{
		return *typeId;
	}

	bool ComponentBase::IsA(const loupe::type& baseType) const
	{
		return typeId->is_a(baseType);
	}

	Entity::Entity(std::string name)
	{
		Add<Name>(std::move(name));
	}

	Entity::Entity(const Transform& pose)
		: Transform(pose)
	{
	}

	Entity::~Entity()
	{
		RemoveAllComponents();
		RemoveAllTags();
	}

	Entity::Ptr Entity::MakeNewRoot()
	{
		auto entity = MakeNew();
		entity->Add<Hierarchy>();

		return entity;
	}

	Entity::Ptr Entity::MakeNewRoot(std::string name)
	{
		auto entity = MakeNew();
		entity->Add<Hierarchy>();
		entity->Add<Name>(std::move(name));

		return entity;
	}

	Entity::Ptr Entity::MakeNewRoot(const Transform& pose)
	{
		auto entity = MakeNew(pose);
		entity->Add<Hierarchy>();

		return entity;
	}

	mat4 Entity::GetWorldTransform() const
	{
		mat4 result;
		if (auto* hierarchy = Try<Hierarchy>())
		{
			result = hierarchy->GetWorldTransform();
		}
		else
		{
			result = mat4(rotation, position, scale);
		}

		return result;
	}

	vec3 Entity::GetWorldPosition() const
	{
		vec3 result;
		if (auto* hierarchy = Try<Hierarchy>())
		{
			result = hierarchy->GetWorldTransform().GetTranslation();
		}
		else
		{
			result = position;
		}

		return result;
	}

	quat Entity::GetWorldRotation() const
	{
		quat result;
		if (auto* hierarchy = Try<Hierarchy>())
		{
			result = hierarchy->GetWorldRotation();
		}
		else
		{
			result = rotation;
		}

		return result;
	}

	void Entity::LookAt(const vec3& pos, const vec3& target, const vec3& up)
	{
		// Passthrough to get around C++ name-hiding.
		Transform::LookAt(pos, target, up);
	}

	void Entity::LookAt(const Entity& target, const vec3& up)
	{
		ASSERT(&target != this, "Entity cannot look at itself.");

		vec3 targetPos = target.GetWorldPosition();

		// If we have a parent, we are not in world-space.
		// We must resolve for the target position in local-space.
		auto* hierarchy = Try<Hierarchy>();
		if (hierarchy && !hierarchy->IsRoot())
		{
			targetPos = vec3(hierarchy->GetWorldTransform() * vec4(targetPos, 1.0f));
		}

		LookAt(position, targetPos, up);
	}

	std::span<const ComponentBase* const> Entity::GetAllComponents() const
	{
		return components;
	}

	std::span<ComponentBase* const> Entity::GetAllComponents()
	{
		return components;
	}

	void Entity::RemoveAllComponents()
	{
		if (isEnabled)
		{
			for (const auto* comp : components)
			{
				if (comp->IsComponentEnabled())
				{
					UnindexWithBases(*comp);
				}
			}
		}

		// This iteration strategy safely removes all components
		// even if they remove others during their destruction.
		while (!components.empty())
		{
			auto* comp = components.back();
			components.pop_back();

			comp->~ComponentBase();
			_aligned_free(comp);
		}
	}

	void Entity::RemoveAllTags()
	{
		if (isEnabled)
		{
			for (auto tag : tags)
			{
				UnindexTag(tag);
			}
		}

		tags.clear();
	}

	void Entity::Enable()
	{
		if (isEnabled)
		{
			return;
		}

		isEnabled = true;

		for (auto tag : tags)
		{
			IndexTag(tag);
		}

		for (auto* comp : components)
		{
			if (comp->IsComponentEnabled())
			{
				IndexWithBases(*comp);
			}
		}

		for (auto* comp : components)
		{
			if (comp->IsComponentEnabled())
			{
				comp->OnEnable();
			}
		}
	}

	void Entity::Disable()
	{
		if (!isEnabled)
		{
			return;
		}

		isEnabled = false;

		for (auto tag : tags)
		{
			UnindexTag(tag);
		}

		for (auto* comp : components)
		{
			if (comp->IsComponentEnabled())
			{
				UnindexWithBases(*comp);
			}
		}

		for (auto* comp : components)
		{
			if (comp->IsComponentEnabled())
			{
				comp->OnDisable();
			}
		}
	}

	void Entity::Enable(ComponentBase& comp)
	{
		// The component state changes either way, but it only gets indexed if the Entity is also enabled.
		const bool wasCompEnabled = comp.isEnabled;
		comp.isEnabled = true;

		if (this->isEnabled && !wasCompEnabled)
		{
			IndexWithBases(comp);
			comp.OnEnable();
		}
	}

	void Entity::Disable(ComponentBase& comp)
	{
		// The component state changes either way, but it only gets removed if the Entity is also enabled.
		const bool wasCompEnabled = comp.isEnabled;
		comp.isEnabled = false;

		if (this->isEnabled && wasCompEnabled)
		{
			UnindexWithBases(comp);
			comp.OnDisable();
		}
	}

	bool Entity::IsEnabled() const
	{
		return isEnabled;
	}

	ComponentBase& Entity::Add(const loupe::type& compType)
	{
		ASSERT(compType.is_a(*BaseComponentTypeId), "\"compType\" must refer to a component type.");

#if GEM_DEBUG
		for (const auto* comp : components)
		{
			ASSERT(!comp->IsA(compType), "The Component (or one sharing a hierarchy) already exists on this entity.");
			ASSERT(!compType.is_a(*comp->typeId), "The Component (or one sharing a hierarchy) already exists on this entity.");
		}
#endif

		auto* newComponent = static_cast<ComponentBase*>(_aligned_malloc(compType.size, compType.alignment));
		compType.user_construct_at(newComponent, *this);

		newComponent->typeId = &compType;
		components.push_back(newComponent);

		if (IsEnabled())
		{
			IndexWithBases(*newComponent);
		}

		return *newComponent;
	}

	ComponentBase& Entity::Require(const loupe::type& compType)
	{
		ComponentBase* comp = Try(compType);
		return comp ? *comp : Add(compType);
	}
	
	ComponentBase& Entity::Get(const loupe::type& compType)
	{
		return GetComponent(compType);
	}

	const ComponentBase& Entity::Get(const loupe::type& compType) const
	{
		return GetComponent(compType);
	}

	ComponentBase* Entity::Try(const loupe::type& compType)
	{
		return TryComponent(compType);
	}

	const ComponentBase* Entity::Try(const loupe::type& compType) const
	{
		return TryComponent(compType);
	}

	bool Entity::Has(const loupe::type& compType) const
	{
		return Try(compType) != nullptr;
	}

	void Entity::Remove(const loupe::type& compType)
	{
		ASSERT(compType.is_a(*BaseComponentTypeId), "\"compType\" must refer to a component type.");

		for (unsigned i = 0; i < components.size(); ++i)
		{
			auto* comp = components[i];
			if (comp->IsA(compType))
			{
				components[i] = components.back();
				components.pop_back();

				if (isEnabled && comp->isEnabled)
				{
					UnindexWithBases(*comp);
				}

				comp->~ComponentBase();
				_aligned_free(comp);
				return;
			}
		}
	}

	ComponentBase& Entity::GetComponent(const loupe::type& compType) const
	{
		ASSERT(compType.is_a(*BaseComponentTypeId), "\"compType\" must refer to a component type.");

		auto itr = components.begin();
		while (true)
		{
			ASSERT(itr != components.end(), "Entity did not have the expected component.");
			if ((*itr)->IsA(compType))
			{
				return **itr;
			}

			++itr;
		}
	}

	ComponentBase* Entity::TryComponent(const loupe::type& compType) const
	{
		ASSERT(compType.is_a(*BaseComponentTypeId), "\"compType\" must refer to a component type.");

		for (auto* comp : components)
		{
			if (comp->IsA(compType))
			{
				return comp;
			}
		}

		return nullptr;
	}

	void Entity::AddTag(detail::ComponentId tagId)
	{
		if (IsEnabled())
		{
			IndexTag(tagId);
		}

		tags.push_back(tagId);
	}

	void Entity::RemoveTag(detail::ComponentId tagId)
	{
		auto itr = std::find(tags.begin(), tags.end(), tagId);
		if (itr == tags.end())
			return;

		*itr = tags.back();
		tags.pop_back();

		if (IsEnabled())
		{
			UnindexTag(tagId);
		}
	}

	void Entity::IndexTag(detail::ComponentId tagId)
	{
		// Adjust [id, entity] index.
		auto& table = detail::tagIndex[tagId];
		table.insert(std::lower_bound(std::begin(table), std::end(table), this), this);
	}

	void Entity::UnindexTag(detail::ComponentId tagId)
	{
		// Adjust [id, entity] index.
		auto& table = detail::tagIndex[tagId];
		table.erase(std::lower_bound(std::begin(table), std::end(table), this));
	}

	void Entity::Index(ComponentBase& comp, const loupe::type& typeId)
	{
		// Adjust [typeId, entity] index.
		auto& table = detail::typeIndex[&typeId];
		table.insert(std::lower_bound(std::begin(table), std::end(table), this), this);

		// Adjust [id, component] index.
		detail::componentLists[&typeId].push_back(&comp);
	}

	void Entity::Unindex(const ComponentBase& comp, const loupe::type& typeId)
	{
		// Adjust [typeId, entity] index.
		auto& table = detail::typeIndex[&typeId];
		table.erase(std::lower_bound(std::begin(table), std::end(table), this));

		// Adjust [id, component] index.
		auto& componentTable = detail::componentLists[&typeId];
		auto itr = std::find(std::begin(componentTable), std::end(componentTable), &comp);
		*itr = componentTable.back();
		componentTable.pop_back();
	}

	void Entity::IndexWithBases(ComponentBase& comp)
	{
		Index(comp, *comp.typeId);

		auto implementation = [this](this auto self, ComponentBase& comp, const loupe::structure& structure) -> bool {
			for (const loupe::type* type : structure.bases)
			{
				if (type == BaseComponentTypeId)
				{
					return true;
				}

				if (self(comp, std::get<loupe::structure>(type->data)))
				{
					Index(comp, *type);
					return true;
				}
			}

			return false;
		};

		implementation(comp, std::get<loupe::structure>(comp.typeId->data));
	}

	void Entity::UnindexWithBases(const ComponentBase& comp)
	{
		Unindex(comp, *comp.typeId);

		auto implementation = [this](this auto self, const ComponentBase& comp, const loupe::structure& structure) -> bool {
			for (const loupe::type* type : structure.bases)
			{
				if (type == BaseComponentTypeId)
				{
					return true;
				}

				if (self(comp, std::get<loupe::structure>(type->data)))
				{
					Unindex(comp, *type);
					return true;
				}
			}

			return false;
		};

		implementation(comp, std::get<loupe::structure>(comp.typeId->data));
	}

	bool operator==(const Entity& lhs, const Entity& rhs)
	{
		return &lhs == &rhs;
	}

	bool operator==(const Entity& lhs, const Entity::Ptr& rhs)
	{
		return &lhs == rhs.get();
	}

	bool operator==(const Entity::Ptr& lhs, const Entity& rhs)
	{
		return lhs.get() == &rhs;
	}

	bool operator!=(const Entity& lhs, const Entity& rhs)
	{
		return &lhs != &rhs;
	}

	bool operator!=(const Entity& lhs, const Entity::Ptr& rhs)
	{
		return &lhs != rhs.get();
	}

	bool operator!=(const Entity::Ptr& lhs, const Entity& rhs)
	{
		return lhs.get() != &rhs;
	}
}

REFLECT_SIMPLE(gem::detail::ComponentId);

REFLECT(gem::ComponentBase)
	MEMBERS {
		REF_PRIVATE_MEMBER(typeId,    gem::ReadOnly)
		REF_PRIVATE_MEMBER(isEnabled, gem::ReadOnly)
	}
REF_END;

REFLECT(gem::Entity)
	BASES {
		REF_BASE(gem::Transform)
	}
	MEMBERS {
		REF_PRIVATE_MEMBER(components, gem::ReadOnly)
		REF_PRIVATE_MEMBER(tags,       gem::ReadOnly)
		REF_PRIVATE_MEMBER(isEnabled,  gem::ReadOnly)
	}
REF_END;
