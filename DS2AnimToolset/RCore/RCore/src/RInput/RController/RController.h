#pragma once
#include <Windows.h>
#include <xinput.h>

namespace RInput
{
	class RController
	{
	public:
		RController(int userIdx)
		{
			this->m_userIndex = userIdx;
		}

		~RController() {}

		bool update(float dt);
		bool isKeyPressed(WORD key);
		bool isKeyReleased(WORD key);
		bool isKeyHeld(WORD key);
		float getHeldTime(WORD key);
	private:
		void updateHeldTime(WORD key, float dt);
		void resetHeldTime(WORD key);

		int m_userIndex;
		XINPUT_STATE m_prevState;
		XINPUT_STATE m_state;
		float m_heldTime[14] = { 0.f };
	};
}