// Copyright (c) 2022 Emilian Cioca
namespace gem
{
	template<class Self, class Functor>
	void Hierarchy::ForEachChild(this Self& self, Functor&& Func)
	{
		for (auto& entity : self.children)
		{
			if (Func(*entity))
			{
				entity->Get<Hierarchy>().ForEachChild(Func);
			}
		}
	};
}
