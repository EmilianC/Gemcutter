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

	void Entity::RemoveAllComponents()
	{
		// This loop safely removes all components even if they remove others during destruction.
		while (!components.empty())
		{
			auto* comp = components.back();
			components.pop_back();

			if (comp->IsEnabled())
			{
				Unindex(*comp);
			}

			delete comp;
		}
	}

	void Entity::RemoveAllTags()
	{
		if (IsEnabled())
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

		// Re-Index all active components.
		for (auto* comp : components)
		{
			if (comp->IsComponentEnabled())
			{
				Index(*comp);
				comp->OnEnable();
			}
		}

		// Re-Index all tags.
		for (auto tag : tags)
		{
			IndexTag(tag);
		}

		isEnabled = true;
	}

	void Entity::Disable()
	{
		if (!isEnabled)
		{
			return;
		}

		// UnIndex all active components.
		for (auto* comp : components)
		{
			if (comp->IsComponentEnabled())
			{
				Unindex(*comp);
				comp->OnDisable();
			}
		}

		// UnIndex all tags.
		for (auto tag : tags)
		{
			UnindexTag(tag);
		}

		isEnabled = false;
	}

	bool Entity::IsEnabled() const
	{
		return isEnabled;
	}

	void Entity::Tag(detail::ComponentId tagId)
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

	void Entity::Index(ComponentBase& comp)
	{
		// Adjust [typeId, entity] index.
		auto& table = detail::typeIndex[comp.typeId];
		table.insert(std::lower_bound(std::begin(table), std::end(table), this), this);

		// Adjust [id, component] index.
		detail::componentLists[comp.typeId].push_back(&comp);
	}

	void Entity::Unindex(const ComponentBase& comp)
	{
		// Adjust [typeId, entity] index.
		auto& table = detail::typeIndex[comp.typeId];
		table.erase(std::lower_bound(std::begin(table), std::end(table), this));

		// Adjust [id, component] index.
		auto& componentTable = detail::componentLists[comp.typeId];
		auto itr = std::find(std::begin(componentTable), std::end(componentTable), &comp);
		*itr = componentTable.back();
		componentTable.pop_back();
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
		REF_PRIVATE_MEMBER(typeId)
		REF_PRIVATE_MEMBER(isEnabled)
	}
REF_END;

REFLECT(gem::Entity)
	BASES {
		REF_BASE(gem::Transform)
	}
	MEMBERS {
		REF_PRIVATE_MEMBER(components)
		REF_PRIVATE_MEMBER(tags)
		REF_PRIVATE_MEMBER(isEnabled)
	}
REF_END;
