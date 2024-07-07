// Copyright (c) 2017 Emilian Cioca
#include "Input.h"
#include "gemcutter/Application/Application.h"

#include <loupe/loupe.h>
#include <Windows.h>
#include <windowsx.h> // GET_X_LPARAM and GET_Y_LPARAM

namespace
{
	// Resolves Left/Right virtual key codes for Shift/Control/Alt.
	constexpr WPARAM MapLeftRightKeys(WPARAM vk, LPARAM lParam)
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
		return { static_cast<float>(x), static_cast<float>(y) };
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

REFLECT(gem::Key)
	ENUM_VALUES {
		REF_VALUE(Space)
		REF_VALUE(PageUp)
		REF_VALUE(PageDown)
		REF_VALUE(End)
		REF_VALUE(Home)

		REF_VALUE(Left)
		REF_VALUE(Up)
		REF_VALUE(Right)
		REF_VALUE(Down)

		REF_VALUE(LeftShift)
		REF_VALUE(RightShift)
		REF_VALUE(LeftControl)
		REF_VALUE(RightControl)
		REF_VALUE(LeftAlt)
		REF_VALUE(RightAlt)

		REF_VALUE(Insert)
		REF_VALUE(Delete)
		REF_VALUE(Backspace)
		REF_VALUE(CapsLock)
		REF_VALUE(Enter)
		REF_VALUE(Escape)
		REF_VALUE(Tab)

		REF_VALUE(Semicolon)
		REF_VALUE(Plus)
		REF_VALUE(Comma)
		REF_VALUE(Minus)
		REF_VALUE(Period)
		REF_VALUE(ForwardSlash)
		REF_VALUE(Backtick)

		REF_VALUE(BracketLeft)
		REF_VALUE(BackSlash)
		REF_VALUE(BracketRight)
		REF_VALUE(Quote)

		REF_VALUE(Mute)
		REF_VALUE(VolumeDown)
		REF_VALUE(VolumeUp)
		REF_VALUE(NextTrack)
		REF_VALUE(PreviousTrack)
		REF_VALUE(MediaStop)
		REF_VALUE(MediaPlayPause)

		REF_VALUE(KeyBoard0)
		REF_VALUE(KeyBoard1)
		REF_VALUE(KeyBoard2)
		REF_VALUE(KeyBoard3)
		REF_VALUE(KeyBoard4)
		REF_VALUE(KeyBoard5)
		REF_VALUE(KeyBoard6)
		REF_VALUE(KeyBoard7)
		REF_VALUE(KeyBoard8)
		REF_VALUE(KeyBoard9)

		REF_VALUE(Numpad0)
		REF_VALUE(Numpad1)
		REF_VALUE(Numpad2)
		REF_VALUE(Numpad3)
		REF_VALUE(Numpad4)
		REF_VALUE(Numpad5)
		REF_VALUE(Numpad6)
		REF_VALUE(Numpad7)
		REF_VALUE(Numpad8)
		REF_VALUE(Numpad9)
		REF_VALUE(NumpadMultiply)
		REF_VALUE(NumpadPlus)
		REF_VALUE(NumpadSeparator)
		REF_VALUE(NumpadMinus)
		REF_VALUE(NumpadDecimal)
		REF_VALUE(NumpadDivide)

		REF_VALUE(A)
		REF_VALUE(B)
		REF_VALUE(C)
		REF_VALUE(D)
		REF_VALUE(E)
		REF_VALUE(F)
		REF_VALUE(G)
		REF_VALUE(H)
		REF_VALUE(I)
		REF_VALUE(J)
		REF_VALUE(K)
		REF_VALUE(L)
		REF_VALUE(M)
		REF_VALUE(N)
		REF_VALUE(O)
		REF_VALUE(P)
		REF_VALUE(Q)
		REF_VALUE(R)
		REF_VALUE(S)
		REF_VALUE(T)
		REF_VALUE(U)
		REF_VALUE(V)
		REF_VALUE(W)
		REF_VALUE(X)
		REF_VALUE(Y)
		REF_VALUE(Z)

		REF_VALUE(F1)
		REF_VALUE(F2)
		REF_VALUE(F3)
		REF_VALUE(F4)
		REF_VALUE(F5)
		REF_VALUE(F6)
		REF_VALUE(F7)
		REF_VALUE(F8)
		REF_VALUE(F9)
		REF_VALUE(F10)
		REF_VALUE(F11)
		REF_VALUE(F12)

		REF_VALUE(LeftWindows)
		REF_VALUE(RightWindows)

		REF_VALUE(MouseLeft)
		REF_VALUE(MouseRight)
		REF_VALUE(MouseMiddle)

		REF_VALUE(NUM_KEYS, hidden())
	}
REF_END;
