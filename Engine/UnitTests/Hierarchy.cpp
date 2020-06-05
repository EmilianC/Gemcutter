#include <catch.hpp>
#include <Jewel3D/Entity/Entity.h>
#include <Jewel3D/Entity/Hierarchy.h>

using namespace Jwl;

TEST_CASE("Hierarchy")
{
	auto root = Entity::MakeNewRoot();
	auto e1 = root->Get<Hierarchy>().CreateChild();
	auto e2 = root->Get<Hierarchy>().CreateChild();
	auto e3 = Entity::MakeNew();
	root->Get<Hierarchy>().AddChild(e3);

	SECTION("HierarchyRoot Tag")
	{
		CHECK(root->HasTag<HierarchyRoot>());
		CHECK(!e1->HasTag<HierarchyRoot>());
		CHECK(!e2->HasTag<HierarchyRoot>());
		CHECK(!e3->HasTag<HierarchyRoot>());

		e1->Get<Hierarchy>().AddChild(e2);
		CHECK(root->HasTag<HierarchyRoot>());
		CHECK(!e1->HasTag<HierarchyRoot>());
		CHECK(!e2->HasTag<HierarchyRoot>());
		CHECK(!e3->HasTag<HierarchyRoot>());

		root->Get<Hierarchy>().ClearChildren();
		CHECK(root->HasTag<HierarchyRoot>());
		CHECK(e1->HasTag<HierarchyRoot>());
		CHECK(!e2->HasTag<HierarchyRoot>());
		CHECK(e3->HasTag<HierarchyRoot>());

		e1->Get<Hierarchy>().RemoveChild(*e2);
		CHECK(root->HasTag<HierarchyRoot>());
		CHECK(e1->HasTag<HierarchyRoot>());
		CHECK(e2->HasTag<HierarchyRoot>());
		CHECK(e3->HasTag<HierarchyRoot>());

		root->Remove<Hierarchy>();
		e1->Remove<Hierarchy>();
		e2->Remove<Hierarchy>();
		e3->Remove<Hierarchy>();
		CHECK(!root->HasTag<HierarchyRoot>());
		CHECK(!e1->HasTag<HierarchyRoot>());
		CHECK(!e2->HasTag<HierarchyRoot>());
		CHECK(!e3->HasTag<HierarchyRoot>());
	}

	SECTION("Parents")
	{
		CHECK(root->Get<Hierarchy>().GetParent() == nullptr);
		CHECK(e1->Get<Hierarchy>().GetParent() == root);
		CHECK(e2->Get<Hierarchy>().GetParent() == root);
		CHECK(e3->Get<Hierarchy>().GetParent() == root);

		CHECK(root->Get<Hierarchy>().GetRoot() == root);
		CHECK(e1->Get<Hierarchy>().GetRoot() == root);
		CHECK(e2->Get<Hierarchy>().GetRoot() == root);
		CHECK(e3->Get<Hierarchy>().GetRoot() == root);

		CHECK(root->Get<Hierarchy>().IsRoot());
		CHECK(!e1->Get<Hierarchy>().IsRoot());
		CHECK(!e2->Get<Hierarchy>().IsRoot());
		CHECK(!e3->Get<Hierarchy>().IsRoot());
	}

	SECTION("Children")
	{
		CHECK(root->Get<Hierarchy>().GetNumChildren() == 3);
		CHECK(e1->Get<Hierarchy>().GetNumChildren() == 0);
		CHECK(e2->Get<Hierarchy>().GetNumChildren() == 0);
		CHECK(e3->Get<Hierarchy>().GetNumChildren() == 0);

		CHECK(root->Get<Hierarchy>().IsChild(*e1));
		CHECK(root->Get<Hierarchy>().IsChild(*e2));
		CHECK(root->Get<Hierarchy>().IsChild(*e3));
		CHECK(!root->Get<Hierarchy>().IsChild(*root));
		CHECK(!e1->Get<Hierarchy>().IsChild(*e2));
		CHECK(!e2->Get<Hierarchy>().IsChild(*e3));
		CHECK(!e3->Get<Hierarchy>().IsChild(*e1));

		CHECK(!root->Get<Hierarchy>().IsLeaf());
		CHECK(e1->Get<Hierarchy>().IsLeaf());
		CHECK(e2->Get<Hierarchy>().IsLeaf());
		CHECK(e3->Get<Hierarchy>().IsLeaf());

		root->Get<Hierarchy>().ClearChildren();
		CHECK(root->Get<Hierarchy>().GetNumChildren() == 0);
		CHECK(!root->Get<Hierarchy>().IsChild(*e1));
		CHECK(!root->Get<Hierarchy>().IsChild(*e2));
		CHECK(!root->Get<Hierarchy>().IsChild(*e3));
		CHECK(root->Get<Hierarchy>().IsLeaf());
	}
}
