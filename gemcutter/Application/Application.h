// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Application/Event.h"
#include "gemcutter/Rendering/Viewport.h"

#include <functional>
#include <string_view>

struct HWND__; typedef HWND__* HWND;

namespace gem
{
	// Provides an interface to the operating system and the game window.
	// The class also responsible for scheduling and executing the game-loop.
	extern class ApplicationSingleton Application;
	class ApplicationSingleton
	{
		friend class InputSingleton;
	public:
		ApplicationSingleton() = default;
		~ApplicationSingleton();

		bool CreateGameWindow(std::string_view title, unsigned glMajorVersion, unsigned glMinorVersion);
		void DestroyGameWindow();

		// Starts the main game-loop.
		void GameLoop(const std::function<void()>& update, const std::function<void()>& draw);

		// Updates systems provided by the engine.
		// - Dispatches the event queue.
		// - Updates all Engine-Side components.
		// - Steps the Sound System.
		void UpdateEngine();

		// Marks the program to close at the start of the next game-loop.
		void Exit();

		// Shows or hides the default system cursor.
		void ShowCursor(bool visibility);

		bool IsFullscreen() const;
		bool IsBordered() const;
		bool IsResizable() const;
		int GetScreenWidth() const;
		int GetScreenHeight() const;
		float GetAspectRatio() const;

		// Returns the delta-time in seconds for one step of the simulation's step rate.
		// This value is constant between frames. It can only change if SetUpdatesPerSecond() is called.
		float GetDeltaTime() const;

		// The current framerate. Updated once per second.
		unsigned GetFPS() const;

		// Sets the maximum allowed framerate. 0 sets fps as uncapped.
		void SetFPSCap(unsigned fps);
		unsigned GetFPSCap() const;

		void SetUpdatesPerSecond(unsigned ups);
		unsigned GetUpdatesPerSecond() const;

		// Returns the max refresh rate of the system's display.
		unsigned GetSystemRefreshRate() const;
		// Returns the current desktop resolution of the system.
		Viewport GetSystemResolution() const;

		// Gathers all pending events from the OS. This is already called regularly, but you can call
		// it again manually to ensure that the latest input events have been gathered from the system.
		// A call to EventQueue.Dispatch() will still be required to process the queued gemcutter events.
		void GatherSystemEvents();

		// If the game's update rate has fallen behind the target updates per second,
		// this will skip the fast-forwarding effect caused by the update loop catching up to real time.
		// The lost time is ignored. This should be called at the start of a real-time gameplay segment
		// or after loading a high volume of assets and stalling the game loop.
		void SkipToPresentTime();

		const Viewport& GetScreenViewport() const;
		std::string GetOpenGLVersionString() const;

		bool SetFullscreen(bool state);
		bool SetBordered(bool state);
		bool SetResizable(bool state);
		bool SetResolution(unsigned width, unsigned height);

		// Enlarges the window to fit the desktop area. Has no effect when running fullscreen.
		void MaximizeWindow();

	private:
		HWND GetWindowHandle();

		bool appIsRunning = true;

		// The target amount of time between updates.
		__int64 updateStep = 0;
		// The target amount of time between renders.
		__int64 renderStep = 0;

		bool skipToPresent = false;

		bool fullscreen = false;
		bool bordered = true;
		bool resizable = false;

		unsigned updatesPerSecond = 60;
		unsigned FPSCap = 0;

		// The number of frames rendered during the last second.
		unsigned fps = 0;
	};

	// An event distributed by the engine when the game window is resized.
	struct Resize : public Event<Resize>
	{
		Resize(unsigned width, unsigned height);

		// Returns the new aspect ratio of the window, calculated as Width / Height.
		float GetAspectRatio() const;

		// The new Width of the window.
		const unsigned width;
		// The new Height of the window.
		const unsigned height;
	};
}
