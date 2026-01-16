#pragma once
#include "../Attribute.h"
#include "NMDBExtensions/Pointer.inl"
#include "mcd/ControlParameter/ControlParameter.h"

namespace mcd
{
	class ControlParameterAttribute : public Attribute
	{
		std::unique_ptr<db::Pointer<mcd::Pin>> m_targetCpPin;

	public:
		ControlParameterAttribute(db::CompositeAttribute* parent, std::string name, mcd::ControlParameter* controlParameter);

		virtual ~ControlParameterAttribute() override {};

		mcd::ControlParameter* getControlParameter() const { return dynamic_cast<mcd::ControlParameter*>(m_targetCpPin->getValue()); }
		void setControlParameter(mcd::ControlParameter* controlParameter) { m_targetCpPin->setValue(controlParameter->getResultDataPin()); }

		bool isValueEqualTo(Attribute* attr) override;
		bool assignValue(Attribute* other) override;
	};
}
