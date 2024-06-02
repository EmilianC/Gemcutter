// Copyright (c) 2017 Emilian Cioca
#include "Application.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Application/Reflection.h"
#include "gemcutter/Application/Timer.h"
#include "gemcutter/GUI/Button.h"
#include "gemcutter/GUI/Widget.h"
#include "gemcutter/Input/Input.h"
#include "gemcutter/Rendering/Light.h"
#include "gemcutter/Rendering/ParticleEmitter.h"
#include "gemcutter/Rendering/Primitives.h"
#include "gemcutter/Rendering/Rendering.h"
#include "gemcutter/Rendering/RenderTarget.h"
#include "gemcutter/Resource/Font.h"
#include "gemcutter/Resource/Model.h"
#include "gemcutter/Resource/Shader.h"
#include "gemcutter/Resource/Texture.h"
#include "gemcutter/Resource/VertexArray.h"
#include "gemcutter/Sound/SoundSystem.h"

#include <glew/glew.h>
#include <glew/wglew.h>
#include <Windows.h>
#if IMGUI_ENABLED
	#include <imgui.h>
	#include <Backends/imgui_impl_Win32.h>
	#include <Backends/imgui_impl_opengl3.h>
#endif

// This is the HINSTANCE of the application.
extern "C" IMAGE_DOS_HEADER __ImageBase;

#define BITS_PER_PIXEL			32
#define DEPTH_BUFFER_BITS		24
#define STENCIL_BUFFER_BITS		8

#define STYLE_BORDERED			(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE)
#define STYLE_BORDERED_FIXED	(WS_CAPTION | WS_SYSMENU | WS_POPUP	| WS_MINIMIZEBOX | WS_VISIBLE)
#define STYLE_BORDERLESS		(WS_POPUP | WS_VISIBLE)
#define STYLE_EXTENDED			(WS_EX_APPWINDOW)

#if IMGUI_ENABLED
	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

namespace
{
	HWND hwnd = NULL;
	HINSTANCE apInstance = NULL;
	HGLRC renderContext = NULL;
	HDC deviceContext = NULL;
	gem::Viewport screenViewport{ 0, 0, 1280, 720 };
	unsigned glMajorVersion = 3;
	unsigned glMinorVersion = 3;

	bool SetVsync(gem::VSyncMode mode)
	{
		if (!wglewIsSupported("WGL_EXT_swap_control"))
		{
			gem::Error("VSync is not supported on this system.");
			return false;
		}

		auto wglSwapIntervalEXT = reinterpret_cast<bool (APIENTRY *)(int)>(wglGetProcAddress("wglSwapIntervalEXT"));
		if (!wglSwapIntervalEXT)
		{
			gem::Error("Could not retrieve \"wglSwapIntervalEXT\" function.");
			return false;
		}

		int modeValue = static_cast<int>(mode);
		if (mode == gem::VSyncMode::Adaptive)
		{
			modeValue = -1;

			if (!wglewIsSupported("WGL_EXT_swap_control_tear"))
			{
				gem::Error("The system does not support Adaptive VSync. VSync mode left unchanged.");
				return false;
			}
		}

		if (!wglSwapIntervalEXT(modeValue))
		{
			gem::Error("Failed to change VSync mode to (%s).", EnumToString(mode));
			return false;
		}

		return true;
	}

	constexpr LONG GetStyle(bool fullScreen, bool bordered, bool resizable)
	{
		if (bordered && !fullScreen)
		{
			return resizable ? STYLE_BORDERED : STYLE_BORDERED_FIXED;
		}

		return STYLE_BORDERLESS;
	}

	RECT GetWindowSize(LONG style, unsigned clientWidth, unsigned clientHeight)
	{
		RECT windowRect {
			.left   = 0,
			.top    = 0,
			.right  = static_cast<LONG>(clientWidth),
			.bottom = static_cast<LONG>(clientHeight)
		};

		if (!AdjustWindowRectEx(&windowRect, style, FALSE, STYLE_EXTENDED))
		{
			gem::Warning("Console: Could not resolve the window's size.");
		}

		return windowRect;
	}

	bool ApplyFullscreen(HWND window, bool state, unsigned clientWidth, unsigned clientHeight)
	{
		if (state)
		{
			DEVMODE dmScreenSettings {
				.dmSize       = sizeof(DEVMODE),
				.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT,
				.dmBitsPerPel = BITS_PER_PIXEL,
				.dmPelsWidth  = clientWidth,
				.dmPelsHeight = clientHeight
			};

			if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				gem::Warning("Console: Could not enter fullscreen mode.");
				return false;
			}

			if (SetWindowPos(window, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW) == ERROR)
			{
				gem::Warning("Console: Could not enter fullscreen mode.");
				return false;
			}
		}
		else
		{
			if (ChangeDisplaySettings(NULL, 0) != DISP_CHANGE_SUCCESSFUL)
			{
				gem::Warning("Console: Could not successfully exit fullscreen mode.");
				return false;
			}

			if (SetWindowPos(window, 0, 20, 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW) == ERROR)
			{
				gem::Warning("Console: Could not successfully exit fullscreen mode.");
				return false;
			}
		}

		return true;
	}

	bool ApplyStyle(HWND window, LONG style, unsigned clientWidth, unsigned clientHeight)
	{
		RECT windowRect = GetWindowSize(style, clientWidth, clientHeight);

		if (SetWindowLongPtr(window, GWL_STYLE, style) == ERROR ||
			SetWindowPos(window, 0, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW) == ERROR)
		{
			gem::Warning("Console: Could not apply the window's border style.");
			return false;
		}

		return true;
	}

	bool ApplyResolution(HWND window, LONG style, unsigned clientWidth, unsigned clientHeight)
	{
		RECT windowRect = GetWindowSize(style, clientWidth, clientHeight);
		if (SetWindowPos(window, 0, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW) == ERROR)
		{
			gem::Warning("Console: Could not apply the resolution.");
			return false;
		}

		return true;
	}

#ifdef GEM_DEBUG
	void CALLBACK OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* /* unused */)
	{
		// Ignored messages.
		if (
			// Usage warning : Generic vertex attribute array # uses a pointer with a small value (#). Is this intended to be used as an offset into a buffer object?
			id == 0x00020004 ||
			// Buffer info : Total VBO memory usage in the system : #
			id == 0x00020070 ||
			// Program / shader state info : GLSL shader # failed to compile.
			id == 0x00020090)
		{
			return;
		}

		char buffer[9] = { '\0' };
		snprintf(buffer, 9, "%.8x", id);

		std::string message;
		message.reserve(128);
		message += "OpenGL(0x";
		message += buffer;
		message += "): ";

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:
			message += "Error\n";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			message += "Deprecated behaviour\n";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			message += "Undefined behaviour\n";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			message += "Portability issue\n";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			message += "Performance issue\n";
			break;
		case GL_DEBUG_TYPE_MARKER:
			message += "Stream annotation\n";
			break;
		case GL_DEBUG_TYPE_OTHER_ARB:
			message += "Other\n";
			break;
		}

		message += "Source: ";
		switch (source)
		{
		case GL_DEBUG_SOURCE_API:
			message += "API\n";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			message += "Window system\n";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			message += "Shader compiler\n";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			message += "Third party\n";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			message += "Application\n";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			message += "Other\n";
			break;
		}

		message += "Severity: ";
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			message += "HIGH\n";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			message += "Medium\n";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			message += "Low\n";
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			message += "Notification\n";
			break;
		}

		message.append(msg, length);
		if (message.back() != '\n')
		{
			message.push_back('\n');
		}

		if (type == GL_DEBUG_TYPE_ERROR)
		{
			gem::Error(message);
		}
		else
		{
			gem::Log(message);
		}
	}
#endif

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_SIZE:
		{
			// Adjust screen data.
			const unsigned width = LOWORD(lParam);
			const unsigned height = HIWORD(lParam);

			if (width > 0 && height > 0)
			{
				screenViewport.width = width;
				screenViewport.height = height;
				screenViewport.bind();

#if IMGUI_ENABLED
				ImGui::GetIO().DisplaySize = {
					static_cast<float>(width),
					static_cast<float>(height),
				};
#endif

				gem::EventQueue.Push(std::make_unique<gem::Resize>(screenViewport.width, screenViewport.height));
			}
			return 0;
		}

		case WM_CREATE:
		{
			hwnd = hWnd;
			deviceContext = GetDC(hWnd);

			if (deviceContext == 0)
			{
				gem::Error("Console: DeviceContext could not be created.");
				return -1;
			}

			/* Initialize OpenGL */
			PIXELFORMATDESCRIPTOR pfd {
				.nSize        = sizeof(PIXELFORMATDESCRIPTOR),
				.nVersion     = 1,
				.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
				.iPixelType   = PFD_TYPE_RGBA,
				.cColorBits   = BITS_PER_PIXEL,
				.cDepthBits   = DEPTH_BUFFER_BITS,
				.cStencilBits = STENCIL_BUFFER_BITS,
				.iLayerType   = PFD_MAIN_PLANE
			};

			int pixelFormat = ChoosePixelFormat(deviceContext, &pfd);
			SetPixelFormat(deviceContext, pixelFormat, &pfd);

			int attribs[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, static_cast<int>(glMajorVersion),
				WGL_CONTEXT_MINOR_VERSION_ARB, static_cast<int>(glMinorVersion),
				WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifdef GEM_DEBUG
				WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,
#else
				WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
#endif
			0 };

			// Create a temporary context so we can get a pointer to our newer context.
			HGLRC tmpContext = wglCreateContext(deviceContext);
			if (!wglMakeCurrent(deviceContext, tmpContext))
			{
				gem::Error("Console: Device-Context could not be made current.\nTry updating your graphics drivers.");
				return -1;
			}

			// Get the function we can use to generate a modern context.
			auto wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
			if (wglCreateContextAttribsARB)
			{
				// Create modern OpenGL context using the new function and delete the old one.
				renderContext = wglCreateContextAttribsARB(deviceContext, 0, attribs);
			}

			if (renderContext == NULL)
			{
				gem::Error("Console: Failed to create OpenGL rendering context.\nTry updating your graphics drivers.");
				return -1;
			}

			// Switch to new context.
			wglMakeCurrent(deviceContext, NULL);
			wglDeleteContext(tmpContext);
			if (!wglMakeCurrent(deviceContext, renderContext))
			{
				gem::Error("Console: Render-Context could not be made current.\nTry updating your graphics drivers.");
				return -1;
			}

			glewExperimental = GL_TRUE;
			auto glewResult = glewInit();
			if (glewResult != GLEW_OK)
			{
				gem::Error("Console: GLEW failed to initialize. ( %s )\nTry updating your graphics drivers.",
					reinterpret_cast<const char*>(glewGetErrorString(glewResult)));

				return -1;
			}

			// Initializing GLEW can sometimes emit an GL_INVALID_ENUM error, so we discard any errors here.
			glGetError();

#ifdef GEM_DEBUG
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OpenGLDebugCallback, NULL);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, NULL, GL_FALSE);
#endif

			// Setup OpenGL settings.
			gem::SetCullFunc(gem::CullFunc::Clockwise);
			gem::SetDepthFunc(gem::DepthFunc::Normal);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glEnable(GL_PRIMITIVE_RESTART);
			glPrimitiveRestartIndex(gem::VertexBuffer::RESTART_INDEX);
			if (GLEW_ARB_seamless_cube_map)
			{
				glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
			}

#if IMGUI_ENABLED
			// Setup Imgui.
			ImGui::CreateContext();
			ImGui::StyleColorsLight();
			ImGuiStyle& style = ImGui::GetStyle();
			style.FrameRounding = 3;
			style.FrameBorderSize = 1;
			style.WindowBorderSize = 0;
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.78f);

			ImGui_ImplWin32_Init(hwnd);
			ImGui_ImplOpenGL3_Init();
			ImGui_ImplOpenGL3_CreateFontsTexture();
#endif
		}
		return 0;

		case WM_CLOSE:
		case WM_DESTROY:
			gem::Application.Exit();
			return 0;

		case WM_SYSCOMMAND:
			// Ignore screen savers and monitor power-saving modes.
			if (wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER)
			{
				return 0;
			}
			break;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

namespace gem
{
	ApplicationSingleton Application;

	ConfigTable LoadApplicationConfig()
	{
		ConfigTable config;
		config.SetDefaultBool("bordered", true);
		config.SetDefaultBool("resizable", false);
		config.SetDefaultBool("fullscreen", false);
		config.SetDefaultInt("resolution_x", 1280);
		config.SetDefaultInt("resolution_y", 720);
		config.SetDefaultInt("openGL_Major", 3);
		config.SetDefaultInt("openGL_Minor", 3);
		config.SetDefaultInt("updatesPerSecond", 120);
		config.SetDefaultInt("FPSCap", 120);
		config.SetDefaultString("vsync", "Adaptive");

		if (!config.Load("settings.cfg"))
		{
			Warning("Could not load \"settings.cfg\". Generating a new default.");

			if (!config.Save("settings.cfg"))
			{
				Warning("Failed to generate a new settings.cfg file.");
			}
		}

		return config;
	}

	void ApplicationSingleton::GatherSystemEvents()
	{
		// Windows message loop.
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);

#if IMGUI_ENABLED
			ImGuiIO& gui = ImGui::GetIO();
			const bool skipWndProc = ImGui_ImplWin32_WndProcHandler(msg.hwnd, msg.message, msg.wParam, msg.lParam);
			const bool skipKeyboardInput = gui.WantCaptureKeyboard;
			const bool skipMouseInput    = gui.WantCaptureMouse;
#else
			const bool skipWndProc       = false;
			const bool skipKeyboardInput = false;
			const bool skipMouseInput    = false;
#endif

			// Filter input events from other system/windows events and send them to the Input class.
			if (msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST && !skipKeyboardInput ||
				msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST && !skipMouseInput)
			{
				if (Input.Update(msg))
				{
					continue;
				}
			}

			if (!skipWndProc)
			{
				// This message was not handled by our input system, send it to our main WndProc.
				DispatchMessage(&msg);
			}
		}
	}

	bool ApplicationSingleton::Startup()
	{
		OpenOutputLog();

#ifdef GEM_DEBUG
		CreateConsoleWindow();
		Log("-- Application Starting Up [DEBUG] --");
#elif defined(GEM_DEV)
		Log("-- Application Starting Up [DEVELOPMENT] --");
#else
		Log("-- Application Starting Up --");
#endif

		InitializeReflectionTables();
		SeedRandomNumberGenerator();

		SoundSystem.Init();

		return true;
	}

	void ApplicationSingleton::Shutdown()
	{
		ASSERT(hwnd == NULL, __FUNCTION__ "() cannot be called while a game window is still active.");

#ifdef GEM_DEBUG
		Log("-- Application Shutting Down [DEBUG] --");
#elif defined(GEM_DEV)
		Log("-- Application Shutting Down [DEVELOPMENT] --");
#else
		Log("-- Application Shutting Down --");
#endif

		SoundSystem.Unload();

#ifdef GEM_DEBUG
		DestroyConsoleWindow();
#endif
		CloseOutputLog();
	}

	bool ApplicationSingleton::CreateGameWindow(std::string_view title, const ConfigTable& config)
	{
		ASSERT(hwnd == NULL, "A game window is already open.");

		bool success = true;
		auto CheckSetting = [&](std::string_view setting) {
			if (!config.HasSetting(setting))
			{
				Error("Config is missing the (%s) field.", setting.data());
				success = false;
			}
		};

		CheckSetting("bordered");
		CheckSetting("resizable");
		CheckSetting("fullscreen");
		CheckSetting("resolution_x");
		CheckSetting("resolution_y");
		CheckSetting("openGL_Major");
		CheckSetting("openGL_Minor");
		CheckSetting("updatesPerSecond");
		CheckSetting("FPSCap");
		CheckSetting("vsync");

		if (!success)
		{
			return false;
		}

		return CreateGameWindow(title,
			config.GetInt("openGL_Major"), config.GetInt("openGL_Minor"),
			config.GetInt("resolution_x"), config.GetInt("resolution_y"),
			config.GetBool("fullscreen"), config.GetBool("bordered"), config.GetBool("resizable"),
			StringToEnum<VSyncMode>(config.GetString("vsync")).value(),
			config.GetInt("updatesPerSecond"), config.GetInt("FPSCap")
		);
	}

	bool ApplicationSingleton::CreateGameWindow(std::string_view title, unsigned _glMajorVersion, unsigned _glMinorVersion)
	{
		ASSERT(hwnd == NULL, "A game window is already open.");
		ASSERT(_glMajorVersion > 3 || (_glMajorVersion == 3 && _glMinorVersion == 3), "OpenGL version must be 3.3 or greater.");

		apInstance = reinterpret_cast<HINSTANCE>(&__ImageBase);
		glMajorVersion = _glMajorVersion;
		glMinorVersion = _glMinorVersion;

		WNDCLASSEX windowClass {
			.cbSize        = sizeof(WNDCLASSEX),
			.style         = CS_HREDRAW | CS_VREDRAW,
			.lpfnWndProc   = WindowProc,
			.cbClsExtra    = 0,
			.cbWndExtra    = 0,
			.hInstance     = apInstance,
			.hIcon         = LoadIcon(NULL, IDI_APPLICATION), // Default icon.
			.hCursor       = LoadCursor(NULL, IDC_ARROW),     // Default arrow.
			.hbrBackground = NULL,
			.lpszMenuName  = NULL,
			.lpszClassName = "Gemcutter",
			.hIconSm       = LoadIcon(NULL, IDI_WINLOGO)      // Windows logo small icon.
		};

		if (!RegisterClassEx(&windowClass))
		{
			Error("Console: Failed to register window.");
			return false;
		}

		LONG style = GetStyle(fullscreen, bordered, resizable);
		RECT windowRect = GetWindowSize(style, screenViewport.width, screenViewport.height);

#ifdef GEM_DEBUG
		std::string windowTitle = std::string(title) + " [DEBUG]";
#elif defined(GEM_DEV)
		std::string windowTitle = std::string(title) + " [DEVELOPMENT]";
#else
		const auto& windowTitle = title;
#endif

		// This will internally send a WM_CREATE message that is handled before the function returns.
		if (!CreateWindowEx(
			STYLE_EXTENDED,
			"Gemcutter",  // Class name.
			windowTitle.data(),
			style,
			CW_USEDEFAULT, CW_USEDEFAULT,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			NULL,         // Handle to parent.
			NULL,         // Handle to menu.
			apInstance,
			NULL))
		{
			Error("Console: Failed to create window.");
			return false;
		}

		GPUInfo.ScanDevice();
		Shader::BuildCommonHeader();
		RenderTarget::InitDrawFlags();

		ShowWindow(hwnd, SW_SHOW);
		SetForegroundWindow(hwnd);

		SetVsync(vsyncMode);

		if (!Primitives.Load())
		{
			return false;
		}

		// Ensure that the deltaTime between frames has been computed.
		SetUpdatesPerSecond(updatesPerSecond);

		Log("Initialized Window. OpenGL version: " + Application.GetOpenGLVersionString());
		return true;
	}

	bool ApplicationSingleton::CreateGameWindow(std::string_view title,
		unsigned _glMajorVersion, unsigned _glMinorVersion,
		unsigned width, unsigned height,
		bool _fullscreen, bool _bordered, bool _resizable,
		VSyncMode _vsyncMode, int _updatesPerSecond, int _FPSCap)
	{
		ASSERT(hwnd == NULL, "A game window is already open.");

		{
			RECT desktop = {};
			const bool safeDesktopResolution = GetWindowRect(GetDesktopWindow(), &desktop) == TRUE;

			if (width == 0)
			{
				width = safeDesktopResolution ? static_cast<int>(desktop.right) : 1280;
			}

			if (height == 0)
			{
				height = safeDesktopResolution ? static_cast<int>(desktop.bottom) : 720;
			}

			SetResolution(width, height);
		}

		SetBordered(_bordered);
		SetResizable(_resizable);
		SetFullscreen(_fullscreen);
		SetUpdatesPerSecond(_updatesPerSecond);
		SetFPSCap(_FPSCap);
		vsyncMode = _vsyncMode;

		return CreateGameWindow(title, _glMajorVersion, _glMinorVersion);
	}

	void ApplicationSingleton::DestroyGameWindow()
	{
		if (hwnd == NULL)
			return;

		// Delete all resources that require the OpenGL context.
		UnloadAll<Font>();
		UnloadAll<Shader>();
		UnloadAll<Texture>();
		UnloadAll<Model>();

		Primitives.Unload();

		if (IsFullscreen())
		{
			SetFullscreen(false);
		}

		ShowCursor(true);

#ifdef GEM_DEBUG
		// Some drivers will emit erroneous errors on shutdown, so we disable debug info first.
		glDebugMessageCallback(NULL, NULL);
#endif
		// Release Device and Render Contexts.
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(renderContext);
		ReleaseDC(hwnd, deviceContext);
		DestroyWindow(hwnd);
		UnregisterClass("Gemcutter", apInstance);

		renderContext = NULL;
		deviceContext = NULL;
		hwnd = NULL;
	}

	void ApplicationSingleton::GameLoop(void (*updateFunc)(), void (*drawFunc)())
	{
		ASSERT(hwnd != NULL, "A window must be created before a calling " __FUNCTION__ "().");
		ASSERT(updateFunc, "An update function must be provided.");
		ASSERT(drawFunc, "A draw function must be provided.");

		// Timing control variables.
		constexpr unsigned MAX_CONCURRENT_UPDATES = 5;
		unsigned fpsCounter = 0;
		int64_t lastUpdate = Timer::GetCurrentTick();
		int64_t lastRender = lastUpdate;
		int64_t lastFpsCapture = lastRender;

		while (true)
		{
			// Updates our input and Windows OS events.
			GatherSystemEvents();
			if (!appIsRunning) [[unlikely]]
				return;

				// Record the FPS for the previous second of time.
				int64_t currentTime = Timer::GetCurrentTick();
				if (currentTime - lastFpsCapture >= Timer::GetTicksPerSecond())
				{
					// We don't want to update the timer variable with "+= 1.0" here. After a lag spike this
					// would cause FPS to suddenly be recorded more often than once a second.
					lastFpsCapture = currentTime;

					fps = fpsCounter;
					fpsCounter = 0;
				}

				// Update to keep up with real time.
				unsigned updateCount = 0;
				while (currentTime - lastUpdate >= updateStep)
				{
#if IMGUI_ENABLED
					ImGui_ImplWin32_NewFrame();
					ImGui_ImplOpenGL3_NewFrame();
					ImGui::NewFrame();
					updateFunc();
					ImGui::EndFrame();
#else
					updateFunc();
#endif
					// The user might have requested to exit during update().
					if (!appIsRunning) [[unlikely]]
						return;

						lastUpdate += updateStep;
						updateCount++;

						if (skipToPresent) [[unlikely]]
							{
								lastUpdate = currentTime;
								skipToPresent = false;
								break;
							}

								// Avoid spiral of death. This also allows us to keep rendering even in a worst-case scenario.
								if (updateCount >= MAX_CONCURRENT_UPDATES)
									break;
				}

				if (updateCount > 0)
				{
					// If the frame rate is uncapped or we are due for a new frame, render the latest game-state.
					if (FPSCap == 0 || (currentTime - lastRender) >= renderStep)
					{
						drawFunc();
#if IMGUI_ENABLED
						ImGui::Render();
						ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
						SwapBuffers(deviceContext);

						lastRender += renderStep;
						fpsCounter++;
					}
				}
		}
	}

	void ApplicationSingleton::UpdateEngine()
	{
		// Distribute all queued events to their listeners.
		EventQueue.Dispatch();

		// Update engine components.
		Widget::UpdateAll();

		for (auto& button : All<Button>())
		{
			button.Update();
		}

		for (auto& emitter : All<ParticleEmitter>())
		{
			emitter.Update();
		}

		for (auto& light : All<Light>())
		{
			light.Update();
		}

		// Step the SoundSystem.
		SoundSystem.Update();
	}

	void ApplicationSingleton::Exit()
	{
		appIsRunning = false;
	}

	void ApplicationSingleton::ShowCursor(bool visibility)
	{
		if (visibility)
		{
			while (::ShowCursor(true) <= 0);
		}
		else
		{
			while (::ShowCursor(false) >= 0);
		}
	}

	bool ApplicationSingleton::IsFullscreen() const
	{
		return fullscreen;
	}

	bool ApplicationSingleton::IsBordered() const
	{
		return bordered;
	}

	bool ApplicationSingleton::IsResizable() const
	{
		return resizable;
	}

	std::string ApplicationSingleton::GetOpenGLVersionString() const
	{
		ASSERT(hwnd != NULL, "A game window must be created before calling " __FUNCTION__ "().");

		return reinterpret_cast<const char*>(glGetString(GL_VERSION));
	}

	int ApplicationSingleton::GetScreenWidth() const
	{
		return screenViewport.width;
	}

	int ApplicationSingleton::GetScreenHeight() const
	{
		return screenViewport.height;
	}

	float ApplicationSingleton::GetAspectRatio() const
	{
		return screenViewport.GetAspectRatio();
	}

	float ApplicationSingleton::GetDeltaTime() const
	{
		return 1.0f / updatesPerSecond;
	}

	unsigned ApplicationSingleton::GetFPS() const
	{
		return fps;
	}

	void ApplicationSingleton::SetFPSCap(unsigned _fps)
	{
		if (_fps == 0)
		{
			renderStep = 0;
		}
		else
		{
			renderStep = Timer::GetTicksPerSecond() / _fps;
		}

		FPSCap = _fps;

		if (FPSCap > updatesPerSecond)
		{
			Warning("FPSCap is higher than the Update rate. Frames cannot be rendered more often than there are updates.");
		}
	}

	unsigned ApplicationSingleton::GetFPSCap() const
	{
		return FPSCap;
	}

	void ApplicationSingleton::SetUpdatesPerSecond(unsigned ups)
	{
		ASSERT(ups > 0, "Invalid update rate.");

		updateStep = Timer::GetTicksPerSecond() / ups;

		updatesPerSecond = ups;
	}

	unsigned ApplicationSingleton::GetUpdatesPerSecond() const
	{
		return updatesPerSecond;
	}

	unsigned ApplicationSingleton::GetSystemRefreshRate() const
	{
		DEVMODE devMode {
			.dmSize = sizeof(DEVMODE),
			.dmDriverExtra = 0
		};

		if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode))
		{
			return static_cast<unsigned>(devMode.dmDisplayFrequency);
		}

		Warning("Could not retrieve the display's refresh rate. Assuming 60hz.");
		return 60;
	}

	Viewport ApplicationSingleton::GetSystemResolution() const
	{
		Viewport result;
		RECT desktop;
		if (GetWindowRect(GetDesktopWindow(), &desktop))
		{
			result.width = desktop.right;
			result.height = desktop.bottom;
		}
		else
		{
			Warning("Could not retrieve the system display's resolution. Assuming 1920x1080.");
			result.width = 1920;
			result.height = 1080;
		}

		return result;
	}

	void ApplicationSingleton::SkipToPresentTime()
	{
		skipToPresent = true;
	}

	const Viewport& ApplicationSingleton::GetScreenViewport() const
	{
		return screenViewport;
	}

	bool ApplicationSingleton::SetVsyncMode(VSyncMode mode)
	{
		if (hwnd != NULL)
		{
			if (mode == vsyncMode)
			{
				return true;
			}

			if (!SetVsync(mode))
			{
				return false;
			}
		}

		vsyncMode = mode;
		return true;
	}

	VSyncMode ApplicationSingleton::GetVsyncMode() const
	{
		return vsyncMode;
	}

	bool ApplicationSingleton::SetFullscreen(bool state)
	{
		if (hwnd != NULL)
		{
			if (state == fullscreen)
			{
				return true;
			}

			LONG style;
			if (state)
			{
				style = GetStyle(true, false, false);
			}
			else
			{
				style = GetStyle(false, IsBordered(), IsResizable());
			}

			if (!ApplyStyle(hwnd, style, GetScreenWidth(), GetScreenHeight()))
			{
				return false;
			}

			if (!ApplyFullscreen(hwnd, state, GetScreenWidth(), GetScreenHeight()))
			{
				return false;
			}
		}

		fullscreen = state;
		return true;
	}

	bool ApplicationSingleton::SetBordered(bool state)
	{
		if (hwnd != NULL)
		{
			if (state == bordered)
			{
				return true;
			}

			if (IsFullscreen())
			{
				// State will be applied next time we exit fullscreen mode.
				bordered = state;
				return true;
			}

			LONG style = GetStyle(false, state, IsResizable());
			if (!ApplyStyle(hwnd, style, GetScreenWidth(), GetScreenHeight()))
			{
				return false;
			}
		}

		bordered = state;
		return true;
	}

	bool ApplicationSingleton::SetResizable(bool state)
	{
		if (hwnd != NULL)
		{
			if (state == resizable)
			{
				return true;
			}

			if (IsFullscreen() || !IsBordered())
			{
				// State will be applied next time we exit fullscreen mode and have a border.
				resizable = state;
				return true;
			}

			LONG style = GetStyle(false, true, state);
			if (!ApplyStyle(hwnd, style, GetScreenWidth(), GetScreenHeight()))
			{
				return false;
			}
		}

		resizable = state;
		return true;
	}

	bool ApplicationSingleton::SetResolution(unsigned width, unsigned height)
	{
		ASSERT(width > 0, "'width' must be greater than 0.");
		ASSERT(height > 0, "'height' must be greater than 0.");

		bool wasFullscreen = fullscreen;

		if (hwnd != NULL)
		{
			if (width == screenViewport.width && height == screenViewport.height)
			{
				return true;
			}

			if (wasFullscreen && !SetFullscreen(false))
			{
				return false;
			}

			LONG style = GetStyle(fullscreen, IsBordered(), IsResizable());
			if (!ApplyResolution(hwnd, style, width, height))
			{
				return false;
			}
		}

		screenViewport.width = width;
		screenViewport.height = height;

		if (hwnd != NULL)
		{
			if (wasFullscreen && !SetFullscreen(true))
			{
				return false;
			}
		}

		return true;
	}

	void ApplicationSingleton::MaximizeWindow()
	{
		ASSERT(hwnd != NULL, "A game window must be created before calling MaximizeWindow().");

		if (IsFullscreen())
			return;

		ShowWindow(hwnd, SW_MAXIMIZE);
	}

	HWND ApplicationSingleton::GetWindowHandle()
	{
		return hwnd;
	}

	ApplicationSingleton::~ApplicationSingleton()
	{
		if (hwnd != NULL)
		{
			DestroyGameWindow();
		}
	}

	Resize::Resize(unsigned _width, unsigned _height)
		: width(_width)
		, height(_height)
	{
	}

	float Resize::GetAspectRatio() const
	{
		return static_cast<float>(width) / static_cast<float>(height);
	}
}

REFLECT(gem::VSyncMode)
	ENUM_VALUES {
		REF_VALUE(Off)
		REF_VALUE(On)
		REF_VALUE(Adaptive)
	}
REF_END;
