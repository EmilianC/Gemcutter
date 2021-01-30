// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Application/Event.h"
#include "Jewel3D/Math/Vector.h"

struct tagMSG;
typedef tagMSG MSG;

namespace Jwl
{
	enum class Key
	{
		// Keyboard keys
		Left = 0x25,
		Up,
		Right,
		Down,

		LeftShift = 0xA0,
		RightShift,
		LeftControl,
		RightControl,
		LeftAlt,
		RightAlt,

		Plus = 0xBB,
		Minus = 0xBD,
		Delete = 0x2E,
		Space = 0x20,
		Enter = 0x0D,
		Escape = 0x1B,
		Tab = 0x09,

		KeyBoard0 = '0', KeyBoard1, KeyBoard2, KeyBoard3, KeyBoard4, KeyBoard5, KeyBoard6, KeyBoard7, KeyBoard8, KeyBoard9,
		Numpad0 = 0x60, Numpad1, Numpad2, Numpad3, Numpad4, Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
		F1 = 0x70, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		A = 'A', B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

		// Mouse buttons
		MouseLeft = 257,
		MouseRight,
		MouseMiddle,

		NUM_KEYS
	};

	// Singleton class for managing keyboard and mouse input.
	extern class InputSingleton Input;
	class InputSingleton
	{
		friend class ApplicationSingleton;
	public:
		bool IsDown(Key key) const;
		bool IsUp(Key key) const;

		int GetMouseX() const;
		int GetMouseY() const;
		vec2 GetMousePos() const;

	private:
		bool Update(const MSG& msg);

		bool keys[static_cast<unsigned>(Key::NUM_KEYS)] = { false };
		int x = 0;
		int y = 0;
	};

	// An event distributed by the engine when the mouse position has changed from the previous frame.
	struct MouseMoved : public Event<MouseMoved>
	{
		MouseMoved(const vec2& pos, const vec2& delta);

		// The new mouse position.
		const vec2 pos;
		// The different between this position and the last.
		const vec2 delta;
	};

	// An event distributed by the engine when the mouse wheel is moved.
	struct MouseScrolled : public Event<MouseScrolled>
	{
		MouseScrolled(int scroll);

		// Each positive integer indicates a single roll away from the user, and vice-versa.
		const int scroll;
	};

	// An event distributed by the engine when a key is first pressed.
	struct KeyPressed : public Event<KeyPressed>
	{
		KeyPressed(Key key);

		const Key key;
	};

	// An event distributed by the engine when a key is released after previously being down.
	struct KeyReleased : public Event<KeyReleased>
	{
		KeyReleased(Key key);

		const Key key;
	};
}
