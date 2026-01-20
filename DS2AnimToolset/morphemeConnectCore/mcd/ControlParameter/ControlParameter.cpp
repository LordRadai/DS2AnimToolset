#include "ControlParameter.h"

namespace mcd
{
	ControlParameter::ControlParameter(db::Node* parent, const std::string& name, DataPin::DataType dataType)
		: mcd::AttributePinNodeBase(parent, "ControlParameter", name),
		m_dataPins(std::make_unique<db::TypedNodeContainer<mcd::DataPin>>(this, "DataPinEntry")),
		m_min(std::make_unique<db::FloatAttribute>(this, "Min", 0.0f)),
		m_max(std::make_unique<db::FloatAttribute>(this, "Max", 1.0f)),
		m_defaultFloat(std::make_unique<db::FloatAttribute>(this, "DefaultFloat", 0.f)),
		m_defaultString(std::make_unique<db::StringAttribute>(this, "DefaultString", "")),
		m_defaultVector3(std::make_unique<db::Vector3Attribute>(this, "DefaultVector3", NMP::Vector3Zero())),
		m_defaultBool(std::make_unique<db::BoolAttribute>(this, "DefaultBool", false)),
		m_defaultInt(std::make_unique<db::IntAttribute>(this, "DefaultInt", 0)),
		m_minInt(std::make_unique<db::IntAttribute>(this, "MinInt", 0)),
		m_maxInt(std::make_unique<db::IntAttribute>(this, "MaxInt", 100)),
		m_defaultQuaternion(std::make_unique<db::QuaternionAttribute>(this, "DefaultQuaternion", NMP::QuatIdentity()))
	{
		this->addAttribute(m_dataPins.get());
		m_dataPins->add(new mcd::DataPin(this, "Result", dataType));
	}

	mcd::DataPin* ControlParameter::getResultDataPin() const
	{
		if (m_dataPins->size() == 0)
			return nullptr;

		return m_dataPins->getNode(0);
	}
}
