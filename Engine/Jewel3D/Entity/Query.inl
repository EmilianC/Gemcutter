// Copyright (c) 2017 Emilian Cioca
namespace Jwl
{
	namespace detail
	{
		// Index of all Entities for each component and tag type. Used to power the queries.
		// Sorted to allow for logical operations, such as ANDing, between multiple tables in the index.
		extern std::unordered_map<unsigned, std::vector<Entity*>> entityIndex;
		// Index of all Components of a particular type. Used to power the queries.
		// Not sorted since no logical operations are performed using these tables.
		extern std::unordered_map<unsigned, std::vector<ComponentBase*>> componentIndex;

		// This is a light-weight tag representing the end of a query's range.
		// This allows us to avoid creating a second, complex, iterator just to represent the end of a query.
		// Since our custom iterators already have all the information they need to know if they have expired,
		// we can use this tag to ask them when they has finished enumerating the range.
		struct RangeEndSentinel {};
		
		// Enumerates a table of the componentIndex while performing a cast and a dereference.
		template<class Component>
		class ComponentIterator : public std::iterator<std::forward_iterator_tag, Component>
		{
			using Iterator = std::vector<ComponentBase*>::iterator;
		public:
			ComponentIterator(Iterator _itr, const Iterator _itrEnd)
				: itr(_itr), itrEnd(_itrEnd)
			{}

			ComponentIterator& operator++()
			{
				ASSERT(!IsTerminated(), "Invalid range is in use.");
				++itr;
				return *this;
			}

			Component& operator*() const
			{
				ASSERT(!IsTerminated(), "Invalid range is in use.");
				return *static_cast<Component*>(*itr);
			}

			bool operator!=(RangeEndSentinel) const
			{
				return !IsTerminated();
			}

			bool IsTerminated() const
			{
				return itr == itrEnd;
			}

		private:
			// The current position in the target table.
			Iterator itr;
			// Ensures that we don't surpass the table while we are skipping items.
			const Iterator itrEnd;
		};

		// A safe iterator used to enumerate the entityIndex tables.
		// Although it models a single iterator, it knows when it has reached the end of the table.
		class SafeIterator : public std::iterator<std::forward_iterator_tag, Entity*>
		{
			using Iterator = std::vector<Entity*>::iterator;
		public:
			SafeIterator(Iterator _itr, const Iterator _itrEnd)
				: itr(_itr), itrEnd(_itrEnd)
			{}

			SafeIterator& operator++()
			{
				ASSERT(!IsTerminated(), "Invalid range is in use.");
				++itr;
				return *this;
			}

			Entity& operator*() const
			{
				ASSERT(!IsTerminated(), "Invalid range is in use.");
				return *(*itr);
			}

			bool operator!=(RangeEndSentinel) const
			{
				return !IsTerminated();
			}

			Entity* Get() const
			{
				ASSERT(!IsTerminated(), "Invalid range is in use.");
				return *itr;
			}

			bool IsTerminated() const
			{
				return itr == itrEnd;
			}

			void Terminate()
			{
				itr = itrEnd;
			}

		private:
			// The current position in the table.
			Iterator itr;
			// Ensures that we don't surpass the table while we are skipping items.
			const Iterator itrEnd;
		};

		// Provides functions to advance two SafeIterators as a logical AND of two entityIndex tables.
		// This provider pattern will allow us to add more operations in the future.
		struct Intersection
		{
			static void FindFirst(SafeIterator& itr1, SafeIterator& itr2)
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

			static void FindNext(SafeIterator& itr1, SafeIterator& itr2)
			{
				FindFirst(++itr1, itr2);
			}
		};

		// Enumerates a SafeIterator and a templated iterator while performing a logical operation between them.
		// The templated iterator expands into a subtree of more LogicalIterators.
		template<class Iterator, class Operation>
		class LogicalIterator : public std::iterator<std::forward_iterator_tag, Entity*>
		{
		public:
			LogicalIterator(SafeIterator _itr1, const Iterator _itr2)
				: itr1(_itr1), itr2(_itr2)
			{
				Operation::FindFirst(itr1, itr2.GetCurrentItr());
			}

			LogicalIterator& operator++()
			{
				// Update the right-hand side of the iterator hierarchy.
				++itr2;

				// Intersect the result of the subtree with our left-hand SafeIterator.
				Operation::FindNext(itr1, itr2.GetCurrentItr());
				return *this;
			}

			Entity& operator*() const
			{
				return *itr1;
			}

			bool operator!=(RangeEndSentinel) const
			{
				return !itr1.IsTerminated();
			}

			SafeIterator& GetCurrentItr()
			{
				return itr1;
			}

		private:
			SafeIterator itr1;
			Iterator itr2;
		};

		// LogicalIterator specialization for the bottom of the hierarchy.
		// Directly operates on two SafeIterators.
		template<class Operation>
		class LogicalIterator<SafeIterator, Operation> : public std::iterator<std::forward_iterator_tag, Entity*>
		{
		public:
			LogicalIterator(SafeIterator _itr1, const SafeIterator _itr2)
				: itr1(_itr1), itr2(_itr2)
			{
				Operation::FindFirst(itr1, itr2);
			}

			LogicalIterator& operator++()
			{
				Operation::FindNext(itr1, itr2);
				return *this;
			}

			Entity& operator*() const
			{
				return *itr1;
			}

			bool operator!=(RangeEndSentinel) const
			{
				return !itr1.IsTerminated();
			}

			SafeIterator& GetCurrentItr()
			{
				return itr1;
			}

		private:
			SafeIterator itr1;
			SafeIterator itr2;
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
				return RangeEndSentinel();
			}

		private:
			// The starting position of the range.
			RootIterator itr;
		};

		// Template deduction assisted construction.
		template<class Iterator>
		auto BuildLogicalIterator(SafeIterator&& set1, Iterator&& set2)
		{
			return LogicalIterator<Iterator, Intersection>(set1, set2);
		}

		// Constructs a logical iterator representing the start of the sequence.
		template<typename Arg1, typename Arg2, typename... Args>
		auto BuildRootIterator()
		{
			auto& index = entityIndex[Arg1::GetComponentId()];
			auto rawBegin = index.begin();
			auto rawEnd = index.end();

			return BuildLogicalIterator(SafeIterator(rawBegin, rawEnd), BuildRootIterator<Arg2, Args...>());
		}

		// BuildRootIterator() base case.
		template<typename Arg>
		SafeIterator BuildRootIterator()
		{
			auto& index = entityIndex[Arg::GetComponentId()];
			auto rawBegin = index.begin();
			auto rawEnd = index.end();

			return SafeIterator(rawBegin, rawEnd);
		}
	}

	// Returns an enumerable range of all enabled Components of the specified type.
	// This is a faster option than With<>() but it only allows you to specify a single Component type.
	// Additionally, by giving you the Component directly you don't have to waste time calling Entity.Get<>().
	template<class Component>
	auto All()
	{
		static_assert(
			std::is_base_of_v<ComponentBase, Component>,
			"Template argument must be a Component.");

		static_assert(
			!std::is_base_of_v<TagBase, Component>,
			"Cannot query tags with All<>(). Use With<>() instead.");

		static_assert(
			std::is_same_v<Component, typename Component::StaticComponentType>,
			"Only a direct inheritor from Component<> can be used in a query.");

		using namespace detail;
		auto& index = componentIndex[Component::GetComponentId()];
		auto itr = ComponentIterator<Component>(index.begin(), index.end());

		return detail::Range<decltype(itr)>(itr);
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
			"Only a direct inheritor from Component<> can be used in a query.");

		using namespace detail;
		auto itr = BuildRootIterator<Args...>();

		return detail::Range<decltype(itr)>(itr);
	}

	// Returns all Entities which have an active instance of each specified Component/Tag.
	// Disabled Components and Components belonging to disabled Entities are not considered.
	// Unlike With<>(), adding or removing Components/Tags of the queried type will NOT invalidate the returned Range.
	// * This should only be used if necessary, as it is much slower than using With<>() *
	template<typename... Args>
	auto CaptureWith()
	{
		auto range = With<Args...>();

		std::vector<Entity::Ptr> result;
		for (Entity& ent : range)
		{
			result.push_back(ent.GetPtr());
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
