// Copyright (c) 2021 Emilian Cioca
namespace gem
{
	template<class T, typename... Args>
	T& Widget::CreateChild(Args&&... constructorParams)
	{
		static_assert(std::is_base_of_v<Widget, T>, "Template argument must be a Widget.");

		auto entity = owner.Get<Hierarchy>().CreateChild();
		return entity->Add<T>(std::forward<Args>(constructorParams)...);
	}
}
