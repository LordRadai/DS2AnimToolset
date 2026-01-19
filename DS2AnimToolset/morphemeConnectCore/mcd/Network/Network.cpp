#include "Network.h"

namespace mcd
{
	Network::Network(db::Node* parent, const std::string& name)
		: db::Node(parent, "Network", name),
		m_graphs(std::make_unique<db::TypedNodeContainer<mcd::Graph>>(this, "GraphEntry")),
		m_controlParameters(std::make_unique<mcd::ControlParametersNode>(this, "ControlParameters")),
		m_requests(std::make_unique<mcd::RequestsNode>(this, "Requests")),
		m_resultPin(std::make_unique<mcd::PassDownPin>(this, "Result", true))
	{
		addAttribute(m_graphs.get());
		addAttribute(m_resultPin.get());
		addAttribute(m_controlParameters.get());
		addAttribute(m_requests.get());
	}

	mcd::StateMachine* Network::createStateMachineAsParent(mcc::MMStateMachine* manifestSM)
	{
		mcd::StateMachine* rootStateMachine = new mcd::StateMachine(this->m_graphs.get(), "RootStateMachine", manifestSM->getName(), manifestSM->getVersion());

		if (m_graphs->size() != 0)
		{
			mcd::Graph* oldRoot = m_graphs->getNode(0);
			std::string oldRootName = "";
			std::string oldRootType = "";
			int oldRootManifestVersion = -1;

			if (oldRoot->isOfType<mcd::StateMachine>())
			{
				mcd::StateMachine* root = dynamic_cast<mcd::StateMachine*>(oldRoot);
				root->setName("StateMachine");

				oldRootName = oldRoot->getName();
				oldRootType = oldRoot->getType();
				oldRootManifestVersion = root->getManifestVersion();
			}
			else if (oldRoot->isOfType<mcd::BlendTree>())
			{
				mcd::BlendTree* root = dynamic_cast<mcd::BlendTree*>(oldRoot);
				root->setName("StateMachine");

				oldRootName = root->getName();
				oldRootType = "BlendTree";
				oldRootManifestVersion = -1;
			}
			else
				throw std::runtime_error("Network::createStateMachineAsParent() - Root graph is not a StateMachine or BlendTree.");

			mcd::StateMachineNode* smNode = new mcd::StateMachineNode(rootStateMachine, oldRootName, 0.f, 0.f, 100.f, 70.f, oldRootType, oldRootManifestVersion);
			smNode->setGraphEntry(oldRoot);

			rootStateMachine->addStateMachineNode(smNode);

			return rootStateMachine;
		}

		m_graphs->add(rootStateMachine);

		return rootStateMachine;
	}

	mcd::BlendTree* Network::createBlendTreeAsParent()
	{
		mcd::BlendTree* rootBlendTree = new mcd::BlendTree(this->m_graphs.get(), "RootBlendTree");

		if (m_graphs->size() != 0)
		{
			mcd::Graph* oldRoot = m_graphs->getNode(0);
			std::string oldRootName = "";
			std::string oldRootType = "";
			int oldRootManifestVersion = -1;

			if (oldRoot->isOfType<mcd::StateMachine>())
			{
				mcd::StateMachine* root = dynamic_cast<mcd::StateMachine*>(oldRoot);
				root->setName("StateMachine");

				oldRootName = oldRoot->getName();
				oldRootType = oldRoot->getType();
				oldRootManifestVersion = root->getManifestVersion();
			}
			else if (oldRoot->isOfType<mcd::BlendTree>())
			{
				mcd::BlendTree* root = dynamic_cast<mcd::BlendTree*>(oldRoot);
				root->setName("StateMachine");

				oldRootName = root->getName();
				oldRootType = "BlendTree";
				oldRootManifestVersion = -1;
			}
			else
				throw std::runtime_error("Network::createBlendTreeAsParent() - Root graph is not a StateMachine or BlendTree.");

			mcd::BlendTreeNode* btNode = new mcd::BlendTreeNode(rootBlendTree, oldRootName, 0.f, 0.f, 100.f, 70.f, oldRootType, oldRootManifestVersion);
			btNode->setGraphEntry(oldRoot);

			rootBlendTree->addBlendTreeNode(btNode);

			return rootBlendTree;
		}

		m_graphs->add(rootBlendTree);

		return rootBlendTree;
	}

	bool Network::reparentGraph(mcd::Graph* newParent)
	{
		bool bReparented = newParent->reparent(this->m_graphs->getNode(0));

		if (!bReparented)
			return false;

		if (m_graphs->size() > 0)
			m_graphs->clearArray();

		return true;
	}
}
