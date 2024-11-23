// Copyright (c) 2022 Emilian Cioca
namespace gem
{
	template<class Self, class Functor>
	void Hierarchy::ForEachChild(this Self& self, bool recursive, Functor&& Func)
	{
		for (auto& entity : self.children)
		{
			Func(*entity);

			if (recursive)
			{
				entity->Get<Hierarchy>().ForEachChild(true, Func);
			}
		}
	};
}
