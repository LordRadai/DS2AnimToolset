#pragma once
#include "../../EmittedControlParameter/EmittedControlParameter.h"
#include "../../NodeContainer/NodeContainer.h"

namespace db
{
	class EmittedControlParametersNode : public Node
	{
		NodeContainer m_emittedControlParameters;

	public:
		EmittedControlParametersNode(Node* parent) : Node(parent, "EmittedControlParametersNode", "EmittedControlParameters"), m_emittedControlParameters(this, "EmittedControlParameterArray") {};
		virtual ~EmittedControlParametersNode() {};
		virtual bool isValid() const { return Node::isValid() && m_emittedControlParameters.isValid(); };
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		void addEmittedControlParameter(EmittedControlParameter* emittedControlParameter) { m_emittedControlParameters.addNode(emittedControlParameter); }
		void removeEmittedControlParameter(int index) { m_emittedControlParameters.removeNode(index); }
		EmittedControlParameter* getEmittedControlParameter(int index) const { return dynamic_cast<EmittedControlParameter*>(m_emittedControlParameters.getNode(index)); }
		size_t getNumEmittedControlParameters() const { return m_emittedControlParameters.getNumNodes(); }
	};
}
