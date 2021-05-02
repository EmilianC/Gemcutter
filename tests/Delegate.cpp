#include <catch/catch.hpp>
#include <gemcutter/Application/Delegate.h>
#include <gemcutter/Application/Event.h>
#include <gemcutter/Entity/Entity.h>

using namespace gem;

TEST_CASE("Events")
{
	SECTION("Delegates")
	{
		int count = 0;
		std::optional<int> result;

		Delegate<int()> delegate;
		CHECK(!delegate);

		DelegateHandle handle = delegate.Bind([&]() { return ++count; });
		CHECK(delegate);
		CHECK(count == 0);

		result = delegate();
		REQUIRE(result.has_value());
		CHECK(result.value() == 1);
		CHECK(count == 1);

		result = delegate();
		REQUIRE(result.has_value());
		CHECK(result.value() == 2);
		CHECK(count == 2);

		SECTION("Handle Expired")
		{
			handle.Expire();
			result = delegate();
			CHECK(!result.has_value());
			CHECK(count == 2);
		}

		SECTION("Moved")
		{
			Delegate<int()> movedDelegate = std::move(delegate);
			CHECK(movedDelegate);

			result = movedDelegate();
			REQUIRE(result.has_value());
			CHECK(result.value() == 3);
			CHECK(count == 3);
			CHECK(movedDelegate);
		}

		SECTION("Clear")
		{
			delegate.Clear();
			CHECK(!delegate);

			result = delegate();
			CHECK(!result.has_value());
			CHECK(count == 2);
		}

		SECTION("Lifetime Object")
		{
			int lifetimeCount = 0;
			auto lifetimeObject = std::make_shared<int>(0);

			delegate.Clear();
			CHECK(!delegate);

			delegate.Bind(lifetimeObject, [&]() { return ++lifetimeCount; });
			CHECK(delegate);

			result = delegate();
			REQUIRE(result.has_value());
			CHECK(result.value() == 1);
			CHECK(lifetimeCount == 1);
			CHECK(delegate);

			result = delegate();
			REQUIRE(result.has_value());
			CHECK(result.value() == 2);
			CHECK(lifetimeCount == 2);
			CHECK(delegate);

			lifetimeObject.reset();
			CHECK(delegate);

			result = delegate();
			CHECK(!result.has_value());
			CHECK(!delegate);
			CHECK(lifetimeCount == 2);
		}
	}

	SECTION("Dispatcher")
	{
		int countA = 0;
		int countB = 0;

		Dispatcher<void()> dispatcher;
		CHECK(!dispatcher.HasBindings());

		DelegateHandle handleA = dispatcher.Add([&]() { countA++; });
		DelegateHandle handleB = dispatcher.Add([&]() { countB++; });
		CHECK(dispatcher.HasBindings());
		CHECK(countA == 0);
		CHECK(countB == 0);

		dispatcher.Dispatch();
		CHECK(countA == 1);
		CHECK(countB == 1);

		dispatcher.Dispatch();
		CHECK(countA == 2);
		CHECK(countB == 2);

		SECTION("Handles Expired")
		{
			handleA.Expire();
			CHECK(dispatcher.HasBindings());

			dispatcher.Dispatch();
			CHECK(dispatcher.HasBindings());
			CHECK(countA == 2);
			CHECK(countB == 3);

			handleB.Expire();
			CHECK(!dispatcher.HasBindings());

			dispatcher.Dispatch();
			CHECK(!dispatcher.HasBindings());
			CHECK(countA == 2);
			CHECK(countB == 3);
		}

		SECTION("Moved")
		{
			Dispatcher<void()> movedDispatcher = std::move(dispatcher);
			CHECK(movedDispatcher.HasBindings());

			movedDispatcher.Dispatch();
			CHECK(movedDispatcher.HasBindings());
			CHECK(countA == 3);
			CHECK(countB == 3);
		}

		SECTION("Clear")
		{
			dispatcher.Clear();
			CHECK(!dispatcher.HasBindings());
			CHECK(countA == 2);
			CHECK(countB == 2);
		}

		SECTION("Lifetime Object")
		{
			int lifetimeCount = 0;
			auto lifetimeObject = std::make_shared<int>(0);

			dispatcher.Clear();
			CHECK(!dispatcher.HasBindings());

			dispatcher.Add(lifetimeObject, [&]() { return ++lifetimeCount; });
			CHECK(dispatcher.HasBindings());

			dispatcher.Dispatch();
			CHECK(dispatcher.HasBindings());
			CHECK(lifetimeCount == 1);

			dispatcher.Dispatch();
			CHECK(dispatcher.HasBindings());
			CHECK(lifetimeCount == 2);

			lifetimeObject.reset();
			CHECK(dispatcher.HasBindings());

			dispatcher.Dispatch();
			CHECK(!dispatcher.HasBindings());
			CHECK(lifetimeCount == 2);
		}
	}
}
