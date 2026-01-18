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
		std::unique_ptr<mcd::PassDownPin> m_resultPin;

	public:
		Network(db::Node* parent, const std::string& name);

		virtual ~Network() override {};

		mcd::Graph* getGraph() const { return m_graphs->getNode(0); }
		mcd::ControlParametersNode* getControlParametersNode() const { return m_controlParameters.get(); }
		mcd::RequestsNode* getRequestsNode() const { return m_requests.get(); }

		mcd::StateMachine* createStateMachineAsParent(mcc::MMStateMachine* manifestSM);
		mcd::BlendTree* createBlendTreeAsParent();
		bool reparentGraph(mcd::Graph* newParent);

		void addControlParameter(mcd::ControlParameter* controlParameter) { m_controlParameters->addControlParameter(controlParameter); }
		mcd::ControlParameter* getControlParameter(const uint32_t index) const { return m_controlParameters->getControlParameter(index); }
		mcd::ControlParameter* findControlParameter(const std::string& name) const { return m_controlParameters->findControlParameter(name); }
		uint32_t getNumControlParameters() const { return m_controlParameters->size(); }

		void addRequest(mcd::Request* request) { m_requests->addRequest(request); }
		mcd::Request* getRequest(const uint32_t index) const { return m_requests->getRequest(index); }
		mcd::Request* findRequest(const uint32_t id) const { return m_requests->findRequest(id); }
		mcd::Request* findRequest(const std::string& name) const { return m_requests->findRequest(name); }
		uint32_t getNumRequests() const { return m_requests->size(); }

		mcd::PassDownPin* getResultPin() const { return m_resultPin.get(); }
	};
}
