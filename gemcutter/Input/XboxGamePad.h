// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Math/Vector.h"

#include <Windows.h>
#include <Xinput.h>

namespace gem
{
	class XboxGamePad
	{
	public:
		enum class Buttons
		{
			DPadUp = 1,
			DPadDown = 2,
			DPadLeft = 4,
			DPadRight = 8,
			Start = 16,
			Back = 32,
			LeftThumb = 64,
			RightThumb = 128,
			LeftShoulder = 256,
			RightShoulder = 512,
			A = 4096,
			B = 8192,
			X = 16384,
			Y = 32768
		};

		XboxGamePad() = delete;
		// Initialize with a game-pad ID to read input from. [0,1,2,3]
		XboxGamePad(unsigned gamepadID = 0);

		// Returns true if the controller is connected and ready to use. Implicitly updates.
		bool IsConnected();

		// Updates the internal states by polling the device for input.
		void Update();

		// Set the level of vibration. [0,1]
		void SetVibration(float left, float right);

		// Returns the state of a given button.
		bool GetButton(Buttons button) const;

		// Returns the state of the left trigger. [0,1]
		float GetLeftTrigger() const;
		// Returns the state of the right trigger. [0,1]
		float GetRightTrigger() const;

		// Returns x,y state of the left thumb stick. [-1,1]
		vec2 GetLeftThumbStick() const;
		// Returns x,y state of the right thumb stick. [-1,1]
		vec2 GetRightThumbStick() const;

		// Returns the distance of the left thumb stick from the origin. [0,1]
		float GetLeftThumbStickMagnitude() const;
		// Returns the distance of the right thumb stick from the origin. [0,1]
		float GetRightThumbStickMagnitude() const;

	private:
		vec2 leftThumbStick;
		vec2 rightThumbStick;
		float leftThumbStickMagnitude = 0.0f;
		float rightThumbStickMagnitude = 0.0f;

		XINPUT_STATE controllerState;
		unsigned gamepadID = 0;
	};
}
