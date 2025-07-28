#pragma once
#include "NMDatabase/NMDatabase.h"
#include "mcd/Graph/Graph.h"
#include "mcd/ControlParametersNode/ControlParametersNode.h"

namespace mcd
{
	class Network : public db::Node
	{
		std::unique_ptr<db::TypedNodeContainer<mcd::Graph>> m_graphs;
		std::unique_ptr<mcd::ControlParametersNode> m_controlParametersNode;

	public:
		Network(db::Node* parent, const std::string& name);

		virtual ~Network() override {};

		mcd::ControlParametersNode* getControlParametersNode() const { return m_controlParametersNode.get(); }

		void createStateMachineAsParent() {};
		void createBlendTreeAsParent() {};

		void reparentGraph(mcd::Graph* newParent) {};

		void addControlParameter(mcd::ControlParameter* controlParameter) { m_controlParametersNode->addControlParameter(controlParameter); }
		mcd::ControlParameter* getControlParameter(const uint32_t index) const { return m_controlParametersNode->getControlParameter(index); }
		mcd::ControlParameter* findControlParameter(const std::string& name) const { return m_controlParametersNode->findControlParameter(name); }
	};
}
