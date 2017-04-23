// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Application/Event.h"
#include "Jewel3D/Rendering/Viewport.h"
#include "Jewel3D/Utilities/Singleton.h"

#include <Windows.h>
#include <string>
#include <functional>

namespace Jwl
{
	//- Provides an interface to the Windows OS and runs the game loop.
	static class Application : public Singleton<class Application>
	{
	public:
		Application();
		~Application();

		bool CreateGameWindow(const std::string& title, unsigned glMajorVersion, unsigned glMinorVersion);
		void DestroyGameWindow();

		void GameLoop(std::function<void()> update, std::function<void()> draw);

		//- Updates systems provided by the engine.
		//	1. Dispatches the event queue.
		//	2. Updates all Engine-Side components.
		//	3. Steps the Sound System.
		void UpdateEngine();

		//- Stops the GameLoop.
		void Exit();

		//- Shows the default windows cursor.
		void EnableCursor();

		//- Hides the default windows cursor.
		void DisableCursor();

		bool IsFullscreen();
		bool IsBordered();
		bool IsResizable();
		int GetScreenWidth() const;
		int GetScreenHeight() const;
		float GetAspectRatio() const;

		//- The amount of time in seconds that the simulation step is currently calculating.
		float GetDeltaTime() const;

		//- The current framerate. Updated once per second.
		unsigned GetFPS() const;

		//- Sets the maximum allowed framerate. 0 sets fps as uncapped.
		void SetFPSCap(unsigned fps);
		unsigned GetFPSCap() const;

		void SetUpdatesPerSecond(unsigned ups);
		unsigned GetUpdatesPerSecond() const;

		const Viewport& GetScreenViewport() const;
		std::string GetOpenGLVersionString() const;

		bool SetFullscreen(bool state);
		bool SetBordered(bool state);
		bool SetResizable(bool state);
		bool SetResolution(unsigned width, unsigned height);

	private:
		//- Processes events from the OS.
		void DrainEventQueue();

		//- Windows callback function to handle events directed at our window.
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		bool appIsRunning = true;
		
		//- The target amount of time between updates.
		__int64 updateStep = 0;
		//- The target amount of time between renders.
		__int64 renderStep = 0;
		//- Display variables.
		bool fullscreen = false;
		bool bordered = true;
		bool resizable = false;

		unsigned glMajorVersion;
		unsigned glMinorVersion;
		unsigned updatesPerSecond = 60;
		unsigned FPSCap = 0;
		Viewport screenViewport;

		//- The number of frames render during the last second.
		unsigned fps = 0;

		//- Window handles.
		HWND hwnd;
		HINSTANCE apInstance;
		HGLRC renderContext;
		HDC deviceContext;
	} &Application = Singleton<class Application>::instanceRef;

	//- An event distributed by the engine when the game window is resized.
	class Resize : public Event<Resize>
	{
	public:
		Resize(unsigned width, unsigned height);
		//- Returns the new aspect ratio of the window, calculated as Width / Height.
		float GetAspectRatio() const;
		//- The new Width of the window.
		const unsigned width;
		//- The new Height of the window.
		const unsigned height;
	};
}
