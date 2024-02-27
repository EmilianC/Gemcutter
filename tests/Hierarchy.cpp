#include <catch/catch.hpp>
#include <gemcutter/Entity/Entity.h>
#include <gemcutter/Entity/Hierarchy.h>

using namespace gem;

TEST_CASE("Hierarchy")
{
	auto root = Entity::MakeNewRoot();
	auto e1 = root->Get<Hierarchy>().CreateChild();
	auto e2 = root->Get<Hierarchy>().CreateChild();
	auto e3 = Entity::MakeNew();
	auto e4 = Entity::MakeNew();
	root->Get<Hierarchy>().AddChild(e3);
	e3->Get<Hierarchy>().AddChild(e4);

	SECTION("HierarchyRoot Tag")
	{
		CHECK(root->Has<HierarchyRoot>());
		CHECK(!e1->Has<HierarchyRoot>());
		CHECK(!e2->Has<HierarchyRoot>());
		CHECK(!e3->Has<HierarchyRoot>());
		CHECK(!e4->Has<HierarchyRoot>());

		e1->Get<Hierarchy>().AddChild(e2);
		CHECK(root->Has<HierarchyRoot>());
		CHECK(!e1->Has<HierarchyRoot>());
		CHECK(!e2->Has<HierarchyRoot>());
		CHECK(!e3->Has<HierarchyRoot>());
		CHECK(!e4->Has<HierarchyRoot>());

		root->Get<Hierarchy>().ClearChildren();
		CHECK(root->Has<HierarchyRoot>());
		CHECK(e1->Has<HierarchyRoot>());
		CHECK(!e2->Has<HierarchyRoot>());
		CHECK(e3->Has<HierarchyRoot>());
		CHECK(!e4->Has<HierarchyRoot>());

		e1->Get<Hierarchy>().RemoveChild(*e2);
		CHECK(root->Has<HierarchyRoot>());
		CHECK(e1->Has<HierarchyRoot>());
		CHECK(e2->Has<HierarchyRoot>());
		CHECK(e3->Has<HierarchyRoot>());
		CHECK(!e4->Has<HierarchyRoot>());

		root->Remove<Hierarchy>();
		e1->Remove<Hierarchy>();
		e2->Remove<Hierarchy>();
		e3->Remove<Hierarchy>();
		e4->Remove<Hierarchy>();
		CHECK(!root->Has<HierarchyRoot>());
		CHECK(!e1->Has<HierarchyRoot>());
		CHECK(!e2->Has<HierarchyRoot>());
		CHECK(!e3->Has<HierarchyRoot>());
		CHECK(!e4->Has<HierarchyRoot>());
	}

	SECTION("Removal")
	{
		e1->Remove<Hierarchy>();
		CHECK_FALSE(root->Get<Hierarchy>().IsDescendant(*e1));
		CHECK(root->Get<Hierarchy>().IsDescendant(*e2));
		CHECK(root->Get<Hierarchy>().IsDescendant(*e3));
		CHECK(root->Get<Hierarchy>().IsDescendant(*e4));

		e2->Remove<Hierarchy>();
		CHECK_FALSE(root->Get<Hierarchy>().IsDescendant(*e1));
		CHECK_FALSE(root->Get<Hierarchy>().IsDescendant(*e2));
		CHECK(root->Get<Hierarchy>().IsDescendant(*e3));
		CHECK(root->Get<Hierarchy>().IsDescendant(*e4));

		e3->Remove<Hierarchy>();
		CHECK_FALSE(root->Get<Hierarchy>().IsDescendant(*e1));
		CHECK_FALSE(root->Get<Hierarchy>().IsDescendant(*e2));
		CHECK_FALSE(root->Get<Hierarchy>().IsDescendant(*e3));
		CHECK_FALSE(root->Get<Hierarchy>().IsDescendant(*e4));

		e4->Remove<Hierarchy>();
		CHECK_FALSE(root->Get<Hierarchy>().IsDescendant(*e1));
		CHECK_FALSE(root->Get<Hierarchy>().IsDescendant(*e2));
		CHECK_FALSE(root->Get<Hierarchy>().IsDescendant(*e3));
		CHECK_FALSE(root->Get<Hierarchy>().IsDescendant(*e4));
	}

	SECTION("Parents")
	{
		CHECK(root->Get<Hierarchy>().GetParent() == nullptr);
		CHECK(e1->Get<Hierarchy>().GetParent() == root);
		CHECK(e2->Get<Hierarchy>().GetParent() == root);
		CHECK(e3->Get<Hierarchy>().GetParent() == root);
		CHECK(e4->Get<Hierarchy>().GetParent() == e3);

		CHECK(root->Get<Hierarchy>().GetParentHierarchy() == nullptr);
		CHECK(e1->Get<Hierarchy>().GetParentHierarchy() == &root->Get<Hierarchy>());
		CHECK(e2->Get<Hierarchy>().GetParentHierarchy() == &root->Get<Hierarchy>());
		CHECK(e3->Get<Hierarchy>().GetParentHierarchy() == &root->Get<Hierarchy>());
		CHECK(e4->Get<Hierarchy>().GetParentHierarchy() == &e3->Get<Hierarchy>());

		CHECK(!root->Get<Hierarchy>().IsDescendant(*root));
		CHECK(root->Get<Hierarchy>().IsDescendant(*e1));
		CHECK(root->Get<Hierarchy>().IsDescendant(*e2));
		CHECK(root->Get<Hierarchy>().IsDescendant(*e3));
		CHECK(root->Get<Hierarchy>().IsDescendant(*e4));

		CHECK(!e1->Get<Hierarchy>().IsDescendantOf(*e1));
		CHECK(e1->Get<Hierarchy>().IsDescendantOf(*root));
		CHECK(e2->Get<Hierarchy>().IsDescendantOf(*root));
		CHECK(e3->Get<Hierarchy>().IsDescendantOf(*root));
		CHECK(e4->Get<Hierarchy>().IsDescendantOf(*root));

		CHECK(root->Get<Hierarchy>().GetRoot() == root);
		CHECK(e1->Get<Hierarchy>().GetRoot() == root);
		CHECK(e2->Get<Hierarchy>().GetRoot() == root);
		CHECK(e3->Get<Hierarchy>().GetRoot() == root);
		CHECK(e4->Get<Hierarchy>().GetRoot() == root);

		CHECK(root->Get<Hierarchy>().IsRoot());
		CHECK(!e1->Get<Hierarchy>().IsRoot());
		CHECK(!e2->Get<Hierarchy>().IsRoot());
		CHECK(!e3->Get<Hierarchy>().IsRoot());
		CHECK(!e4->Get<Hierarchy>().IsRoot());
	}

	SECTION("Children")
	{
		CHECK(root->Get<Hierarchy>().GetNumChildren() == 3);
		CHECK(e1->Get<Hierarchy>().GetNumChildren() == 0);
		CHECK(e2->Get<Hierarchy>().GetNumChildren() == 0);
		CHECK(e3->Get<Hierarchy>().GetNumChildren() == 1);
		CHECK(e4->Get<Hierarchy>().GetNumChildren() == 0);

		CHECK(!root->Get<Hierarchy>().IsChild(*root));
		CHECK(root->Get<Hierarchy>().IsChild(*e1));
		CHECK(root->Get<Hierarchy>().IsChild(*e2));
		CHECK(root->Get<Hierarchy>().IsChild(*e3));
		CHECK(!root->Get<Hierarchy>().IsChild(*e4));
		CHECK(!e1->Get<Hierarchy>().IsChild(*e2));
		CHECK(!e2->Get<Hierarchy>().IsChild(*e4));
		CHECK(!e3->Get<Hierarchy>().IsChild(*e1));
		CHECK(!e4->Get<Hierarchy>().IsChild(*e3));

		CHECK(!root->Get<Hierarchy>().IsChildOf(*root));
		CHECK(e1->Get<Hierarchy>().IsChildOf(*root));
		CHECK(e2->Get<Hierarchy>().IsChildOf(*root));
		CHECK(e3->Get<Hierarchy>().IsChildOf(*root));
		CHECK(e4->Get<Hierarchy>().IsChildOf(*e3));
		CHECK(!e4->Get<Hierarchy>().IsChildOf(*root));

		CHECK(!root->Get<Hierarchy>().IsLeaf());
		CHECK(e1->Get<Hierarchy>().IsLeaf());
		CHECK(e2->Get<Hierarchy>().IsLeaf());
		CHECK(!e3->Get<Hierarchy>().IsLeaf());
		CHECK(e4->Get<Hierarchy>().IsLeaf());

		root->Get<Hierarchy>().ClearChildren();
		CHECK(root->Get<Hierarchy>().GetNumChildren() == 0);
		CHECK(!root->Get<Hierarchy>().IsChild(*e1));
		CHECK(!root->Get<Hierarchy>().IsChild(*e2));
		CHECK(!root->Get<Hierarchy>().IsChild(*e3));
		CHECK(!root->Get<Hierarchy>().IsDescendant(*e4));
		CHECK(root->Get<Hierarchy>().IsLeaf());
	}

	SECTION("ForEachChild")
	{
		unsigned count = 0;

		root->Get<Hierarchy>().ForEachChild(false, [&](Entity&) { ++count; });
		CHECK(count == 3);

		count = 0;
		root->Get<Hierarchy>().ForEachChild(true, [&](Entity&) { ++count; });
		CHECK(count == 4);

		count = 0;
		e1->Get<Hierarchy>().ForEachChild(false, [&](Entity&) { ++count; });
		CHECK(count == 0);

		count = 0;
		e2->Get<Hierarchy>().ForEachChild(true, [&](Entity&) { ++count; });
		CHECK(count == 0);
	}
}
