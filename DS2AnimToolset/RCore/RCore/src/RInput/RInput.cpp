#include "RInput.h"

namespace RInput
{
	bool isKeyHeld(int key)
	{
		return GetAsyncKeyState(key) & 0x8000;	
	}

	bool isKeyStateChanged(int key)
	{
		return GetAsyncKeyState(key) & 0x0001;
	}

	bool isKeyComboPressed(int key1, int key2)
	{
		return isKeyHeld(key1) && isKeyStateChanged(key2);
	}
}