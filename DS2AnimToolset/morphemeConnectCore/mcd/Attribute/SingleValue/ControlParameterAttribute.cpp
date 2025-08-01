#include "ControlParameterAttribute.h"

namespace mcd
{
	ControlParameterAttribute::ControlParameterAttribute(db::Attribute* parent, std::string name, mcd::ControlParameter* controlParameter)
		: Attribute(parent, name, "ControlParameterAttribute"),
		  m_targetCpPin(std::make_unique<db::Pointer<mcd::Pin>>(this, "ControlParameterDataPin", controlParameter->getResultDataPin()))
	{
		addAttribute(m_targetCpPin.get());
	}

	bool ControlParameterAttribute::isValueEqualTo(Attribute* attr)
	{
		auto otherAttr = dynamic_cast<ControlParameterAttribute*>(attr);

		if (!otherAttr)
			return false;

		return m_targetCpPin->getValue() == otherAttr->m_targetCpPin->getValue();
	}

	bool ControlParameterAttribute::assignValue(Attribute* other)
	{
		auto otherAttr = dynamic_cast<ControlParameterAttribute*>(other);
		if (!otherAttr)
			return false;
		m_targetCpPin->assign(otherAttr->m_targetCpPin.get());
		return true;
	}
}