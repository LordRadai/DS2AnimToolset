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
		EmittedControlParameter() {};
		EmittedControlParameter(std::string name, ControlParameter* controlParameter) : Node(name), m_controlParameter(controlParameter) {};
		~EmittedControlParameter() {};

		ControlParameter* getControlParameter() const { return m_controlParameter; };
	};
}
