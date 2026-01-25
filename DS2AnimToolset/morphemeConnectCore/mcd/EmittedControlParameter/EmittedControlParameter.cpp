#include "EmittedControlParameter.h"

namespace mcd
{
	EmittedControlParameter::EmittedControlParameter(db::CompositeAttribute* parent, const std::string& name, ControlParameter* controlParameter)
		: db::Node(parent, "EmittedControlParameter", name)
		, m_controlParameter(std::make_unique<db::Pointer<ControlParameter>>(this, "ControlParameter", controlParameter))
		, m_outputPin(std::make_unique<DataPin>(this, "OutputPin", controlParameter->getDataType()))
	{
		addAttribute(m_controlParameter.get());
		addAttribute(m_outputPin.get());

		m_outputPin->setIsInput(true);
	}
}
