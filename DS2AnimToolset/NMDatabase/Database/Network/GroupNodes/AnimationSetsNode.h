#pragma once
#include "../../AnimationSet/AnimationSet.h"
#include "../../NodeContainer/NodeContainer.h"

namespace db
{
	class AnimationSetsNode : public Node
	{
		NodeContainer m_animationSets;

	public:
		AnimationSetsNode(Node* parent) : Node(parent, "AnimationSetsNode", "AnimationSets"), m_animationSets(this, "AnimationSetArray") {};
		virtual ~AnimationSetsNode() {};
		virtual bool isValid() const { return Node::isValid() && m_animationSets.getNumNodes() > 0; };
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		void addAnimationSet(AnimationSet* animationSet) { m_animationSets.addNode(animationSet); }
		void removeAnimationSet(int index) { m_animationSets.removeNode(index); }
		AnimationSet* getAnimationSet(int index) const { return dynamic_cast<AnimationSet*>(m_animationSets.getNode(index)); }
		size_t getNumAnimationSets() const { return m_animationSets.getNumNodes(); }
	};
}
