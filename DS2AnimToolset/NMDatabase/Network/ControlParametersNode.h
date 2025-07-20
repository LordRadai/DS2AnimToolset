#pragma once
#include "../NodeContainer/NodeContainer.h"
#include "../ControlParameter/ControlParameter.h"

namespace db
{
	class ControlParametersNode : public Node
	{
		NodeContainer m_controlParameters;

	public:
		ControlParametersNode(Node* parent) : Node(parent, "ControlParametersNode", "ControlParameters"), m_controlParameters(this, "ControlParameterArray") {};
		virtual ~ControlParametersNode() {};
		virtual bool isValid() const { return Node::isValid() && m_controlParameters.isValid(); }
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		void addControlParameter(Node* controlParameter) { m_controlParameters.addNode(controlParameter); }
		void removeControlParameter(int index) { m_controlParameters.removeNode(index); }
		ControlParameter* getControlParameter(int index) const { return dynamic_cast<ControlParameter*>(m_controlParameters.getNode(index)); }
		size_t getNumControlParameters() const { return m_controlParameters.getNumNodes(); }
	};
}
