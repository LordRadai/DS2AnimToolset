#pragma once
#include "RCore.h"
#include "../AnimationSet/AnimationSet.h"

namespace db
{
	class CharacterStartPoint : public Node
	{
		AnimationSet* m_animationSet;

	public:
		CharacterStartPoint(Node* parent, AnimationSet* animationSet) : Node(parent, "StartPoint"), m_animationSet(animationSet) {};
		
		virtual ~CharacterStartPoint() {};
		virtual bool isValid() const;
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		AnimationSet* getAnimationSet() const { return m_animationSet; };
		void setAnimationSet(AnimationSet* animationSet) { m_animationSet = animationSet; };
	};
}