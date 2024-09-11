#include "RController.h"

namespace RInput
{
	bool RController::update(float dt)
	{
		if (XInputGetState(0, &this->m_state) == ERROR_SEVERITY_SUCCESS)
		{
			for (WORD key = XINPUT_GAMEPAD_DPAD_UP; key < XINPUT_GAMEPAD_RIGHT_SHOULDER; key = key << 1)
			{
				if (isKeyHeld(key))
					updateHeldTime(key, dt);
				else if (isKeyReleased(key))
					resetHeldTime(key);
			}

			for (WORD key = XINPUT_GAMEPAD_A; key < XINPUT_GAMEPAD_Y; key = key << 1)
			{
				if (isKeyHeld(key))
					updateHeldTime(key, dt);
				else if (isKeyReleased(key))
					resetHeldTime(key);
			}

			this->m_prevState = this->m_state;

			return true;
		}

		return false;
	}

	bool RController::isKeyPressed(WORD key)
	{
		if ((this->m_state.Gamepad.wButtons & key) && !(this->m_prevState.Gamepad.wButtons & key))
			return true;

		return false;
	}

	bool RController::isKeyReleased(WORD key)
	{
		if (!(this->m_state.Gamepad.wButtons & key) && (this->m_prevState.Gamepad.wButtons & key))
			return true;

		return false;
	}

	bool RController::isKeyHeld(WORD key)
	{
		if ((this->m_state.Gamepad.wButtons & key) && (this->m_prevState.Gamepad.wButtons & key))
			return true;

		return false;
	}

	float RController::getHeldTime(WORD key)
	{
		switch (key)
		{
		case XINPUT_GAMEPAD_DPAD_UP:
			return this->m_heldTime[0];
		case XINPUT_GAMEPAD_DPAD_DOWN:
			return this->m_heldTime[1];
		case XINPUT_GAMEPAD_DPAD_LEFT:
			return this->m_heldTime[2];
		case XINPUT_GAMEPAD_DPAD_RIGHT:
			return this->m_heldTime[3];
		case XINPUT_GAMEPAD_START:
			return this->m_heldTime[4];
		case XINPUT_GAMEPAD_BACK:
			return this->m_heldTime[5];
		case XINPUT_GAMEPAD_LEFT_SHOULDER:
			return this->m_heldTime[6];
		case XINPUT_GAMEPAD_RIGHT_SHOULDER:
			return this->m_heldTime[7];
		case XINPUT_GAMEPAD_LEFT_THUMB:
			return this->m_heldTime[8];
		case XINPUT_GAMEPAD_RIGHT_THUMB:
			return this->m_heldTime[9];
		case XINPUT_GAMEPAD_A:
			return this->m_heldTime[10];
		case XINPUT_GAMEPAD_B:
			return this->m_heldTime[11];
		case XINPUT_GAMEPAD_X:
			return this->m_heldTime[12];
		case XINPUT_GAMEPAD_Y:
			return this->m_heldTime[13];
		}
	}

	void RController::updateHeldTime(WORD key, float dt)
	{
		switch (key)
		{
		case XINPUT_GAMEPAD_DPAD_UP:
			this->m_heldTime[0] += dt;
			break;
		case XINPUT_GAMEPAD_DPAD_DOWN:
			this->m_heldTime[1] += dt;
			break;
		case XINPUT_GAMEPAD_DPAD_LEFT:
			this->m_heldTime[2] += dt;
			break;
		case XINPUT_GAMEPAD_DPAD_RIGHT:
			this->m_heldTime[3] += dt;
			break;
		case XINPUT_GAMEPAD_START:
			this->m_heldTime[4] += dt;
			break;
		case XINPUT_GAMEPAD_BACK:
			this->m_heldTime[5] += dt;
			break;
		case XINPUT_GAMEPAD_LEFT_SHOULDER:
			this->m_heldTime[6] += dt;
			break;
		case XINPUT_GAMEPAD_RIGHT_SHOULDER:
			this->m_heldTime[7] += dt;
			break;
		case XINPUT_GAMEPAD_LEFT_THUMB:
			this->m_heldTime[8] += dt;
			break;
		case XINPUT_GAMEPAD_RIGHT_THUMB:
			this->m_heldTime[9] += dt;
			break;
		case XINPUT_GAMEPAD_A:
			this->m_heldTime[10] += dt;
			break;
		case XINPUT_GAMEPAD_B:
			this->m_heldTime[11] += dt;
			break;
		case XINPUT_GAMEPAD_X:
			this->m_heldTime[12] += dt;
			break;
		case XINPUT_GAMEPAD_Y:
			this->m_heldTime[13] += dt;
			break;
		}
	}

	void RController::resetHeldTime(WORD key)
	{
		switch (key)
		{
		case XINPUT_GAMEPAD_DPAD_UP:
			this->m_heldTime[0] = 0.f;
			break;
		case XINPUT_GAMEPAD_DPAD_DOWN:
			this->m_heldTime[1] = 0.f;
			break;
		case XINPUT_GAMEPAD_DPAD_LEFT:
			this->m_heldTime[2] = 0.f;
			break;
		case XINPUT_GAMEPAD_DPAD_RIGHT:
			this->m_heldTime[3] = 0.f;
			break;
		case XINPUT_GAMEPAD_START:
			this->m_heldTime[4] = 0.f;
			break;
		case XINPUT_GAMEPAD_BACK:
			this->m_heldTime[5] = 0.f;
			break;
		case XINPUT_GAMEPAD_LEFT_SHOULDER:
			this->m_heldTime[6] = 0.f;
			break;
		case XINPUT_GAMEPAD_RIGHT_SHOULDER:
			this->m_heldTime[7] = 0.f;
			break;
		case XINPUT_GAMEPAD_LEFT_THUMB:
			this->m_heldTime[8] = 0.f;
			break;
		case XINPUT_GAMEPAD_RIGHT_THUMB:
			this->m_heldTime[9] = 0.f;
			break;
		case XINPUT_GAMEPAD_A:
			this->m_heldTime[10] = 0.f;
			break;
		case XINPUT_GAMEPAD_B:
			this->m_heldTime[11] = 0.f;
			break;
		case XINPUT_GAMEPAD_X:
			this->m_heldTime[12] = 0.f;
			break;
		case XINPUT_GAMEPAD_Y:
			this->m_heldTime[13] = 0.f;
			break;
		}
	}
}