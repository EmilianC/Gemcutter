// Copyright (c) 2017 Emilian Cioca
#include "XboxGamePad.h"
#include "gemcutter/Application/Logging.h"

#include <loupe/loupe.h>
#include <Windows.h>
#include <Xinput.h>

namespace gem
{
	constexpr float MAX_VAL = 32767.0f;

	XboxGamePad::XboxGamePad(unsigned gamepadID)
		: gamepadID(gamepadID)
	{
		ASSERT(gamepadID < 4, "XboxGamePad can only get input from up to four game-pads.");

		controllerState = static_cast<XINPUT_STATE*>(calloc(1, sizeof(XINPUT_STATE)));
	}

	XboxGamePad::~XboxGamePad()
	{
		free(controllerState);
	}

	bool XboxGamePad::IsConnected()
	{
		memset(controllerState, 0, sizeof(XINPUT_STATE));

		return XInputGetState(gamepadID, controllerState) == ERROR_SUCCESS;
	}

	void XboxGamePad::Update()
	{
		memset(controllerState, 0, sizeof(XINPUT_STATE));
		XInputGetState(gamepadID, controllerState);

		leftThumbStick.x = static_cast<float>(controllerState->Gamepad.sThumbLX);
		leftThumbStick.y = static_cast<float>(controllerState->Gamepad.sThumbLY);
		leftThumbStickMagnitude = Length(leftThumbStick);

		rightThumbStick.x = static_cast<float>(controllerState->Gamepad.sThumbRX);
		rightThumbStick.y = static_cast<float>(controllerState->Gamepad.sThumbRY);
		rightThumbStickMagnitude = Length(rightThumbStick);
	}

	void XboxGamePad::SetVibration(float left, float right)
	{
		ASSERT(left >= 0.0f && left <= 1.0f, "XboxGamePad: 'left' vibration value must be in range of [0, 1].");
		ASSERT(right >= 0.0f && right <= 1.0f, "XboxGamePad: 'right' vibration value must be in range of [0, 1].");

		XINPUT_VIBRATION vibration {
			.wLeftMotorSpeed  = static_cast<WORD>(left  * MAX_VAL),
			.wRightMotorSpeed = static_cast<WORD>(right * MAX_VAL)
		};

		XInputSetState(gamepadID, &vibration);
	}

	bool XboxGamePad::GetButton(Buttons button) const
	{
		return (controllerState->Gamepad.wButtons & static_cast<WORD>(button)) != 0;
	}

	float XboxGamePad::GetLeftTrigger() const
	{
		if (controllerState->Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{
			return static_cast<float>(controllerState->Gamepad.bLeftTrigger) / 255.0f;
		}
		else
		{
			return 0.0f;
		}
	}

	float XboxGamePad::GetRightTrigger() const
	{
		if (controllerState->Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{
			return static_cast<float>(controllerState->Gamepad.bRightTrigger) / 255.0f;
		}
		else
		{
			return 0.0f;
		}
	}

	vec2 XboxGamePad::GetLeftThumbStick() const
	{
		if (leftThumbStickMagnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			return leftThumbStick / MAX_VAL;
		}
		else
		{
			return vec2::Zero;
		}
	}

	vec2 XboxGamePad::GetRightThumbStick() const
	{
		if (rightThumbStickMagnitude > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			return rightThumbStick / MAX_VAL;
		}
		else
		{
			return vec2::Zero;
		}
	}

	float XboxGamePad::GetLeftThumbStickMagnitude() const
	{
		if (leftThumbStickMagnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			return leftThumbStickMagnitude;
		}
		else
		{
			return 0.0f;
		}
	}

	float XboxGamePad::GetRightThumbStickMagnitude() const
	{
		if (rightThumbStickMagnitude > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			return rightThumbStickMagnitude;
		}
		else
		{
			return 0.0f;
		}
	}
}

REFLECT(gem::XboxGamePad::Buttons)
	ENUM_VALUES {
		REF_VALUE(DPadUp)
		REF_VALUE(DPadDown)
		REF_VALUE(DPadLeft)
		REF_VALUE(DPadRight)
		REF_VALUE(Start)
		REF_VALUE(Back)
		REF_VALUE(LeftThumb)
		REF_VALUE(RightThumb)
		REF_VALUE(LeftShoulder)
		REF_VALUE(RightShoulder)
		REF_VALUE(A)
		REF_VALUE(B)
		REF_VALUE(X)
		REF_VALUE(Y)
	}
REF_END;
