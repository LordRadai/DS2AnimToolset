#pragma once
#include "../NodeContainer/NodeContainer.h"

namespace db
{
	class MessagePresetsGroupNode : public Node
	{
		NodeContainer m_messagePresets;

	public:
		MessagePresetsGroupNode(Node* parent)
			: Node(parent, "MessagePresetsGroupNode", "MessagePresetGroups"), m_messagePresets(this, "MessagePresetArray") { };
		virtual ~MessagePresetsGroupNode() {};
		virtual bool isValid() const { return Node::isValid(); }
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		NodeContainer& getMessagePresets() { return m_messagePresets; }
		const NodeContainer& getMessagePresets() const { return m_messagePresets; }
	};
}
