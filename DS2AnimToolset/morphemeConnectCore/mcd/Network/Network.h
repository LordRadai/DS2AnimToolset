#pragma once
#include "NMDatabase/NMDatabase.h"
#include "mcd/StateMachine/StateMachine.h"
#include "mcd/BlendTree/BlendTree.h"
#include "mcd/ControlParametersNode/ControlParametersNode.h"
#include "mcd/RequestsNode/RequestsNode.h"
#include "mcc/MorphemeManifest/MMManifestItem/StateMachine/MMStateMachine.h"

namespace mcd
{
	class Network : public db::Node
	{
		std::unique_ptr<db::TypedNodeContainer<mcd::Graph>> m_graphs;
		std::unique_ptr<mcd::ControlParametersNode> m_controlParameters;
		std::unique_ptr<mcd::RequestsNode> m_requests;

	public:
		Network(db::Node* parent, const std::string& name);

		virtual ~Network() override {};

		db::TypedNodeContainer<mcd::Graph>* getGraphs() const { return m_graphs.get(); }
		mcd::ControlParametersNode* getControlParametersNode() const { return m_controlParameters.get(); }
		mcd::RequestsNode* getRequestsNode() const { return m_requests.get(); }

		mcd::StateMachine* createStateMachineAsParent(mcc::MMStateMachine* manifestSM);
		mcd::BlendTree* createBlendTreeAsParent();
		bool reparentGraph(mcd::Graph* newParent);

		void addControlParameter(mcd::ControlParameter* controlParameter) { m_controlParameters->addControlParameter(controlParameter); }
		mcd::ControlParameter* getControlParameter(const uint32_t index) const { return m_controlParameters->getControlParameter(index); }
		mcd::ControlParameter* findControlParameter(const std::string& name) const { return m_controlParameters->findControlParameter(name); }

		void addRequest(mcd::Request* request) { m_requests->addControlParameter(request); }
		mcd::Request* getRequest(const uint32_t index) const { return m_requests->getRequest(index); }
		mcd::Request* findRequest(const uint32_t id) const { return m_requests->findRequest(id); }
		mcd::Request* findRequest(const std::string& name) const { return m_requests->findRequest(name); }
	};
}
