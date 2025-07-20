#pragma once
#include "RCore.h"
#include "../ControlParameter/ControlParameter.h"
#include "../Node/Node.h"

namespace db
{
	class EmittedControlParameter : public Node
	{
		ControlParameter* m_controlParameter;

	public:
		EmittedControlParameter(Node* parent, ControlParameter* controlParameter) : Node(parent, "EmittedControlParameter", controlParameter->getName()), m_controlParameter(controlParameter) {};
		
		virtual ~EmittedControlParameter() {};
		virtual bool isValid() const { return Node::isValid() && m_controlParameter != nullptr && m_controlParameter->isValid(); };
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		ControlParameter* getControlParameter() const { return m_controlParameter; };
	};
}
