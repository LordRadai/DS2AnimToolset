#include "ControlParameter.h"

namespace mcd
{
	ControlParameter::ControlParameter(db::Node* parent, const std::string& name)
		: db::Node(parent, "ControlParameter", name),
		m_dataPins(this, "DataPins"),
		m_min(this, "Min", 0.0f),
		m_max(this, "Max", 1.0f),
		m_defaultFloat(this, "DefaultFloat", 0.f),
		m_defaultString(this, "DefaultString", ""),
		m_defaultVector3(this, "DefaultVector3", NMP::Vector3Zero()),
		m_defaultBool(this, "DefaultBool", false),
		m_defaultInt(this, "DefaultInt", 0),
		m_minInt(this, "MinInt", 0),
		m_maxInt(this, "MaxInt", 100),
		m_defaultQuaternion(this, "DefaultQuaternion", NMP::QuatIdentity())
	{
		this->addAttribute(&m_dataPins);
		m_dataPins.add(new mcd::DataPin(this, "Result", "bool"));
	}

	mcd::DataPin* ControlParameter::getResultDataPin() const
	{
		if (m_dataPins.size() == 0)
			return nullptr;

		return m_dataPins[0];
	}
}
