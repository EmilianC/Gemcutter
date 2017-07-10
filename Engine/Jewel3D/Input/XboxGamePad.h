// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Math/Vector.h"

#include <Windows.h>
#include <Xinput.h>

namespace Jwl
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
		//- Initialize with a game-pad ID to read input from. [0,1,2,3]
		XboxGamePad(u32 gamepadID = 0);

		//- Returns true if the controller is connected and ready to use. Implicitly updates.
		bool IsConnected();

		//- Updates the internal states by polling the device for input.
		void Update();

		//- Set the level of vibration. [0,1]
		void SetVibration(f32 left, f32 right);

		//- Returns the state of a given button.
		bool GetButton(Buttons button) const;

		//- Returns the state of the left trigger. [0,1]
		f32 GetLeftTrigger() const;
		//- Returns the state of the right trigger. [0,1]
		f32 GetRightTrigger() const;

		//- Returns x,y state of the left thumb stick. [-1,1]
		vec2 GetLeftThumbStick() const;
		//- Returns x,y state of the right thumb stick. [-1,1]
		vec2 GetRightThumbStick() const;

		//- Returns the distance of the left thumb stick from the origin. [0,1]
		f32 GetLeftThumbStickMagnitude() const;
		//- Returns the distance of the right thumb stick from the origin. [0,1]
		f32 GetRightThumbStickMagnitude() const;

	private:
		vec2 leftThumbStick;
		vec2 rightThumbStick;
		f32 leftThumbStickMagnitude = 0.0f;
		f32 rightThumbStickMagnitude = 0.0f;

		XINPUT_STATE controllerState;
		u32 gamepadID = 0;
	};
}
