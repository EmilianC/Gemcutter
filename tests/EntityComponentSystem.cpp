#include <catch/catch.hpp>
#include <gemcutter/Entity/Entity.h>

using namespace gem;

class Comp1 : public Component<Comp1>
{
public:
	Comp1(Entity& owner) : Component(owner) {}
};

class Comp2 : public Component<Comp2>
{
public:
	Comp2(Entity& owner) : Component(owner) {}
};

class Base : public Component<Base>
{
public:
	Base(Entity& owner) : Component(owner) {}

	bool onEnableCalled = false;
	bool onDisableCalled = false;

	void OnEnable() override { onEnableCalled = true; }
	void OnDisable() override { onDisableCalled = true; }
};

// Inherits indirectly from Component<>, and doesn't generate its own Component ID.
class DerivedA : public Base
{
public:
	DerivedA(Entity& owner) : Base(owner) {}
};

// Inherits indirectly from Component<>, and doesn't generate its own Component ID.
class DerivedB : public Base
{
public:
	DerivedB(Entity& owner) : Base(owner) {}
};

class TagA : public Tag<TagA> {};
class TagB : public Tag<TagB> {};
class TagC : public Tag<TagC> {};

TEST_CASE("Entity-Component-System")
{
	SECTION("Adding/Removing Components")
	{
		auto ent = Entity::MakeNew();

		CHECK(!ent->Has<Base>());
		CHECK(!ent->Has<DerivedA>());
		CHECK(!ent->Has<DerivedB>());

		ent->Add<Base>();
		CHECK(ent->Has<Base>());
		CHECK(!ent->Has<DerivedA>());
		CHECK(!ent->Has<DerivedB>());

		ent->Remove<Base>();
		CHECK(!ent->Has<Base>());
		CHECK(!ent->Has<DerivedA>());
		CHECK(!ent->Has<DerivedB>());
	}

	SECTION("Adding/Removing Multiple Components")
	{
		auto ent = Entity::MakeNew();

		CHECK(!ent->Has<Comp1>());
		CHECK(!ent->Has<Comp2>());
		CHECK(!ent->Has<DerivedA>());

		auto [comp1, comp2, derived] = ent->Add<Comp1, Comp2, DerivedA>();
		CHECK(ent->Has<Comp1>());
		CHECK(ent->Has<Comp2>());
		CHECK(ent->Has<DerivedA>());
		CHECK(&comp1 == &ent->Get<Comp1>());
		CHECK(&comp2 == &ent->Get<Comp2>());
		CHECK(&derived == &ent->Get<DerivedA>());
		CHECK(comp1.IsEnabled());
		CHECK(comp2.IsEnabled());
		CHECK(derived.IsEnabled());

		ent->RemoveAllComponents();
		CHECK(!ent->Has<Comp1>());
		CHECK(!ent->Has<Comp2>());
		CHECK(!ent->Has<DerivedA>());
	}

	SECTION("Requiring/Removing Multiple Components")
	{
		auto ent = Entity::MakeNew();

		CHECK(!ent->Has<Comp1>());
		CHECK(!ent->Has<Comp2>());
		CHECK(!ent->Has<DerivedA>());

		ent->Require<Comp1>();
		CHECK(ent->Has<Comp1>());
		CHECK(!ent->Has<Comp2>());
		CHECK(!ent->Has<DerivedA>());

		ent->Require<Comp1, Comp2>();
		CHECK(ent->Has<Comp1>());
		CHECK(ent->Has<Comp2>());
		CHECK(!ent->Has<DerivedA>());

		ent->RemoveAllComponents();
		CHECK(!ent->Has<Comp1>());
		CHECK(!ent->Has<Comp2>());
		CHECK(!ent->Has<DerivedA>());

		auto [comp1, comp2, derived] = ent->Require<Comp1, Comp2, DerivedA>();
		CHECK(ent->Has<Comp1>());
		CHECK(ent->Has<Comp2>());
		CHECK(ent->Has<DerivedA>());

		CHECK(&comp1 == &ent->Get<Comp1>());
		CHECK(&comp2 == &ent->Get<Comp2>());
		CHECK(&derived == &ent->Get<DerivedA>());
		CHECK(comp1.IsEnabled());
		CHECK(comp2.IsEnabled());
		CHECK(derived.IsEnabled());
	}

	SECTION("Try getting Components")
	{
		auto ent = Entity::MakeNew();

		CHECK(!ent->Try<Comp1>());
		CHECK(!ent->Try<Comp2>());

		ent->Add<Comp1>();
		CHECK(ent->Try<Comp1>());
		CHECK(!ent->Try<Comp2>());

		ent->Add<Comp2>();
		CHECK(ent->Try<Comp1>());
		CHECK(ent->Try<Comp2>());

		ent->RemoveAllComponents();
		CHECK(!ent->Try<Comp1>());
		CHECK(!ent->Try<Comp2>());
	}

	SECTION("Derived Component Classes")
	{
		auto ent = Entity::MakeNew();

		ent->Add<DerivedA>();
		CHECK(ent->Has<Base>());
		CHECK(ent->Has<DerivedA>());
		CHECK(!ent->Has<DerivedB>());

		ent->Remove<Base>();
		CHECK(!ent->Has<Base>());
		CHECK(!ent->Has<DerivedA>());
		CHECK(!ent->Has<DerivedB>());

		ent->Add<DerivedB>();
		CHECK(ent->Has<Base>());
		CHECK(!ent->Has<DerivedA>());
		CHECK(ent->Has<DerivedB>());

		// Removing Derived type A shouldn't modify the entity.
		ent->Remove<DerivedA>();
		CHECK(ent->Has<Base>());
		CHECK(!ent->Has<DerivedA>());
		CHECK(ent->Has<DerivedB>());

		ent->Remove<DerivedB>();
		CHECK(!ent->Has<Base>());
		CHECK(!ent->Has<DerivedA>());
		CHECK(!ent->Has<DerivedB>());
	}

	SECTION("Tags")
	{
		auto ent = Entity::MakeNew();

		CHECK(!ent->HasTag<TagA>());
		CHECK(!ent->HasTag<TagB>());
		CHECK(!ent->HasTag<TagC>());

		ent->Tag<TagA>();
		CHECK(ent->HasTag<TagA>());
		CHECK(!ent->HasTag<TagB>());
		CHECK(!ent->HasTag<TagC>());

		ent->Tag<TagB>();
		CHECK(ent->HasTag<TagA>());
		CHECK(ent->HasTag<TagB>());
		CHECK(!ent->HasTag<TagC>());

		ent->Tag<TagC>();
		CHECK(ent->HasTag<TagA>());
		CHECK(ent->HasTag<TagB>());
		CHECK(ent->HasTag<TagC>());

		ent->RemoveTag<TagB>();
		CHECK(ent->HasTag<TagA>());
		CHECK(!ent->HasTag<TagB>());
		CHECK(ent->HasTag<TagC>());

		ent->RemoveAllTags();
		CHECK(!ent->HasTag<TagA>());
		CHECK(!ent->HasTag<TagB>());
		CHECK(!ent->HasTag<TagC>());

		ent->Tag<TagA, TagB, TagC>();
		CHECK(ent->HasTag<TagA>());
		CHECK(ent->HasTag<TagB>());
		CHECK(ent->HasTag<TagC>());
	}

	SECTION("Removing Tags Globally")
	{
		auto ent = Entity::MakeNew();
		auto ent2 = Entity::MakeNew();
		auto ent3 = Entity::MakeNew();
		auto ent4 = Entity::MakeNew();

		ent->Tag<TagA>();
		ent2->Tag<TagA>();
		ent3->Tag<TagA>();
		ent4->Tag<TagB>();
		CHECK(ent->HasTag<TagA>());
		CHECK(ent2->HasTag<TagA>());
		CHECK(ent3->HasTag<TagA>());
		CHECK(ent4->HasTag<TagB>());

		Entity::GlobalRemoveTag<TagA>();
		CHECK(!ent->HasTag<TagA>());
		CHECK(!ent2->HasTag<TagA>());
		CHECK(!ent3->HasTag<TagA>());
		CHECK(ent4->HasTag<TagB>());

		Entity::GlobalRemoveTag<TagB>();
		CHECK(!ent4->HasTag<TagB>());
	}

	SECTION("Enabling / Disabling")
	{
		auto ent1 = Entity::MakeNew();
		auto ent2 = Entity::MakeNew();
		auto ent3 = Entity::MakeNew();
		auto ent4 = Entity::MakeNew();

		CHECK(ent1->IsEnabled());
		CHECK(ent2->IsEnabled());
		CHECK(ent3->IsEnabled());
		CHECK(ent4->IsEnabled());

		SECTION("Entities")
		{
			ent1->Disable();
			ent2->Disable();
			ent3->Disable();
			ent4->Disable();

			CHECK_FALSE(ent1->IsEnabled());
			CHECK_FALSE(ent2->IsEnabled());
			CHECK_FALSE(ent3->IsEnabled());
			CHECK_FALSE(ent4->IsEnabled());

			ent1->Enable();
			ent2->Enable();
			ent3->Enable();
			ent4->Enable();

			CHECK(ent1->IsEnabled());
			CHECK(ent2->IsEnabled());
			CHECK(ent3->IsEnabled());
			CHECK(ent4->IsEnabled());
		}

		SECTION("Components")
		{
			ent1->Add<Comp1>();
			ent2->Add<Comp2>();
			auto& derivedA = ent3->Add<DerivedA>();
			auto& derivedB = ent4->Add<DerivedB>();

			CHECK_FALSE(derivedA.onEnableCalled);
			CHECK_FALSE(derivedB.onEnableCalled);
			CHECK_FALSE(derivedA.onDisableCalled);
			CHECK_FALSE(derivedB.onDisableCalled);

			ent1->Disable();
			ent2->Disable<Comp2>();
			ent3->Disable();
			ent4->Disable<DerivedB>();

			CHECK_FALSE(derivedA.onEnableCalled);
			CHECK_FALSE(derivedB.onEnableCalled);
			CHECK(derivedA.onDisableCalled);
			CHECK(derivedB.onDisableCalled);

			CHECK_FALSE(ent1->Get<Comp1>().IsEnabled());
			CHECK_FALSE(ent2->Get<Comp2>().IsEnabled());
			CHECK_FALSE(ent3->Get<DerivedA>().IsEnabled());
			CHECK_FALSE(ent4->Get<DerivedB>().IsEnabled());

			CHECK(ent1->Get<Comp1>().IsComponentEnabled());
			CHECK_FALSE(ent2->Get<Comp2>().IsComponentEnabled());
			CHECK(ent3->Get<DerivedA>().IsComponentEnabled());
			CHECK_FALSE(ent4->Get<DerivedB>().IsComponentEnabled());

			ent1->Enable();
			ent2->Enable<Comp2>();
			ent3->Enable();
			ent4->Enable<DerivedB>();

			CHECK(derivedA.onEnableCalled);
			CHECK(derivedB.onEnableCalled);

			CHECK(ent1->Get<Comp1>().IsEnabled());
			CHECK(ent2->Get<Comp2>().IsEnabled());
			CHECK(ent3->Get<DerivedA>().IsEnabled());
			CHECK(ent4->Get<DerivedB>().IsEnabled());

			CHECK(ent1->Get<Comp1>().IsComponentEnabled());
			CHECK(ent2->Get<Comp2>().IsComponentEnabled());
			CHECK(ent3->Get<DerivedA>().IsComponentEnabled());
			CHECK(ent4->Get<DerivedB>().IsComponentEnabled());
		}
	}

	SECTION("Queries")
	{
		auto ent1 = Entity::MakeNew();
		auto ent2 = Entity::MakeNew();
		auto ent3 = Entity::MakeNew();
		auto ent4 = Entity::MakeNew();

		SECTION("Component")
		{
			ent1->Add<Comp1>();
			ent2->Add<Comp1>();
			ent3->Add<Comp1>();
			ent4->Add<Comp1>();

			ent3->Disable<Comp1>();
			ent4->Disable();

			// Some extra noise that shouldn't change the results.
			ent4->Add<Comp2>();
			ent4->Tag<TagA>();
			ent1->Add<Base>();
			ent1->Tag<TagB>();

			auto count = 0;
			for (Comp1& comp : All<Comp1>())
			{
				count++;
				CHECK(comp.IsEnabled());
				CHECK(comp.IsComponentEnabled());
				CHECK(comp.owner.IsEnabled());
			}

			CHECK(count == 2);
		}

		SECTION("Derived Components")
		{
			ent1->Add<DerivedA>();
			ent2->Add<DerivedA>();
			ent3->Add<DerivedA>();
			ent4->Add<DerivedB>();

			ent3->Disable<DerivedA>();

			// Some extra noise that shouldn't change the results.
			ent4->Add<Comp2>();
			ent4->Tag<TagA>();
			ent1->Add<Comp1>();
			ent1->Tag<TagB>();

			auto count = 0;
			for (DerivedA& comp : All<DerivedA>())
			{
				count++;
				CHECK(comp.IsEnabled());
				CHECK(comp.IsComponentEnabled());
				CHECK(comp.owner.IsEnabled());
			}
			CHECK(count == 2);

			count = 0;
			for (DerivedB& comp : All<DerivedB>())
			{
				count++;
				CHECK(comp.IsEnabled());
				CHECK(comp.IsComponentEnabled());
				CHECK(comp.owner.IsEnabled());
			}
			CHECK(count == 1);
		}

		SECTION("With<>()")
		{
			SECTION("1 Component")
			{
				// Target Entity with Comp2
				ent1->Add<Comp2>();

				// Some database noise.
				ent2->Tag<TagA>();
				ent3->Add<Comp1>();
				ent3->Tag<TagB>();

				// Query the Entity Database. We expect only our target Entity as a result.
				auto count = 0;
				for (Entity& e : With<Comp2>())
				{
					CHECK(&e == ent1.get());
					count++;

					REQUIRE(e.Has<Comp2>());
					CHECK(e.Get<Comp2>().IsEnabled());
					CHECK(e.IsEnabled());
				}
				CHECK(count == 1);
			}

			SECTION("1 Tag")
			{
				// Target Entity with TagA
				ent1->Tag<TagA>();

				// Some database noise.
				ent2->Tag<TagB>();
				ent3->Add<Comp1>();
				ent3->Add<Comp2>();
				ent3->Tag<TagC>();

				// Query the Entity Database. We expect only our target Entity as a result.
				auto count = 0;
				for (Entity& e : With<TagA>())
				{
					CHECK(&e == ent1.get());
					count++;

					CHECK(e.HasTag<TagA>());
					CHECK(e.IsEnabled());
				}
				CHECK(count == 1);
			}

			SECTION("2 Components")
			{
				// Target Entity with Comp1/Comp2
				ent1->Add<Comp1, Comp2>();

				// Some database noise.
				ent2->Add<Comp2>();
				ent2->Tag<TagA>();
				ent3->Add<Comp1>();
				ent3->Tag<TagB>();

				// Query the Entity Database. We expect only our target Entity as a result.
				auto count = 0;
				SECTION("Order 1")
				{
					for (Entity& e : With<Comp1, Comp2>())
					{
						CHECK(&e == ent1.get());
						count++;

						REQUIRE(e.Has<Comp1>());
						REQUIRE(e.Has<Comp2>());
						CHECK(e.Get<Comp1>().IsEnabled());
						CHECK(e.Get<Comp2>().IsEnabled());
						CHECK(e.Get<Comp1>().IsComponentEnabled());
						CHECK(e.Get<Comp2>().IsComponentEnabled());
						CHECK(e.IsEnabled());
					}
				}

				SECTION("Order 2")
				{
					for (Entity& e : With<Comp2, Comp1>())
					{
						CHECK(&e == ent1.get());
						count++;

						REQUIRE(e.Has<Comp1>());
						REQUIRE(e.Has<Comp2>());
						CHECK(e.Get<Comp1>().IsEnabled());
						CHECK(e.Get<Comp2>().IsEnabled());
						CHECK(e.Get<Comp1>().IsComponentEnabled());
						CHECK(e.Get<Comp2>().IsComponentEnabled());
						CHECK(e.IsEnabled());
					}
				}
				CHECK(count == 1);
			}

			SECTION("2 Entities With 2 Components")
			{
				// Target Entities with Comp1/Comp2
				ent1->Add<Comp1, Comp2>();
				ent2->Add<Comp1, Comp2>();

				// Some database noise.
				ent1->Tag<TagC>();
				ent2->Tag<TagA>();
				ent3->Add<Comp1>();
				ent3->Tag<TagB>();

				// Query the Entity Database. We expect only our target Entities as a result.
				auto count = 0;
				bool foundEnt1 = false;
				bool foundEnt2 = false;
				SECTION("Order 1")
				{
					for (Entity& e : With<Comp1, Comp2>())
					{
						if (&e == ent1.get()) foundEnt1 = true;
						if (&e == ent2.get()) foundEnt2 = true;
						count++;

						REQUIRE(e.Has<Comp1>());
						REQUIRE(e.Has<Comp2>());
						CHECK(e.Get<Comp1>().IsEnabled());
						CHECK(e.Get<Comp2>().IsEnabled());
						CHECK(e.Get<Comp1>().IsComponentEnabled());
						CHECK(e.Get<Comp2>().IsComponentEnabled());
						CHECK(e.IsEnabled());
					}
				}

				SECTION("Order 2")
				{
					for (Entity& e : With<Comp2, Comp1>())
					{
						if (&e == ent1.get()) foundEnt1 = true;
						if (&e == ent2.get()) foundEnt2 = true;
						count++;

						REQUIRE(e.Has<Comp1>());
						REQUIRE(e.Has<Comp2>());
						CHECK(e.Get<Comp1>().IsEnabled());
						CHECK(e.Get<Comp2>().IsEnabled());
						CHECK(e.Get<Comp1>().IsComponentEnabled());
						CHECK(e.Get<Comp2>().IsComponentEnabled());
						CHECK(e.IsEnabled());
					}
				}
				CHECK(count == 2);
				CHECK(foundEnt1);
				CHECK(foundEnt2);
			}

			SECTION("2 Tags")
			{
				// Target Entity with TagA/TagB
				ent1->Tag<TagA>();
				ent1->Tag<TagB>();

				// Some database noise.
				ent2->Add<Comp2>();
				ent2->Tag<TagC>();
				ent3->Add<Comp1>();
				ent3->Tag<TagB>();

				// Query the Entity Database. We expect only our target Entity as a result.
				auto count = 0;
				SECTION("Order 1")
				{
					for (Entity& e : With<TagA, TagB>())
					{
						CHECK(&e == ent1.get());
						count++;

						CHECK(e.HasTag<TagA>());
						CHECK(e.HasTag<TagB>());
						CHECK(e.IsEnabled());
					}
				}

				SECTION("Order 2")
				{
					for (Entity& e : With<TagB, TagA>())
					{
						CHECK(&e == ent1.get());
						count++;

						CHECK(e.HasTag<TagB>());
						CHECK(e.HasTag<TagA>());
						CHECK(e.IsEnabled());
					}
				}
				CHECK(count == 1);
			}

			SECTION("2 Components : 1 Tag")
			{
				// Target Entity with Comp1/Comp2/TagA
				ent1->Add<Comp1, Comp2>();
				ent1->Tag<TagA>();

				// Some database noise.
				ent2->Add<Comp2>();
				ent2->Tag<TagA>();
				ent3->Add<Comp1>();
				ent3->Tag<TagB>();

				// Query the Entity Database. We expect only our target Entity as a result.
				auto count = 0;
				SECTION("Order 1")
				{
					for (Entity& e : With<Comp1, Comp2, TagA>())
					{
						CHECK(&e == ent1.get());
						count++;

						REQUIRE(e.Has<Comp1>());
						REQUIRE(e.Has<Comp2>());
						CHECK(e.HasTag<TagA>());
						CHECK(e.Get<Comp1>().IsEnabled());
						CHECK(e.Get<Comp2>().IsEnabled());
						CHECK(e.Get<Comp1>().IsComponentEnabled());
						CHECK(e.Get<Comp2>().IsComponentEnabled());
						CHECK(e.IsEnabled());
					}
				}

				SECTION("Order 2")
				{
					for (Entity& e : With<TagA, Comp2, Comp1>())
					{
						CHECK(&e == ent1.get());
						count++;

						REQUIRE(e.Has<Comp1>());
						REQUIRE(e.Has<Comp2>());
						CHECK(e.HasTag<TagA>());
						CHECK(e.Get<Comp1>().IsEnabled());
						CHECK(e.Get<Comp2>().IsEnabled());
						CHECK(e.Get<Comp1>().IsComponentEnabled());
						CHECK(e.Get<Comp2>().IsComponentEnabled());
						CHECK(e.IsEnabled());
					}
				}
				CHECK(count == 1);
			}

			SECTION("3 Components : 3 Tags")
			{
				// Target Entity with Comp1/Comp2/Base/TagA/TagB/TagC
				ent1->Add<Comp1, Comp2, Base>();
				ent1->Tag<TagA>();
				ent1->Tag<TagB>();
				ent1->Tag<TagC>();

				// Some database noise.
				ent2->Add<Base, Comp2, Comp1>();
				ent2->Tag<TagA>();
				ent3->Add<Comp1>();
				ent3->Tag<TagB>();
				ent4->Tag<TagB>();
				ent4->Add<Base>();
				ent4->Tag<TagC>();

				// Query the Entity Database. We expect only our target Entity as a result.
				auto count = 0;
				SECTION("Order 1")
				{
					for (Entity& e : With<Comp1, Comp2, Base, TagA, TagB, TagC>())
					{
						CHECK(&e == ent1.get());
						count++;

						REQUIRE(e.Has<Comp1>());
						REQUIRE(e.Has<Comp2>());
						REQUIRE(e.Has<Base>());
						CHECK(e.HasTag<TagA>());
						CHECK(e.HasTag<TagB>());
						CHECK(e.HasTag<TagC>());
						CHECK(e.Get<Comp1>().IsEnabled());
						CHECK(e.Get<Comp2>().IsEnabled());
						CHECK(e.Get<Base>().IsEnabled());
						CHECK(e.Get<Comp1>().IsComponentEnabled());
						CHECK(e.Get<Comp2>().IsComponentEnabled());
						CHECK(e.Get<Base>().IsComponentEnabled());
						CHECK(e.IsEnabled());
					}
				}

				SECTION("Order 2")
				{
					for (Entity& e : With<TagC, TagB, TagA, Base, Comp2, Comp1>())
					{
						CHECK(&e == ent1.get());
						count++;

						REQUIRE(e.Has<Comp1>());
						REQUIRE(e.Has<Comp2>());
						REQUIRE(e.Has<Base>());
						CHECK(e.HasTag<TagA>());
						CHECK(e.HasTag<TagB>());
						CHECK(e.HasTag<TagC>());
						CHECK(e.Get<Comp1>().IsEnabled());
						CHECK(e.Get<Comp2>().IsEnabled());
						CHECK(e.Get<Base>().IsEnabled());
						CHECK(e.Get<Comp1>().IsComponentEnabled());
						CHECK(e.Get<Comp2>().IsComponentEnabled());
						CHECK(e.Get<Base>().IsComponentEnabled());
						CHECK(e.IsEnabled());
					}
				}
				CHECK(count == 1);
			}

			SECTION("Nested")
			{
				// Target #1 with Comp1
				ent1->Add<Comp1>();
				// Target #2 with Comp2
				ent2->Add<Comp2>();

				// Some database noise.
				ent2->Tag<TagA>();
				ent3->Add<DerivedA>();
				ent3->Tag<TagB>();
				ent4->Add<DerivedB>();
				ent4->Tag<TagC>();

				// Query the Entity Database. We expect only our target Entities as a result.
				auto count = 0;
				SECTION("Order 1")
				{
					for (Entity& e1 : With<Comp1>())
					{
						CHECK(&e1 == ent1.get());
						count++;

						REQUIRE(e1.Has<Comp1>());
						CHECK(e1.Get<Comp1>().IsEnabled());
						CHECK(e1.IsEnabled());

						for (Entity& e2 : With<Comp2>())
						{
							CHECK(&e2 == ent2.get());
							count++;

							REQUIRE(e2.Has<Comp2>());
							CHECK(e2.Get<Comp2>().IsEnabled());
							CHECK(e2.IsEnabled());
						}
					}
				}

				SECTION("Order 2")
				{
					for (Entity& e2 : With<Comp2>())
					{
						CHECK(&e2 == ent2.get());
						count++;

						REQUIRE(e2.Has<Comp2>());
						CHECK(e2.Get<Comp2>().IsEnabled());
						CHECK(e2.IsEnabled());

						for (Entity& e1 : With<Comp1>())
						{
							CHECK(&e1 == ent1.get());
							count++;

							REQUIRE(e1.Has<Comp1>());
							CHECK(e1.Get<Comp1>().IsEnabled());
							CHECK(e1.IsEnabled());
						}
					}
				}
				CHECK(count == 2);
			}

			SECTION("No Results")
			{
				// Some database noise.
				ent1->Tag<TagA>();
				ent1->Tag<TagB>();
				ent1->Add<Comp1>();
				ent2->Tag<TagA>();
				ent3->Add<Comp1>();
				ent3->Tag<TagB>();
				ent3->Disable();
				ent4->Add<Comp2>();

				// Query the Entity Database. No Entities should match these queries.
				auto count = 0;
				for (Entity& e : With<Comp1, Comp2>())
				{
					count++;
				}
				for (Entity& e : With<TagA, TagB, TagC>())
				{
					count++;
				}
				for (Entity& e : With<Base>())
				{
					count++;
				}
				for (Base& b : All<Base>())
				{
					count++;
				}
				CHECK(count == 0);
			}
		}

		SECTION("CaptureWith<>()")
		{
			ent1->Add<Comp1>();
			ent1->Add<Comp2>();
			ent1->Tag<TagA>();

			ent2->Add<Comp1>();
			ent2->Add<Comp2>();
			ent2->Tag<TagC>();

			ent3->Add<Comp1>();
			ent3->Tag<TagB>();
			ent3->Disable();

			ent4->Disable();
			ent4->Add<Comp1>();

			auto count = 0;
			bool foundEnt1 = false;
			bool foundEnt2 = false;
			bool foundEnt3 = false;
			bool foundEnt4 = false;

			SECTION("1 Argument")
			{
				// Query the Entity Database. We expect only our target Entities as a result.
				auto results = CaptureWith<Comp2>();

				// The targets will no longer match the query, but they have already been captured.
				ent1->RemoveAllComponents(); ent1->RemoveAllTags();
				ent2->RemoveAllComponents(); ent2->RemoveAllTags();
				ent3->RemoveAllComponents(); ent3->RemoveAllTags();
				ent4->RemoveAllComponents(); ent4->RemoveAllTags();

				for (auto& entity : results)
				{
					if (entity.get() == ent1.get()) foundEnt1 = true;
					if (entity.get() == ent2.get()) foundEnt2 = true;
					if (entity.get() == ent3.get()) foundEnt3 = true;
					if (entity.get() == ent4.get()) foundEnt4 = true;
					count++;
				}

				CHECK(foundEnt1);
				CHECK(foundEnt2);
				CHECK(!foundEnt3);
				CHECK(!foundEnt4);
				CHECK(count == 2);
			}

			SECTION("2 Arguments")
			{
				// Query the Entity Database. We expect only our target Entities as a result.
				auto results = CaptureWith<Comp1, Comp2>();

				// The targets will no longer match the query, but they have already been captured.
				ent1->RemoveAllComponents(); ent1->RemoveAllTags();
				ent2->RemoveAllComponents(); ent2->RemoveAllTags();
				ent3->RemoveAllComponents(); ent3->RemoveAllTags();
				ent4->RemoveAllComponents(); ent4->RemoveAllTags();

				for (auto& entity : results)
				{
					if (entity.get() == ent1.get()) foundEnt1 = true;
					if (entity.get() == ent2.get()) foundEnt2 = true;
					if (entity.get() == ent3.get()) foundEnt3 = true;
					if (entity.get() == ent4.get()) foundEnt4 = true;
					count++;
				}

				CHECK(foundEnt1);
				CHECK(foundEnt2);
				CHECK(!foundEnt3);
				CHECK(!foundEnt4);
				CHECK(count == 2);
			}
		}
	}
}
