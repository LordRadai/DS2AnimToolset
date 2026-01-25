#pragma once
#include "mcd/EmittedControlParameter/EmittedControlParameter.h"
#include "NMDBExtensions/TypedNodeContainer.inl"

namespace mcd
{
	class EmittedControlParametersNode : public db::Node
	{
		std::unique_ptr<db::TypedNodeContainer<EmittedControlParameter>> m_emittedControlParameters;
	public:
		EmittedControlParametersNode(db::CompositeAttribute* parent, const std::string& name);

		int getNumEmittedControlParameters() const { return static_cast<int>(m_emittedControlParameters->size()); }
		EmittedControlParameter* getEmittedControlParameter(int index) const { return m_emittedControlParameters->getNode(index); }
		EmittedControlParameter* findEmittedControlParameter(const std::string& name) const { return m_emittedControlParameters->find(name); }
		void addEmittedControlParameter(EmittedControlParameter* emittedControlParameter) { m_emittedControlParameters->add(emittedControlParameter); }
	};
}
