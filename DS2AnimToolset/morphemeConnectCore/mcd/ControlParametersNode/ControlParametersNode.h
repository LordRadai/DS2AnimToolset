#pragma once
#include "NMDatabase/NMDatabase.h"
#include "NMDBExtensions/TypedNodeContainer.inl"
#include "mcd/ControlParameter/ControlParameter.h"
#include "mcd/BlendTree/BlendTree.h"

namespace mcd
{
	class ControlParametersNode : public db::Node
	{
		std::unique_ptr<db::TypedNodeContainer<mcd::ControlParameter>> m_controlParameters;
		std::unique_ptr<mcd::BlendTree> m_cpBlendTree;

	public:
		ControlParametersNode(db::Attribute* parent, std::string name);

		virtual ~ControlParametersNode() override {}

		mcd::ControlParameter* getControlParameter(const uint32_t index) const { return m_controlParameters->getNode(index); }
		mcd::ControlParameter* findControlParameter(const std::string& name) const { return m_controlParameters->find(name); }
		mcd::BlendTree* getBlendTree() { return m_cpBlendTree.get(); }

		void addControlParameter(mcd::ControlParameter* controlParameter) { m_controlParameters->add(controlParameter); }
		uint32_t getNumControlParameters() const { return m_controlParameters->size(); }
	};
}
