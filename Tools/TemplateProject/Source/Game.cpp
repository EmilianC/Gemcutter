#include "Game.h"

#include <Jewel3D/Application/Logging.h>
#include <Jewel3D/Entity/Components/Camera.h>
#include <Jewel3D/Entity/Components/SoundListener.h>
#include <Jewel3D/Input/Input.h>
#include <Jewel3D/Rendering/Rendering.h>

Game::Game(ConfigTable& _config)
	: config(_config)
{
	// Basic initialization here.
	// ...
}

Game::~Game()
{
	// Cleanup here.
	// ...
}

bool Game::Init()
{
	// Load game assets here.
	// ...
	
	// Setup the camera. It is also the listener for all sound in the scene.
	camera->Add<SoundListener>();
	camera->Add<Camera>(60.0f, Application.GetAspectRatio(), 1.0f, 1000.0f);
	camera->LookAt(vec3(0.0f, 20.0f, 50.0f), vec3(0.0f));

	// Setup the renderer.
	mainRenderPass.SetCamera(camera);

	// Set the background color to cornflower blue.
	SetClearColor(0.35f, 0.7f, 0.9f, 0.0f);

	return true;
}

void Game::Update()
{
	if (Input.IsDown(Key::Escape))
	{
		Application.Exit();
		return;
	}

	// The amount of time in seconds that has passed since the last update.
	float deltaTime = Application.GetDeltaTime();
	
	// Update entities and game logic here.
	// ...

	// Engine systems and components are updated here.
	Application.UpdateEngine();
}

void Game::Draw()
{
	ClearBackBuffer();

	mainRenderPass.Render(*root);
}
