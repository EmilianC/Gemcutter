// Copyright (c) 2017 Emilian Cioca
#include "Input.h"
#include "gemcutter/Application/Application.h"

#include <Windows.h>
#include <windowsx.h> // GET_X_LPARAM and GET_Y_LPARAM

namespace
{
	// Resolves Left/Right virtual key codes for Shift/Control/Alt.
	WPARAM MapLeftRightKeys(WPARAM vk, LPARAM lParam)
	{
		WPARAM new_vk = vk;
		UINT scancode = (lParam & 0x00ff0000) >> 16;
		int extended = (lParam & 0x01000000) != 0;

		switch (vk)
		{
		case VK_SHIFT:
			new_vk = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
			break;

		case VK_CONTROL:
			new_vk = extended ? VK_RCONTROL : VK_LCONTROL;
			break;

		case VK_MENU:
			new_vk = extended ? VK_RMENU : VK_LMENU;
			break;
		}

		return new_vk;
	}
}

namespace gem
{
	InputSingleton Input;

	bool InputSingleton::IsDown(Key key) const
	{
		return keys[static_cast<unsigned>(key)];
	}

	bool InputSingleton::IsUp(Key key) const
	{
		return !IsDown(key);
	}

	int InputSingleton::GetMouseX() const
	{
		return x;
	}

	int InputSingleton::GetMouseY() const
	{
		return y;
	}

	vec2 InputSingleton::GetMousePos() const
	{
		return vec2(static_cast<float>(x), static_cast<float>(y));
	}

	void InputSingleton::SetMousePos(int posX, int posY, bool emitEvent)
	{
		vec2 lastPos(static_cast<float>(x), static_cast<float>(y));
		x = posX;
		y = posY;

		POINT coords { x, -(y - Application.GetScreenHeight()) };
		ClientToScreen(Application.GetWindowHandle(), &coords);
		SetCursorPos(coords.x, coords.y);

		if (emitEvent)
		{
			vec2 pos(static_cast<float>(posX), static_cast<float>(posY));

			EventQueue.Push(std::make_unique<MouseMoved>(pos, pos - lastPos));
		}
	}

	void InputSingleton::SetCursorLock(bool lock)
	{
		cursorLocked = lock;
		if (cursorLocked)
		{
			SetMousePos(Application.GetScreenWidth() / 2, Application.GetScreenHeight() / 2, false);
		}
	}

	bool InputSingleton::IsCursorLocked() const
	{
		return cursorLocked;
	}

	bool InputSingleton::Update(const MSG& msg)
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			{
				vec2 lastPos(static_cast<float>(x), static_cast<float>(y));

				x = GET_X_LPARAM(msg.lParam);
				y = -(GET_Y_LPARAM(msg.lParam) - Application.GetScreenHeight());
				vec2 pos(static_cast<float>(x), static_cast<float>(y));

				EventQueue.Push(std::make_unique<MouseMoved>(pos, pos - lastPos));

				if (cursorLocked)
				{
					SetMousePos(Application.GetScreenWidth() / 2, Application.GetScreenHeight() / 2, false);
				}

				break;
			}

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			// Here we get the basic key value, characters are in uppercase form.
			if ((msg.lParam & 0x40000000) == 0)
			{
				auto key = MapLeftRightKeys(msg.wParam, msg.lParam);

				// We only distribute the event if the previous key-state was KeyUp.
				keys[key] = true;
				EventQueue.Push(std::make_unique<KeyPressed>(static_cast<Key>(key)));
			}
			break;

		case WM_SYSKEYUP:
		case WM_KEYUP:
			{
				auto key = MapLeftRightKeys(msg.wParam, msg.lParam);

				keys[key] = false;
				EventQueue.Push(std::make_unique<KeyReleased>(static_cast<Key>(key)));
				break;
			}

		case WM_LBUTTONDOWN:
			keys[static_cast<unsigned>(Key::MouseLeft)] = true;
			EventQueue.Push(std::make_unique<KeyPressed>(Key::MouseLeft));
			break;

		case WM_LBUTTONUP:
			keys[static_cast<unsigned>(Key::MouseLeft)] = false;
			EventQueue.Push(std::make_unique<KeyReleased>(Key::MouseLeft));
			break;

		case WM_RBUTTONDOWN:
			keys[static_cast<unsigned>(Key::MouseRight)] = true;
			EventQueue.Push(std::make_unique<KeyPressed>(Key::MouseRight));
			break;

		case WM_RBUTTONUP:
			keys[static_cast<unsigned>(Key::MouseRight)] = false;
			EventQueue.Push(std::make_unique<KeyReleased>(Key::MouseRight));
			break;

		case WM_MBUTTONDOWN:
			keys[static_cast<unsigned>(Key::MouseMiddle)] = true;
			EventQueue.Push(std::make_unique<KeyPressed>(Key::MouseMiddle));
			break;

		case WM_MBUTTONUP:
			keys[static_cast<unsigned>(Key::MouseMiddle)] = false;
			EventQueue.Push(std::make_unique<KeyReleased>(Key::MouseMiddle));
			break;

		case WM_MOUSEWHEEL:
			EventQueue.Push(std::make_unique<MouseScrolled>(static_cast<int>(GET_WHEEL_DELTA_WPARAM(msg.wParam) / WHEEL_DELTA)));
			break;

		default:
			// Message was not handled.
			return false;
		}

		return true;
	}

	MouseMoved::MouseMoved(const vec2& _pos, const vec2& _delta)
		: pos(_pos)
		, delta(_delta)
	{
	}

	MouseScrolled::MouseScrolled(int _scroll)
		: scroll(_scroll)
	{
	}

	KeyPressed::KeyPressed(Key _key)
		: key(_key)
	{
	}

	KeyReleased::KeyReleased(Key _key)
		: key(_key)
	{
	}
}
