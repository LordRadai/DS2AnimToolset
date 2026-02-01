#include "BlendTree.h"
#include "NodeEditor/NodeEditor.h"
#include "NodeEditor/Link/Link.h"
#include "RCore.h"
#include "extern.h"

namespace NodeEditor
{
	BlendTree::BlendTree(NodeEditor* editor, Graph* parent, const std::string& name) : Graph(editor, parent, name), m_resultNode(nullptr)
	{
		m_resultNode = new BlendTreeOutputNode(editor, this);
		m_resultNode->setPosition(600.0f, 200.0f);
	}

	BlendTree::~BlendTree()
	{
		delete m_resultNode;
	}

	void BlendTree::draw()
	{
		Graph::draw();

		m_resultNode->draw();

		for (Link* link : m_links)
			link->draw();
	}

	bool BlendTree::loadFromProjectNode(Project::ProjectNode* projectNode)
	{
		if (!Graph::loadFromProjectNode(projectNode))
			return false;

		// First create all nodes
		for (size_t i = 0; i < projectNode->getNumChildNodes(); i++)
		{
			Project::ProjectNode* projectChildNode = projectNode->getChildNodeAtIndex(i);
			Node* childNode = nullptr;

			if (projectChildNode->isNodeBlendTree())
				childNode = createBlendTree(projectChildNode);
			else if (projectChildNode->isNodeStateMachine())
				childNode = createStateMachine(projectChildNode);
			else
				childNode = createNode(projectChildNode);

			if (childNode)
				childNode->setPosition(projectChildNode->getXPosition(), projectChildNode->getYPosition());
		}

		// Then process connections
		for (size_t i = 0; i < projectNode->getNumChildNodes(); i++)
		{
			Project::ProjectNode* projectChildNode = projectNode->getChildNodeAtIndex(i);

			Node* childNode = getNode(projectChildNode->getNodeID());

			if (!childNode)
			{
				g_appLog->debugMessage(MsgLevel_Error, "Cannot find node %d inside the Graph when attempting connections", projectChildNode->getNodeID());
				return false;
			}

			// Input nodes
			for (size_t j = 0; j < projectChildNode->getNumInputNodes(); j++)
			{
				Project::ProjectNode* projectInputNode = projectChildNode->getInputNodeAtIndex(j);
				Node* inputNode = getNode(projectInputNode->getNodeID());

				if (!inputNode)
				{
					g_appLog->debugMessage(MsgLevel_Error, "Cannot find node %d inside the Graph when attempting connections", projectChildNode->getNodeID());
					return false;
				}

				Pin* outputPin = inputNode->getOutputPin(0);
				Pin* inputPin = childNode->getInputPin(j);

				if (outputPin && inputPin)
					outputPin->connectTo(inputPin);
			}

			// Input CP connections
			for (size_t j = 0; j < projectChildNode->getNumInputCPConnections(); j++)
			{
				Project::ProjectControlParameter* projectCP = projectChildNode->getProject()->getControlParameter(projectChildNode->getInputCPConnection(j)->getNodeID());

				if (!projectCP)
				{
					g_appLog->debugMessage(MsgLevel_Error, "Cannot find Control Parameter %d inside the Project when attempting connections", projectChildNode->getInputCPConnection(j)->getNodeID());
					return false;
				}

				Pin* outputPin = m_ownerEditor->getControlParameter(projectCP->getName())->getOutputPin();
				Pin* inputPin = childNode->getInputPin(j + projectChildNode->getNumInputNodes());

				if (outputPin && inputPin)
					outputPin->connectTo(inputPin);
			}

			// Finally, if this is a BlendTree, the only child node must connect to the output
			if (isOfType<BlendTree>())
			{
				BlendTree* blendTree = asType<BlendTree>();

				if (!blendTree->connectToOutput(childNode->getOutputPin(0)))
				{
					g_appLog->debugMessage(MsgLevel_Error, "Failed to connect BlendTree node %d to its output.", blendTree->getID());
					return false;
				}
			}
		}

		return true;
	}

	Node* BlendTree::createNode(int nodeID, const std::string& typeName, const std::string& name)
	{
		return Graph::createNode(nodeID, typeName, name);
	}

	Node* BlendTree::createNode(int nodeID, const std::string& typeName, const std::string& name, float x, float y)
	{
		return Graph::createNode(nodeID, typeName, name, x, y);
	}

	Node* BlendTree::createNode(Project::ProjectNode* projectNode)
	{
		return Graph::createNode(projectNode);
	}

	bool BlendTree::connectToOutput(Pin* outputPin)
	{
		if (m_resultNode)
		{
			outputPin->connectTo(m_resultNode->getInputPin(0));
			return true;
		}

		return false;
	}
}
