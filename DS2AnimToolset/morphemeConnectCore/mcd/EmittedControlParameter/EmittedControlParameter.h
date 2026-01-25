#pragma once
#include "NMDatabase.h"
#include "mcd//ControlParameter/ControlParameter.h"
#include "NMDBExtensions/Pointer.inl"

namespace mcd
{
	class EmittedControlParameter : public db::Node
	{
		std::unique_ptr<db::Pointer<ControlParameter>> m_controlParameter;
		std::unique_ptr<DataPin> m_outputPin;

	public:
		EmittedControlParameter(db::CompositeAttribute* parent, const std::string& name, ControlParameter* controlParameter);

		mcd::ControlParameter* getControlParameter() const { return m_controlParameter->getValue(); }
		DataPin* getOutputPin() const { return m_outputPin.get(); }
	};
}
