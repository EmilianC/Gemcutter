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
#ifdef GEM_DEV
		std::unordered_map<int, std::string_view>& componentNames = GetComponentNames();
		std::unordered_map<int, std::string_view>& GetComponentNames()
		{
			static std::unordered_map<int, std::string_view> names;
			return names;
		}
#endif
		std::unordered_map<ComponentId, std::vector<Entity*>> entityIndex;
		std::unordered_map<ComponentId, std::vector<ComponentBase*>> componentIndex;
	}

	ComponentBase::ComponentBase(Entity& _owner, ComponentId _componentId)
		: owner(_owner)
		, componentId(_componentId)
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

	void Entity::Tag(ComponentId tagId)
	{
		if (IsEnabled())
		{
			IndexTag(tagId);
		}

		tags.push_back(tagId);
	}

	void Entity::RemoveTag(ComponentId tagId)
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

	void Entity::IndexTag(ComponentId tagId)
	{
		// Adjust [id, entity] index.
		auto& table = detail::entityIndex[tagId];
		table.insert(std::lower_bound(table.begin(), table.end(), this), this);
	}

	void Entity::UnindexTag(ComponentId tagId)
	{
		// Adjust [id, entity] index.
		auto& table = detail::entityIndex[tagId];
		table.erase(std::lower_bound(table.begin(), table.end(), this));
	}

	void Entity::Index(ComponentBase& comp)
	{
		// Adjust [id, entity] index.
		IndexTag(comp.componentId);

		// Adjust [id, component] index.
		detail::componentIndex[comp.componentId].push_back(&comp);
	}

	void Entity::Unindex(ComponentBase& comp)
	{
		// Adjust [id, entity] index.
		UnindexTag(comp.componentId);

		// Adjust [id, component] index.
		auto& componentTable = detail::componentIndex[comp.componentId];
		auto itr = std::find(componentTable.begin(), componentTable.end(), &comp);
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
