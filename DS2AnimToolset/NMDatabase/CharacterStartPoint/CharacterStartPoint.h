#pragma once
#include "RCore.h"
#include "../AnimationSet/AnimationSet.h"

namespace db
{
	class CharacterStartPoint : public Node
	{
		AnimationSet* m_animationSet;

	public:
		CharacterStartPoint(AnimationSet* animationSet) : Node("StartPoint"), m_animationSet(animationSet) {};
		~CharacterStartPoint() {};

		AnimationSet* getAnimationSet() const { return m_animationSet; };
		void setAnimationSet(AnimationSet* animationSet) { m_animationSet = animationSet; };
	};
}