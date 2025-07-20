#pragma once
#include "../BodyGroup/BodyGroup.h"
#include "../NodeContainer/NodeContainer.h"

namespace db
{
	class BodyGroupsNode : public Node
	{
		NodeContainer m_bodyGroups;

	public:
		BodyGroupsNode(Node* parent) : Node(parent, "BodyGroupsNode", "BodyGroups"), m_bodyGroups(this, "BodyGroups") {};
		virtual ~BodyGroupsNode() {};
		virtual bool isValid() const { return Node::isValid() && m_bodyGroups.isValid(); };
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		void addBodyGroup(BodyGroup* bodyGroup) { m_bodyGroups.addNode(bodyGroup); }
		void removeBodyGroup(int index) { m_bodyGroups.removeNode(index); }
		BodyGroup* getBodyGroup(int index) const { return dynamic_cast<BodyGroup*>(m_bodyGroups.getNode(index)); }
		size_t getNumBodyGroups() const { return m_bodyGroups.getNumNodes(); }
	};
}
