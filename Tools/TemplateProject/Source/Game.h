#pragma once
#include <Jewel3D/Entity/Entity.h>
#include <Jewel3D/Rendering/RenderPass.h>
#include <Jewel3D/Resource/ConfigTable.h>

using namespace Jwl;

class Game
{
public:
	Game(ConfigTable& config);
	~Game();

	bool Init();

	void Update();
	void Draw();

private:
	/* Rendering */
	RenderPass mainRenderPass;

	/* Scene */
	Entity::Ptr camera = Entity::MakeNew("camera");
	Entity::Ptr root = Entity::MakeNew("root");

	/* Assets */
	// The config contains all the properties found in Settings.cfg.
	ConfigTable& config;

};
