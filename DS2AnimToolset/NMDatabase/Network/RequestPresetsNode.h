#pragma once
#include "../RequestPreset/RequestPreset.h"
#include "../NodeContainer/NodeContainer.h"

namespace db
{
	class RequestPresetsNode : public Node
	{
		NodeContainer m_requestPresets;

	public:
		RequestPresetsNode(Node* parent) : Node(parent, "RequestPresetsNode", "RequestPresets"), m_requestPresets(this, "MessagePresetArray") {};
		virtual ~RequestPresetsNode() {};
		virtual bool isValid() const { return Node::isValid() && m_requestPresets.isValid(); }
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		void addRequestPreset(RequestPreset* preset) { m_requestPresets.addNode(preset); }
		void removeRequestPreset(int index) { m_requestPresets.removeNode(index); }
		RequestPreset* getRequestPreset(int index) const { return static_cast<RequestPreset*>(m_requestPresets.getNode(index)); }
		size_t getNumRequestPresets() const { return m_requestPresets.getNumNodes(); }
	};
}
