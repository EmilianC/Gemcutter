// Copyright (c) 2022 Emilian Cioca
namespace gem
{
	template<class Functor>
	void Hierarchy::ForEachChild(bool recursive, Functor&& Func)
	{
		for (Entity::Ptr& entity : children)
		{
			Func(*entity);

			if (recursive)
			{
				entity->Get<Hierarchy>().ForEachChild(true, Func);
			}
		}
	};
}
