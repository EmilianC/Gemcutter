// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Application/Event.h"
#include "gemcutter/Math/Vector.h"

struct tagMSG;
typedef tagMSG MSG;

namespace gem
{
	enum class Key : uint16_t
	{
		Space = 0x20,
		PageUp,
		PageDown,
		End,
		Home,

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

		Insert = 0x2D,
		Delete = 0x2E,
		Backspace = 0x08,
		CapsLock = 0x14,
		Enter = 0x0D,
		Escape = 0x1B,
		Tab = 0x09,

		Semicolon = 0xBA,
		Plus,
		Comma,
		Minus,
		Period,
		ForwardSlash,
		Backtick,

		BracketLeft = 0xDB,
		BackSlash,
		BracketRight,
		Quote,

		Mute = 0xAD,
		VolumeDown,
		VolumeUp,
		NextTrack,
		PreviousTrack,
		MediaStop,
		MediaPlayPause,

		KeyBoard0 = 0x30, KeyBoard1, KeyBoard2, KeyBoard3, KeyBoard4, KeyBoard5, KeyBoard6, KeyBoard7, KeyBoard8, KeyBoard9,
		Numpad0 = 0x60, Numpad1, Numpad2, Numpad3, Numpad4, Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
		NumpadMultiply,
		NumpadPlus,
		NumpadSeparator,
		NumpadMinus,
		NumpadDecimal,
		NumpadDivide,

		A = 0x41, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		F1 = 0x70, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

		LeftWindows,
		RightWindows,

		MouseLeft = 0x101,
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
		// Moves the system cursor to the given window position.
		// Can optionally emit a MouseMoved event.
		void SetMousePos(int posX, int posY, bool emitEvent);

		// Locks the cursor to the center of the window so that the player's
		// input will not cause the mouse to leave the game's focus.
		void SetCursorLock(bool lock);
		bool IsCursorLocked() const;

	private:
		bool Update(const MSG& msg);

		bool cursorLocked = false;
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
