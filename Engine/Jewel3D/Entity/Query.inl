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
		
		// Enumerates a table of the componentIndex while performing a cast and a dereference.
		template<class Component>
		class ComponentIterator : public std::iterator<std::forward_iterator_tag, Component>
		{
			using Iterator = std::vector<ComponentBase*>::iterator;
		public:
			ComponentIterator(Iterator _itr)
				: itr(_itr)
			{}

			ComponentIterator& operator++()
			{
				++itr;
				return *this;
			}

			ComponentIterator operator++(int)
			{
				ComponentIterator result(*this);
				operator++();
				return result;
			}

			Component& operator*() const
			{
				return *static_cast<Component*>(*itr);
			}

			Component* operator->() const
			{
				return static_cast<Component*>(*itr);
			}

			bool operator==(const ComponentIterator& other) const
			{
				return itr == other.itr;
			}

			bool operator!=(const ComponentIterator& other) const
			{
				return itr != other.itr;
			}

		private:
			// The current position in the target table.
			Iterator itr;
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
				ASSERT(!IsTerminated(), "Iterator cannot be incremented. Check for invalid usage of With<>().");
				++itr;
				return *this;
			}

			SafeIterator operator++(int)
			{
				ASSERT(!IsTerminated(), "Iterator cannot be incremented. Check for invalid usage of With<>().");
				SafeIterator result(*this);
				operator++();
				return result;
			}

			Entity& operator*() const
			{
				ASSERT(!IsTerminated(), "Iterator cannot be dereferenced. Check for invalid usage of With<>().");
				return *(*itr);
			}

			Entity* operator->() const
			{
				ASSERT(!IsTerminated(), "Iterator cannot be dereferenced. Check for invalid usage of With<>().");
				return *itr;
			}

			bool operator==(const SafeIterator& other) const
			{
				ASSERT(itrEnd == other.itrEnd, 
					"Comparison between iterators of different index tables. Check for invalid usage of With<>().");
				return itr == other.itr;
			}

			bool operator!=(const SafeIterator& other) const
			{
				ASSERT(itrEnd == other.itrEnd, 
					"Comparison between iterators of different index tables. Check for invalid usage of With<>().");
				return itr != other.itr;
			}

			Entity* Get() const
			{
				ASSERT(!IsTerminated(), "Iterator is invalid. Check for invalid usage of With<>().");
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
			// This is required to ensure we don't surpass the table while we are skipping items.
			const Iterator itrEnd;
		};

		// Provides functions to advance two SafeIterators as a logical AND of two entityIndex tables.
		// This provider pattern will allow us to add more operations in the future.
		struct Intersection
		{
			static void FindFirst(SafeIterator& itr1, SafeIterator& itr2)
			{
				while (!(itr1.IsTerminated() || itr2.IsTerminated()) && itr1.Get() != itr2.Get())
				{
					if (itr1.Get() < itr2.Get())
					{
						++itr1;
					}
					else
					{
						++itr2;
					}
				}

				// Kill the iterators if any reached their end.
				if (itr1.IsTerminated())
				{
					itr2.Terminate();
				}
				else if (itr2.IsTerminated())
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

			LogicalIterator operator++(int)
			{
				LogicalIterator result(*this);
				operator++();
				return result;
			}

			Entity& operator*() const
			{
				return *itr1;
			}

			Entity* operator->() const
			{
				return &(*itr1);
			}

			bool operator==(const LogicalIterator& other) const
			{
				return itr1 == other.itr1;
			}

			bool operator!=(const LogicalIterator& other) const
			{
				return itr1 != other.itr1;
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

			LogicalIterator operator++(int)
			{
				LogicalIterator result(*this);
				operator++();
				return result;
			}

			Entity& operator*() const
			{
				return *itr1;
			}

			Entity* operator->() const
			{
				return &(*itr1);
			}

			bool operator==(const LogicalIterator& other) const
			{
				return itr1 == other.itr1 && itr2 == other.itr2;
			}

			bool operator!=(const LogicalIterator& other) const
			{
				return itr1 != other.itr1 || itr2 != other.itr2;
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
			Range(RootIterator _itr, const RootIterator _itrEnd)
				: itr(_itr), itrEnd(_itrEnd)
			{}

			RootIterator& begin() { return itr; }
			const RootIterator& end() const { return itrEnd; }

		private:
			// The starting position of the range.
			RootIterator itr;
			// The ending position of the range.
			const RootIterator itrEnd;
		};

		// Template deduction assisted construction.
		template<class Iterator>
		auto BuildLogicalIterator(SafeIterator&& set1, Iterator&& set2)
		{
			return LogicalIterator<Iterator, Intersection>(set1, set2);
		}

		// Constructs a logical iterator representing the start of the sequence.
		template<typename Arg1, typename Arg2, typename... Args>
		auto BuildRootBegin()
		{
			auto& index = entityIndex[Arg1::GetComponentId()];
			auto rawBegin = index.begin();
			auto rawEnd = index.end();

			return BuildLogicalIterator(SafeIterator(rawBegin, rawEnd), BuildRootBegin<Arg2, Args...>());
		}

		// BuildRootBegin() base case.
		template<typename Arg>
		SafeIterator BuildRootBegin()
		{
			auto& index = entityIndex[Arg::GetComponentId()];
			auto rawBegin = index.begin();
			auto rawEnd = index.end();

			return SafeIterator(rawBegin, rawEnd);
		}

		// Constructs a logical iterator representing the end of the sequence.
		template<typename Arg1, typename Arg2, typename... Args>
		auto BuildRootEnd()
		{
			auto rawEnd = entityIndex[Arg1::GetComponentId()].end();

			return BuildLogicalIterator(SafeIterator(rawEnd, rawEnd), BuildRootEnd<Arg2, Args...>());
		}

		// BuildRootEnd() base case.
		template<typename Arg>
		SafeIterator BuildRootEnd()
		{
			auto rawEnd = entityIndex[Arg::GetComponentId()].end();

			return SafeIterator(rawEnd, rawEnd);
		}
	}

	// Returns an enumerable range of all enabled Components of the specified type.
	// This is a faster option than With<>() but it only allows you to specify a single Component type.
	// Additionally, by giving you the Component directly you don't have to waste time calling Entity.Get<>().
	template<class Component>
	auto All()
	{
		static_assert(
			std::is_base_of<ComponentBase, Component>::value,
			"Template argument must be a Component.");

		static_assert(
			!std::is_base_of<TagBase, Component>::value,
			"Cannot query tags with All<>(). Use With<>() instead.");

		static_assert(
			std::is_same<Component, typename Component::StaticComponentType>::value,
			"Only a direct inheritor from Component<> can be used in a query.");

		using namespace detail;
		auto& index = componentIndex[Component::GetComponentId()];
		auto begin = ComponentIterator<Component>(index.begin());
		auto end = ComponentIterator<Component>(index.end());

		return detail::Range<decltype(begin)>(begin, end);
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
		auto begin = BuildRootBegin<Args...>();
		auto end = BuildRootEnd<Args...>();

		return detail::Range<decltype(begin)>(begin, end);
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
		result.reserve(std::distance(range.begin(), range.end()));

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
