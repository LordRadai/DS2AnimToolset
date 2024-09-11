#pragma once
#include "RController/RController.h"

namespace RInput
{
	bool isKeyHeld(int key);
	bool isKeyStateChanged(int key);
	bool isKeyComboPressed(int key1, int key2);
}