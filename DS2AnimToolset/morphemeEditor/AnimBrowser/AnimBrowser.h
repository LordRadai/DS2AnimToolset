#pragma once
#include "AnimObject/AnimObject.h"
#include <vector>

class AnimBrowser
{
public:
	AnimBrowser() {}
	~AnimBrowser() {}

private:
	std::vector<AnimObject*> m_animList;
};