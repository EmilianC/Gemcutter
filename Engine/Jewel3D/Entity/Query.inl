// Copyright (c) 2017 Emilian Cioca
namespace Jwl
{
	namespace detail
	{
		// Index of all Entities for each component and tag type.
		// Sorted to allow for logical operations, such as ANDing, between multiple tables in the index.
		extern std::unordered_map<unsigned, std::vector<Entity*>> entityIndex;

		// Index of all Components of a particular type.
		// Not sorted since no logical operations are performed using these tables.
		extern std::unordered_map<unsigned, std::vector<ComponentBase*>> componentIndex;

		// A lightweight tag representing the end of a query's range. We use this rather than creating another
		// potentially large end-iterator. Our custom iterators already have all the information they need to
		// detect if they have expired, so we use this tag to ask them when they has finished enumerating the range.
		struct RangeEndSentinel {};

		// Used to enumerate an Entity or Component index table.
		// Although it models a single iterator, it knows when it has reached the end of the table.
		template<class SourcePtr, class Target, bool UseDynamicCast = false>
		class SafeIterator
		{
			using Iterator          = typename std::vector<SourcePtr>::iterator;
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type        = Target&;
			using difference_type   = std::ptrdiff_t;
			using pointer           = Target*;
			using reference         = Target&;

			SafeIterator(Iterator _itr, Iterator _itrEnd)
				: itr(_itr), itrEnd(_itrEnd)
			{
				if constexpr (UseDynamicCast)
				{
					if (!IsTerminated() && !dynamic_cast<Target*>(*itr))
					{
						++(*this);
					}
				}
			}

			SafeIterator& operator++()
			{
				ASSERT(!IsTerminated(), "Invalid range.");
				++itr;

				if constexpr (UseDynamicCast)
				{
					while (!IsTerminated())
					{
						if (dynamic_cast<Target*>(*itr))
							break;

						++itr;
					}
				}

				return *this;
			}

			Target& operator*() const
			{
				ASSERT(!IsTerminated(), "Invalid range.");
				return *static_cast<Target*>(*itr);
			}

			Target* Get() const
			{
				ASSERT(!IsTerminated(), "Invalid range.");
				return static_cast<Target*>(*itr);
			}

			bool operator==(RangeEndSentinel) const { return IsTerminated(); }
			bool operator!=(RangeEndSentinel) const { return !IsTerminated(); }

			bool IsTerminated() const { return itr == itrEnd; }
			void Terminate() { itr = itrEnd; }

		private:
			// The current position in the table.
			Iterator itr;
			// Ensures that we don't surpass the table while we are skipping items.
			const Iterator itrEnd;
		};

		template<class Component>
		using ComponentIterator = SafeIterator<ComponentBase*, Component, !std::is_same_v<Component, typename Component::StaticComponentType>>;
		using EntityIterator = SafeIterator<Entity*, Entity>;

		// Provides functions to advance two SafeIterators as a logical AND of two entityIndex tables.
		// This provider pattern will allow us to add more operations in the future.
		struct Intersection
		{
			static void FindFirst(EntityIterator& itr1, EntityIterator& itr2)
			{
				while (!(itr1.IsTerminated() || itr2.IsTerminated()))
				{
					Entity* p1 = itr1.Get();
					Entity* p2 = itr2.Get();

					if (p1 < p2)
					{
						++itr1;
					}
					else if (p1 > p2)
					{
						++itr2;
					}
					else
					{
						break;
					}
				}

				// Itr1 is the one that is checked for death, so propagate the termination if Itr2 finished first.
				if (itr2.IsTerminated())
				{
					itr1.Terminate();
				}
			}

			static void FindNext(EntityIterator& itr1, EntityIterator& itr2)
			{
				FindFirst(++itr1, itr2);
			}
		};

		// Enumerates a SafeIterator and a templated iterator while performing a logical operation between them.
		// The templated iterator can expand into a subtree of more LogicalIterators, or be a SafeIterator itself.
		template<class Iterator, class Operation>
		class LogicalIterator
		{
			static constexpr bool isLeaf = std::is_same_v<Iterator, EntityIterator>;
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type        = Entity*;
			using difference_type   = ptrdiff_t;
			using pointer           = Entity**;
			using reference         = Entity*&;

			LogicalIterator(EntityIterator _itr1, Iterator _itr2)
				: itr1(_itr1), itr2(_itr2)
			{
				if constexpr (isLeaf)
				{
					Operation::FindFirst(itr1, itr2);
				}
				else
				{
					Operation::FindFirst(itr1, itr2.GetCurrentItr());
				}
			}

			LogicalIterator& operator++()
			{
				if constexpr (isLeaf)
				{
					Operation::FindNext(itr1, itr2);
				}
				else
				{
					// Intersect the result of the right-hand subtree with our left-hand SafeIterator.
					++itr2;
					Operation::FindNext(itr1, itr2.GetCurrentItr());
				}

				return *this;
			}

			EntityIterator& GetCurrentItr() { return itr1; }
			Entity& operator*() const { return *itr1; }

			bool operator==(RangeEndSentinel) const { return itr1.IsTerminated(); }
			bool operator!=(RangeEndSentinel) const { return !itr1.IsTerminated(); }

		private:
			EntityIterator itr1;
			Iterator itr2;
		};

		// Represents a lazy-evaluated range that can be used in a range-based for loop.
		template<class RootIterator>
		class Range
		{
		public:
			Range(RootIterator _itr)
				: itr(_itr)
			{}

			RootIterator& begin()
			{
				return itr;
			}

			RangeEndSentinel end() const
			{
				return {};
			}

		private:
			// The starting position of the range.
			RootIterator itr;
		};

		// Template deduction assisted construction.
		template<class Iterator>
		auto BuildLogicalIterator(EntityIterator&& set1, Iterator&& set2)
		{
			return LogicalIterator<Iterator, Intersection>(set1, set2);
		}

		// BuildRootIterator() base case.
		template<typename Arg>
		EntityIterator BuildRootIterator()
		{
			auto& index = entityIndex[Arg::GetComponentId()];
			auto begin = index.begin();
			auto end = index.end();

			return EntityIterator(begin, end);
		}

		// Constructs a logical iterator representing the start of the sequence.
		template<typename Arg1, typename Arg2, typename... Args>
		auto BuildRootIterator()
		{
			auto& index = entityIndex[Arg1::GetComponentId()];
			auto begin = index.begin();
			auto end = index.end();

			return BuildLogicalIterator(EntityIterator(begin, end), BuildRootIterator<Arg2, Args...>());
		}
	}

	// Returns an enumerable range of all enabled Components of the specified type.
	// By providing you the Component directly you don't have to waste time calling Entity.Get<>().
	// This is a faster option than With<>() but it only allows you to specify a single Component type.
	template<class Component>
	auto All()
	{
		static_assert(std::is_base_of_v<ComponentBase, Component>,
			"Template argument must be a Component.");

		static_assert(!std::is_base_of_v<TagBase, Component>,
			"Cannot query tags with All<>(). Use With<>() instead.");

		using namespace detail;
		auto& index = componentIndex[Component::GetComponentId()];
		auto itr = ComponentIterator<Component>(index.begin(), index.end());

		return detail::Range(itr);
	}

	// Returns an enumerable range of all Entities which have an active instance of each specified Component/Tag.
	// Disabled Components and Components belonging to disabled Entities are not considered.
	// * Adding/Removing Components or Tags of the queried types will invalidate the returned Range *
	// For this reason, you must not do this until after you are finished using the Range.
	template<typename... Args>
	auto With()
	{
		static_assert(sizeof...(Args),
			"With<>() must receive at least one template argument.");

		static_assert(Meta::all_of_v<std::is_base_of<ComponentBase, Args>::value...>,
			"All template arguments must be either Components or Tags.");

		static_assert(Meta::all_of_v<std::is_same<Args, typename Args::StaticComponentType>::value...>,
			"Only a direct inheritor from Component<> can be used in a With<>() query. Use All<>() instead.");

		using namespace detail;
		auto&& itr = BuildRootIterator<Args...>();

		return detail::Range(itr);
	}

	// Returns all Entities which have an active instance of each specified Component/Tag.
	// Disabled Components and Components belonging to disabled Entities are not considered.
	// Unlike With<>(), adding or removing Components/Tags of the queried type will NOT invalidate the returned Range.
	// * This should only be used if necessary, as it is much slower than using With<>() *
	template<typename Arg1, typename... Args>
	std::vector<Entity::Ptr> CaptureWith()
	{
		std::vector<Entity::Ptr> result;
		if constexpr (sizeof...(Args) == 0)
		{
			using namespace detail;
			result.reserve(entityIndex[Arg1::GetComponentId()].size());
		}

		for (Entity& ent : With<Arg1, Args...>())
		{
			result.emplace_back(ent.GetPtr());
		}

		return result;
	}

	// Returns the raw vector container of the specified Component.
	// This can be useful in special cases when you need custom iterator logic.
	template<class Component>
	std::vector<ComponentBase*>& GetComponentIndex()
	{
		using namespace detail;
		return componentIndex[Component::GetComponentId()];
	}
}
