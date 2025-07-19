#pragma once
#include "RCore.h"
#include "../ControlParameter/ControlParameter.h"

namespace db
{
	class EmittedControlParameter
	{
		ControlParameter* m_controlParameter;

	public:
		EmittedControlParameter(ControlParameter* controlParameter)
			: m_controlParameter(controlParameter) {
		};
		~EmittedControlParameter() {};

		ControlParameter* getControlParameter() const { return m_controlParameter; };
	};
}
