#include <catch.hpp>
#include <Jewel3D/Entity/Entity.h>

using namespace Jwl;

TEST_CASE("Hierarchy")
{
	auto root = Entity::MakeNew();
	auto e1 = root->CreateChild();
	auto e2 = root->CreateChild();
	auto e3 = Entity::MakeNew();
	root->AddChild(e3);

	SECTION("Parents")
	{
		CHECK(root->GetParent() == nullptr);
		CHECK(e1->GetParent() == root);
		CHECK(e2->GetParent() == root);
		CHECK(e3->GetParent() == root);

		CHECK(root->GetRoot() == root);
		CHECK(e1->GetRoot() == root);
		CHECK(e2->GetRoot() == root);
		CHECK(e3->GetRoot() == root);

		CHECK(root->IsRoot());
		CHECK(!e1->IsRoot());
		CHECK(!e2->IsRoot());
		CHECK(!e3->IsRoot());
	}

	SECTION("Children")
	{
		CHECK(root->GetNumChildren() == 3);
		CHECK(e1->GetNumChildren() == 0);
		CHECK(e2->GetNumChildren() == 0);
		CHECK(e3->GetNumChildren() == 0);

		CHECK(root->IsChild(*e1));
		CHECK(root->IsChild(*e2));
		CHECK(root->IsChild(*e3));
		CHECK(!root->IsChild(*root));
		CHECK(!e1->IsChild(*e2));
		CHECK(!e2->IsChild(*e3));
		CHECK(!e3->IsChild(*e1));

		CHECK(!root->IsLeaf());
		CHECK(e1->IsLeaf());
		CHECK(e2->IsLeaf());
		CHECK(e3->IsLeaf());

		root->ClearChildren();
		CHECK(root->GetNumChildren() == 0);
		CHECK(!root->IsChild(*e1));
		CHECK(!root->IsChild(*e2));
		CHECK(!root->IsChild(*e3));
		CHECK(root->IsLeaf());
	}
}
