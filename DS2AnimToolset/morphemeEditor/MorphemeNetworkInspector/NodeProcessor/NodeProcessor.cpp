#include <queue>

#include "NodeProcessor.h"

#include "RLog/RLog.h"
#include "extern.h"

#include "morpheme/mrNetworkDef.h"
#include "morpheme/Nodes/mrNodeStateMachine.h"

#include "NodeNamingStrategy/Utils/Utils.h"
#include "NodeNamingStrategy/DefaultNodeNamingStrategy.h"
#include "NodeNamingStrategy/ReconstructParentChildNameStrategy.h"

#include "GraphLayouterStrategy/BTFanLayouterStrategy.h"

namespace
{
	void addNodeToList(std::vector<MR::NodeDef*>& outList, MR::NodeDef* nodeDef)
	{
		for (size_t i = 0; i < outList.size(); i++)
		{
			if (outList[i]->getNodeID() == nodeDef->getNodeID())
				return;
		}

		outList.push_back(nodeDef);
	}

	bool doesListContainNode(std::vector<MR::NodeDef*>& list, MR::NodeDef* nodeDef)
	{
		for (size_t i = 0; i < list.size(); i++)
		{
			if (list[i]->getNodeID() == nodeDef->getNodeID())
				return true;
		}

		return false;
	}

	void getNodesWithThisAsInput(std::vector<MR::NodeDef*>& outList, MR::NetworkDef* netDef, MR::NodeID nodeID)
	{
		outList.clear();

		for (uint32_t i = 0; i < netDef->getNumNodeDefs(); i++)
		{
			MR::NodeDef* currentNodeDef = netDef->getNodeDef(i);
			
			if (currentNodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE))
				continue;

			for (size_t childIdx = 0; childIdx < currentNodeDef->getNumChildNodes(); childIdx++)
			{
				if (currentNodeDef->getChildNodeID(childIdx) == nodeID)
				{
					outList.push_back(currentNodeDef);
					break;
				}
			}
		}
	}

	void getNodesWithThisAsInputCP(std::vector<MR::NodeDef*>& outList, MR::NetworkDef* netDef, MR::NodeID nodeID)
	{
		outList.clear();

		for (uint32_t i = 0; i < netDef->getNumNodeDefs(); i++)
		{
			MR::NodeDef* currentNodeDef = netDef->getNodeDef(i);

			for (size_t inputCPIdx = 0; inputCPIdx < currentNodeDef->getNumInputCPConnections(); inputCPIdx++)
			{
				const MR::CPConnection* cpConnection = currentNodeDef->getInputCPConnection(inputCPIdx);

				if (cpConnection->m_sourceNodeID == nodeID)
				{
					outList.push_back(currentNodeDef);
					break;
				}
			}
		}
	}
}

bool NodeProcessor::preProcessNetwork(MR::NetworkDef* netDef)
{
	m_blendTreeNodeNames.clear();
	m_blendTreeNodes.clear();
	m_blendTreeNodeMap.clear();
	m_stateMachineNodeMap.clear();
	m_containerNodes.clear();
	m_nodeNameMap.clear();
	m_multiplyConnectedCPOutputNodes.clear();

	if (isNetworkNodeNameMapComplete(netDef))
		m_namingStrategy = new DefaultNodeNamingStrategy();
	else
		m_namingStrategy = new ReconstructParentChildNameStrategy();

	m_blendTreeLayouterStrategy = new BTFanLayouterStrategy();

	collectContainerNodes(netDef);
	collectBlendTreeChildNodes(netDef);
	collectNodeNames(netDef);

	g_appLog->debugMessage(MsgLevel_Debug, "Root Node: %d (name=\"%s\")\n", netDef->getRootNodeID(), netDef->getNodeNameFromNodeID(netDef->getRootNodeID()));

	std::map<MR::NodeID, std::vector<MR::NodeDef*>> containerNodeMap;

	for (const auto& blendTreeNodePair : m_blendTreeNodeMap)
	{
		containerNodeMap[blendTreeNodePair.first] = blendTreeNodePair.second;

		g_appLog->debugMessage(
			MsgLevel_Debug,
			"Blend Tree node %d (name=\"%s\") has %d child nodes:\n",
			blendTreeNodePair.first,
			getNodeName(blendTreeNodePair.first).c_str(),
			blendTreeNodePair.second.size());

		for (const auto& childNode : blendTreeNodePair.second)
		{
			std::string nodeName = getBlendTreeNodeName(childNode->getNodeID());

			if (!isNodeBlendTree(childNode))
				nodeName = getNodeName(childNode->getNodeID());

			g_appLog->debugMessage(
				MsgLevel_Debug,
				"\tID=%d (name=\"%s\", type=\"%s\").\n",
				childNode->getNodeID(),
				nodeName.c_str(),
				nodeTypeAsManifestName(childNode->getNodeTypeID()).c_str()
			);
		}
	}

	for (const auto& smNodePair : m_stateMachineNodeMap)
	{
		containerNodeMap[smNodePair.first] = smNodePair.second;

		g_appLog->debugMessage(
			MsgLevel_Debug,
			"State Machine node %d (name=\"%s\") has %d child nodes:\n",
			smNodePair.first,
			getNodeName(smNodePair.first).c_str(),
			smNodePair.second.size());

		for (const auto& childNode : smNodePair.second)
		{
			std::string nodeName = getBlendTreeNodeName(childNode->getNodeID());

			if (!isNodeBlendTree(childNode))
				nodeName = getNodeName(childNode->getNodeID());

			g_appLog->debugMessage(
				MsgLevel_Debug,
				"\tID=%d (name=\"%s\", type=\"%s\").\n",
				childNode->getNodeID(),
				nodeName.c_str(),
				nodeTypeAsManifestName(childNode->getNodeTypeID()).c_str()
			);
		}
	}

	int numNetworkNodes = 0;

	std::vector<MR::NodeDef*> notFoundNodes;
	for (size_t i = 1; i < netDef->getNumNodeDefs(); i++)
	{
		MR::NodeDef* nodeDef = netDef->getNodeDef(i);
		MR::NodeDef::NodeFlags flags = nodeDef->getNodeFlags();

		// Skip control params or transitions
		if (flags.isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM) ||
			flags.isSet(MR::NodeDef::NODE_FLAG_IS_TRANSITION))
			continue;

		numNetworkNodes++;

		bool foundInBlendTree = false;
		bool foundInStateMachine = false;

		for (const auto& [containerID, children] : m_blendTreeNodeMap)
		{
			if (std::find(children.begin(), children.end(), nodeDef) != children.end())
			{
				foundInBlendTree = true;
				break;
			}
		}

		for (const auto& [containerID, children] : m_stateMachineNodeMap)
		{
			if (std::find(children.begin(), children.end(), nodeDef) != children.end())
			{
				foundInStateMachine = true;
				break;
			}
		}

		if (!foundInBlendTree && !foundInStateMachine)
			notFoundNodes.push_back(nodeDef);
	}

	for (const auto& nodeDef : notFoundNodes)
		g_appLog->debugMessage(MsgLevel_Warn, "NodeProcessor::preProcessNetwork: Node %d (name=\"%s\", type=\"%s\") is not referenced as a child of any blend tree or state machine node.\n", nodeDef->getNodeID(), getNodeName(nodeDef->getNodeID()).c_str(), nodeTypeAsManifestName(nodeDef->getNodeTypeID()).c_str());

	if (!notFoundNodes.empty())
	{
		INVOKE_PANIC(
			"%d nodes in the network are not referenced as a child of any blend tree or state machine node. This indicates an issue with the node processing logic. Please check the log for details.\n",
			notFoundNodes.size());
	}

	return true;
}

NodeEditor::ControlParameter* NodeProcessor::processControlParameter(NodeEditor::Editor* editor, MR::NodeDef* nodeDef, const std::string& name)
{
	const MR::NodeDef::NodeFlags nodeFlags = nodeDef->getNodeFlags();

	if (!nodeFlags.isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM))
	{
		g_appLog->debugMessage(MsgLevel_Error, "NodeProcessor::processControlParameter: Node '%s' is not a control parameter node.", name);
		return nullptr;
	}

	const std::string nodeName = name;

	switch (nodeDef->getNodeTypeID())
	{
	case NODE_TYPE_CP_FLOAT:	return editor->createControlParameterFloat(nodeDef->getNodeID(), nodeName);
	case NODE_TYPE_CP_INT:		return editor->createControlParameterInt(nodeDef->getNodeID(), nodeName);
	case NODE_TYPE_CP_UINT:		return editor->createControlParameterUInt(nodeDef->getNodeID(), nodeName);
	case NODE_TYPE_CP_BOOL:		return editor->createControlParameterBool(nodeDef->getNodeID(), nodeName);
	case NODE_TYPE_CP_VECTOR3:	return editor->createControlParameterVector3(nodeDef->getNodeID(), nodeName);
	case NODE_TYPE_CP_VECTOR4:	return editor->createControlParameterQuaternion(nodeDef->getNodeID(), nodeName);
	default:
		g_appLog->debugMessage(MsgLevel_Error, "NodeProcessor::processControlParameter: Unsupported control parameter type ID %d for node '%s'.", nodeDef->getNodeTypeID(), nodeName);
		return nullptr;
	}
}

NodeEditor::Node* NodeProcessor::processNode(NodeEditor::Graph* graph, MR::NodeDef* nodeDef, const std::string& name)
{
	g_appLog->debugMessage(MsgLevel_Info, "NodeProcessor::processNode: Processing node '%s' of ID %d.\n", name.c_str(), nodeDef->getNodeID());

	if (isNodeBlendTree(nodeDef) && (nodeDef->getNodeID() != graph->getGraphID()))
	{
		g_appLog->debugMessage(MsgLevel_Info, "NodeProcessor::processNode: Creating blend tree node for node %d.\n", nodeDef->getNodeID());

		return graph->createBlendTree(nodeDef->getNodeID(), m_blendTreeNodeNames[graph->getGraphID()]);
	}
	else if (nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE))
	{
		g_appLog->debugMessage(MsgLevel_Info, "NodeProcessor::processNode: Creating state machine node for node %d.\n", nodeDef->getNodeID());

		return graph->createStateMachine(nodeDef->getNodeID(), name);
	}

	if (!graph->isOfType<NodeEditor::BlendTree>())
		INVOKE_PANIC(
			"Non-container node '%s' inside state machine container.\n",
			name.c_str());

	NodeEditor::Node* node = graph->asType<NodeEditor::BlendTree>()->createNode(nodeDef->getNodeID(), nodeTypeAsManifestName(nodeDef->getNodeTypeID()), name);

	// Set node attributes
	return node;
}

NodeEditor::Graph* NodeProcessor::buildRootGraph(NodeEditor::Editor* editor, MR::NodeDef* rootNodeDef)
{
	NodeEditor::Graph* rootGraph = editor->createRootBlendTree(rootNodeDef->getNodeID());

	populateGraph(rootGraph, rootNodeDef);
	populateSubGraphs(rootGraph, rootNodeDef);
	processMultiplyConnectedNodes(editor, rootNodeDef->getOwningNetworkDef());

	return rootGraph;
}

void NodeProcessor::populateGraph(NodeEditor::Graph* graph, MR::NodeDef* ownerNodeDef)
{
	MR::NetworkDef* netDef = ownerNodeDef->getOwningNetworkDef();

	if (graph->isOfType<NodeEditor::BlendTree>())
	{
		auto& childNodes = m_blendTreeNodeMap[ownerNodeDef->getNodeID()];

		// Process all child nodes
		for (MR::NodeDef* child : childNodes)
		{
			const std::string childName = getNodeName(child->getNodeID());

			NodeEditor::Node* node = processNode(graph, child, childName);

			if (node->hasSubGraph() && node->getSubGraph()->isOfType<NodeEditor::BlendTree>())
				node->setName(getBlendTreeNodeName(child->getNodeID()));
		}

		processNodeConnectionsInBlendTree(graph->asType<NodeEditor::BlendTree>(), ownerNodeDef, childNodes);
		setBlendTreeLayout(graph->asType<NodeEditor::BlendTree>(), ownerNodeDef, childNodes);
	}
	else if (graph->isOfType<NodeEditor::StateMachine>())
	{
		// Process all child nodes
		for (uint32_t i = 0; i < ownerNodeDef->getNumChildNodes(); ++i)
		{
			MR::NodeDef* child = ownerNodeDef->getChildNodeDef(i);

			if (child->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_TRANSITION))
				continue;

			const std::string childName = getNodeName(child->getNodeID());
			NodeEditor::Node* node = processNode(graph, child, childName);

			if (node->hasSubGraph() && node->getSubGraph()->isOfType<NodeEditor::BlendTree>())
				node->setName(getBlendTreeNodeName(child->getNodeID()));
		}

		// Set default state
		MR::AttribDataStateMachineDef* stateMachineDef = static_cast<MR::AttribDataStateMachineDef*>(ownerNodeDef->getAttribData(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

		graph->asType<NodeEditor::StateMachine>()->setDefaultNodeID(ownerNodeDef->getChildNodeID(stateMachineDef->m_defaultStartingStateID));

		processNodeTransitionsInStateMachine(
			graph->asType<NodeEditor::StateMachine>(),
			ownerNodeDef);

		setStateMachineLayout(graph->asType<NodeEditor::StateMachine>(), ownerNodeDef);
	}
	else
	{
		INVOKE_PANIC("NodeProcessor::populateGraph: Unsupported graph type for graph ID %d.\n", graph->getGraphID());
	}
}

void NodeProcessor::populateSubGraphs(NodeEditor::Graph* graph, MR::NodeDef* ownerNodeDef)
{
	auto& nodes = graph->getNodes();

	MR::NetworkDef* netDef = ownerNodeDef->getOwningNetworkDef();

	for (size_t i = 0; i < nodes.size(); ++i)
	{
		NodeEditor::Node* node = nodes[i];
		NodeEditor::Graph* subGraph = node->getSubGraph();
		if (!subGraph)
			continue;

		MR::NodeDef* nodeDef = netDef->getNodeDef(node->getNodeID());

		populateGraph(subGraph, nodeDef);
		populateSubGraphs(subGraph, nodeDef);
	}
}

void NodeProcessor::createPassDownConnection(
	NodeEditor::BlendTree* blendTree,
	NodeEditor::PassDownPinsNode* passDownPinNode,
	NodeEditor::Node* multiplyConnectedNode,
	NodeEditor::Node* sourceNode,
	MR::NodeID nodeID,
	int inputIdx,
	bool isCPConnection)
{
	if (inputIdx == -1)
		return;

	char passDownPinName[256];
	sprintf_s(passDownPinName, "PassDown_%s", getNodeName(nodeID).c_str());

	// Create the pass down pin if it does not exist
	if (blendTree->getPassDownPin(passDownPinName) == "")
	{
		blendTree->addPassDownPin(passDownPinName);
		passDownPinNode->updatePins();

		NodeEditor::Graph* currentGraph = blendTree;
		NodeEditor::Node* nodeToConnectTo = nullptr;

		// Walk up the graph hierarchy until we find the graph containing the node
		while (currentGraph && !currentGraph->getNode(nodeID))
		{
			NodeEditor::Node* graphNode = currentGraph->getGraphNode();
			if (!graphNode)
			{
				INVOKE_PANIC(
					"Graph with ID %d (%s) does not have a graph node.\n",
					currentGraph->getGraphID(),
					currentGraph->getFullName().c_str());
			}

			if (currentGraph->isRootGraph())
				break;

			if (currentGraph->isOfType<NodeEditor::StateMachine>())
				currentGraph->addPassDownPin(passDownPinName);

			NodeEditor::Graph* parentGraph = currentGraph->getParentGraph();

			if (parentGraph->isOfType<NodeEditor::BlendTree>())
			{
				auto* parentBlendTree = parentGraph->asType<NodeEditor::BlendTree>();
				NodeEditor::PassDownPinsNode* parentPassDownPinsNode =
					parentBlendTree->getPassDownPinsNode();

				NodeEditor::Pin* parentPassDownPin =
					parentPassDownPinsNode->getOutputPin(passDownPinName);

				if (parentPassDownPin)
					parentPassDownPin->connectTo(graphNode->getInputPin(passDownPinName));
			}

			currentGraph = parentGraph;
			nodeToConnectTo = graphNode;
		}

		g_appLog->debugMessage(
			MsgLevel_Debug,
			"Node to connect to for pass down pin '%s' is %d (%s).\n",
			passDownPinName,
			nodeToConnectTo->getNodeID(),
			nodeToConnectTo->getFullName().c_str());

		if (multiplyConnectedNode->getParentGraph() != nodeToConnectTo->getParentGraph())
		{
			INVOKE_PANIC(
				"Graph containing multiply connected node ID %d (%s) is not the same as "
				"the graph containing the node %d (%s) to connect to.\n",
				multiplyConnectedNode->getNodeID(),
				multiplyConnectedNode->getFullName().c_str(),
				nodeToConnectTo->getNodeID(),
				nodeToConnectTo->getFullName().c_str());
		}

		NodeEditor::Pin* srcPin = multiplyConnectedNode->getOutputPin(0);
		NodeEditor::Pin* dstPin = nodeToConnectTo->getInputPin(passDownPinName);

		NodeEditor::BlendTree* connectionGraph =
			multiplyConnectedNode->getParentGraph()->asType<NodeEditor::BlendTree>();

		if (!connectionGraph->hasConnectionBetween(srcPin, dstPin))
			srcPin->connectTo(dstPin);
	}

	// Final connection to the source node
	NodeEditor::Pin* passDownPin = passDownPinNode->getOutputPin(passDownPinName);
	if (!passDownPin)
	{
		INVOKE_PANIC(
			"Failed to find pass down pin '%s' in blend tree with ID %d (%s) "
			"for multiply connected node ID %d (%s).\n",
			passDownPinName,
			blendTree->getGraphID(),
			blendTree->getFullName().c_str(),
			multiplyConnectedNode->getNodeID(),
			multiplyConnectedNode->getFullName().c_str());
	}

	passDownPin->connectTo(sourceNode->getInputPin(inputIdx));
}

void NodeProcessor::processMultiplyConnectedNodes(NodeEditor::Editor* editor, MR::NetworkDef* netDef)
{
	// Process all multiply connected nodes. For each multiply connected node, we need to find all nodes that reference it as a child. 
	// For each of those nodes, we need to connect the source node to the pass down pin node in its owner blend tree and connect the 
	// multiply connected node pin to the pass down pin of the graph containing the source node. We also need to create pass down pins along the way if they don't already exist.
	for (size_t i = 0; i < netDef->getNumMultiplyConnectedNodes(); i++)
	{
		const MR::NodeID nodeID = netDef->getMultiplyConnectedNodeID(i);
		MR::NodeDef* nodeDef = netDef->getNodeDef(nodeID);

		if (nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM))
			continue;

		NodeEditor::Node* multiplyConnectedNode = editor->getNode(nodeID);

		if (!multiplyConnectedNode)
			INVOKE_PANIC("Multiply connected node %d (%s) is not present in the editor.\n", nodeID, getNodeName(nodeID).c_str());

		std::vector<MR::NodeDef*> nodesWithThisAsInput;
		getNodesWithThisAsInput(nodesWithThisAsInput, netDef, nodeID);

		if (nodesWithThisAsInput.size() > 0)
		{
			g_appLog->debugMessage(MsgLevel_Info, "Node ID %d is multiply connected. It has %d connections.\n", nodeID, nodesWithThisAsInput.size());

			for (MR::NodeDef* nodeDef : nodesWithThisAsInput)
			{
				const MR::NodeID sourceNodeID = nodeDef->getNodeID();
				NodeEditor::Node* sourceNode = editor->getNode(sourceNodeID);

				if (!sourceNode)
				{
					g_appLog->debugMessage(MsgLevel_Warn, "Failed to find source node with ID %d for multiply connected node ID %d.\n", sourceNodeID, nodeID);
					continue;
				}

				if (sourceNode->getTypeName() == "BlendTree")
					sourceNode = sourceNode->getSubGraph()->asType<NodeEditor::BlendTree>()->getNodeConnectedToOutput();

				if (!sourceNode->getParentGraph()->isOfType<NodeEditor::BlendTree>())
					INVOKE_PANIC("Source node with ID %d for multiply connected node ID %d is not in a blend tree. This contradicts core assumptions.\n", sourceNodeID, nodeID);

				NodeEditor::BlendTree* blendTree = sourceNode->getParentGraph()->asType<NodeEditor::BlendTree>();

				NodeEditor::PassDownPinsNode* passDownPinNode = blendTree->getPassDownPinsNode();

				g_appLog->debugMessage(MsgLevel_Debug, "Connecting node ID %d (%s) in blend tree %d (%s) to pass down pins node for multiply connected node ID %d (%s).\n", sourceNode->getNodeID(), sourceNode->getFullName().c_str(), blendTree->getGraphID(), blendTree->getFullName().c_str(), multiplyConnectedNode->getNodeID(), multiplyConnectedNode->getFullName().c_str());
			
				if (!passDownPinNode)
					INVOKE_PANIC("Blend tree with ID %d (%s) does not have a pass down pins node.\n", blendTree->getGraphID(), blendTree->getFullName().c_str());

				// We must do two things:
				// 1) Connect the source node to the pass down pin node in its owner blend tree
				// 2) Connect the multiply connected node pin to the pass down pin of the graph containing the source node

				int inputIdx = -1;
				for (size_t i = 0; i < nodeDef->getNumChildNodes(); i++)
				{
					if (nodeDef->getChildNodeID(i) == nodeID)
					{
						inputIdx = i;
						break;
					}
				}

				if (inputIdx != -1)
				{
					char passDownPinName[256];
					sprintf_s(passDownPinName, "%s", getNodeName(nodeID).c_str());

					if (blendTree->getPassDownPin(passDownPinName) == "")
					{
						g_appLog->debugMessage(MsgLevel_Info, "Creating pass down connection %s for multiply connected node ID %d (%s) in blend tree %d (%s).\n", passDownPinName, multiplyConnectedNode->getNodeID(), multiplyConnectedNode->getFullName().c_str(), blendTree->getGraphID(), blendTree->getFullName().c_str());

						blendTree->addPassDownPin(passDownPinName);
						passDownPinNode->updatePins();

						// Recurse up until we find a graph containing the multiply connected node. Create pass down pins along the way. Connect the pass down pins to the graph nodes as we go up.
						NodeEditor::Graph* currentGraph = blendTree;
						NodeEditor::Node* nodeToConnectTo = nullptr;
						while (currentGraph && !currentGraph->getNode(nodeID))
						{
							NodeEditor::Node* graphNode = currentGraph->getGraphNode();

							if (!graphNode)
								INVOKE_PANIC("Graph with ID %d (%s) does not have a graph node.\n", currentGraph->getGraphID(), currentGraph->getFullName().c_str());

							if (currentGraph->isRootGraph())
								break;

							if (currentGraph->isOfType<NodeEditor::StateMachine>())
								currentGraph->addPassDownPin(passDownPinName);

							NodeEditor::Graph* parentGraph = currentGraph->getParentGraph();

							if (parentGraph->isOfType<NodeEditor::BlendTree>() && multiplyConnectedNode->getParentGraph() != parentGraph)
							{
								NodeEditor::BlendTree* parentBlendTree = parentGraph->asType<NodeEditor::BlendTree>();
								NodeEditor::PassDownPinsNode* parentPassDownPinNode = parentBlendTree->getPassDownPinsNode();

								if (parentBlendTree->getPassDownPin(passDownPinName) == "")
								{
									parentBlendTree->addPassDownPin(passDownPinName);
									parentPassDownPinNode->updatePins();
								}

								NodeEditor::Pin* parentPassDownPin = parentPassDownPinNode->getOutputPin(passDownPinName);
								parentPassDownPin->connectTo(graphNode->getInputPin(0));
							}

							currentGraph = parentGraph;
							nodeToConnectTo = graphNode;
						}

						g_appLog->debugMessage(MsgLevel_Debug, "Node to connect to for pass down pin '%s' is %d (%s).\n", passDownPinName, nodeToConnectTo->getNodeID(), nodeToConnectTo->getFullName().c_str());

						if (multiplyConnectedNode->getParentGraph() != nodeToConnectTo->getParentGraph())
							INVOKE_PANIC("Graph containing multiply connected node ID %d (%s) is not the same as the graph containing the node %d (%s) to connect to.\n", multiplyConnectedNode->getNodeID(), multiplyConnectedNode->getFullName().c_str(), nodeToConnectTo->getNodeID(), nodeToConnectTo->getFullName().c_str());

						NodeEditor::Pin* multiplyConnectedNodePin = multiplyConnectedNode->getOutputPin(0);
						NodeEditor::Pin* nodeToConnectToPin = nodeToConnectTo->getInputPin(passDownPinName);

						NodeEditor::BlendTree* graphForConnection = multiplyConnectedNode->getParentGraph()->asType<NodeEditor::BlendTree>();

						if (!graphForConnection->hasConnectionBetween(multiplyConnectedNodePin, nodeToConnectToPin))
							multiplyConnectedNodePin->connectTo(nodeToConnectToPin);
					}

					NodeEditor::Pin* passDownPin = passDownPinNode->getOutputPin(passDownPinName);

					if (!passDownPin)
						INVOKE_PANIC("Failed to find pass down pin '%s' in blend tree with ID %d (%s) for multiply connected node ID %d (%s).\n", passDownPinName, blendTree->getGraphID(), blendTree->getFullName().c_str(), multiplyConnectedNode->getNodeID(), multiplyConnectedNode->getFullName().c_str());

					passDownPin->connectTo(sourceNode->getInputPin(inputIdx));
				}
			}
		}
	}

	for (size_t i = 0; i < m_multiplyConnectedCPOutputNodes.size(); i++)
	{
		MR::NodeDef* nodeDef = m_multiplyConnectedCPOutputNodes[i];
		const MR::NodeID nodeID = nodeDef->getNodeID();

		NodeEditor::Node* multiplyConnectedNode = editor->getNode(nodeID);

		if (!multiplyConnectedNode)
			INVOKE_PANIC("Multiply connected node %d (%s) is not present in the editor.\n", nodeID, getNodeName(nodeID).c_str());

		std::vector<MR::NodeDef*> nodesWithThisAsInputCP;
		getNodesWithThisAsInputCP(nodesWithThisAsInputCP, netDef, m_multiplyConnectedCPOutputNodes[i]->getNodeID());

		if (nodesWithThisAsInputCP.size() == 1)
			continue;

		for (MR::NodeDef* nodeDef : nodesWithThisAsInputCP)
		{
			const MR::NodeID sourceNodeID = nodeDef->getNodeID();
			NodeEditor::Node* sourceNode = editor->getNode(sourceNodeID);

			if (!sourceNode)
			{
				g_appLog->debugMessage(MsgLevel_Warn, "Failed to find source node with ID %d for multiply connected node ID %d.\n", sourceNodeID, nodeID);
				continue;
			}

			if (sourceNode->getTypeName() == "BlendTree")
				sourceNode = sourceNode->getSubGraph()->asType<NodeEditor::BlendTree>()->getNodeConnectedToOutput();

			if (!sourceNode->getParentGraph()->isOfType<NodeEditor::BlendTree>())
				INVOKE_PANIC("Source node with ID %d for multiply connected node ID %d is not in a blend tree. This contradicts core assumptions.\n", sourceNodeID, nodeID);

			NodeEditor::BlendTree* blendTree = sourceNode->getParentGraph()->asType<NodeEditor::BlendTree>();

			NodeEditor::PassDownPinsNode* passDownPinNode = blendTree->getPassDownPinsNode();

			g_appLog->debugMessage(MsgLevel_Debug, "Connecting node ID %d (%s) in blend tree %d (%s) to pass down pins node for multiply connected node ID %d (%s).\n", sourceNode->getNodeID(), sourceNode->getFullName().c_str(), blendTree->getGraphID(), blendTree->getFullName().c_str(), multiplyConnectedNode->getNodeID(), multiplyConnectedNode->getFullName().c_str());

			if (!passDownPinNode)
				INVOKE_PANIC("Blend tree with ID %d (%s) does not have a pass down pins node.\n", blendTree->getGraphID(), blendTree->getFullName().c_str());

			// We must do two things:
			// 1) Connect the source node to the pass down pin node in its owner blend tree
			// 2) Connect the multiply connected node pin to the pass down pin of the graph containing the source node

			int inputIdx = -1;
			int inputPinIndex = -1;
			for (size_t i = 0; i < nodeDef->getNumInputCPConnections(); i++)
			{
				const MR::CPConnection* cpConnection = nodeDef->getInputCPConnection(i);

				if (cpConnection->m_sourceNodeID == nodeID)
				{
					inputIdx = i;
					inputPinIndex = cpConnection->m_sourcePinIndex;
					break;
				}
			}

			if (inputIdx != -1 && inputPinIndex != -1)
			{
				NodeEditor::Pin* multiplyConnectedTargetPin = multiplyConnectedNode->getOutputDataPin(inputPinIndex);

				char passDownPinName[256];
				sprintf_s(passDownPinName, "%s.%s", getNodeName(nodeID).c_str(), multiplyConnectedTargetPin->getName().c_str());

				if (blendTree->getPassDownPin(passDownPinName) == "")
				{
					g_appLog->debugMessage(MsgLevel_Info, "Creating pass down connection %s for multiply connected node ID %d (%s) in blend tree %d (%s).\n", passDownPinName, multiplyConnectedNode->getNodeID(), multiplyConnectedNode->getFullName().c_str(), blendTree->getGraphID(), blendTree->getFullName().c_str());

					blendTree->addPassDownPin(passDownPinName);
					passDownPinNode->updatePins();

					NodeEditor::Graph* currentGraph = blendTree;
					NodeEditor::Node* nodeToConnectTo = nullptr;

					while (currentGraph && !currentGraph->getNode(nodeID))
					{
						NodeEditor::Node* graphNode = currentGraph->getGraphNode();

						if (!graphNode)
						{
							INVOKE_PANIC(
								"Graph with ID %d (%s) does not have a graph node.\n",
								currentGraph->getGraphID(),
								currentGraph->getFullName().c_str());
						}

						if (currentGraph->isRootGraph())
							break;

						if (currentGraph->isOfType<NodeEditor::StateMachine>())
							currentGraph->addPassDownPin(passDownPinName);

						NodeEditor::Graph* parentGraph = currentGraph->getParentGraph();

						if (parentGraph->isOfType<NodeEditor::BlendTree>())
						{
							auto* parentBlendTree = parentGraph->asType<NodeEditor::BlendTree>();

							NodeEditor::PassDownPinsNode* parentPassDownPinNode = parentBlendTree->getPassDownPinsNode();
							NodeEditor::Pin* parentPassDownPin = parentPassDownPinNode->getOutputPin(passDownPinName);

							if (parentPassDownPin)
								parentPassDownPin->connectTo(graphNode->getInputPin(passDownPinName));
						}

						currentGraph = parentGraph;
						nodeToConnectTo = graphNode;
					}

					g_appLog->debugMessage(
						MsgLevel_Debug,
						"Node to connect to for pass down pin '%s' is %d (%s).\n",
						passDownPinName,
						nodeToConnectTo->getNodeID(),
						nodeToConnectTo->getFullName().c_str());

					if (multiplyConnectedNode->getParentGraph() != nodeToConnectTo->getParentGraph())
					{
						INVOKE_PANIC(
							"Graph containing multiply connected node ID %d (%s) is not the same as "
							"the graph containing the node %d (%s) to connect to.\n",
							multiplyConnectedNode->getNodeID(),
							multiplyConnectedNode->getFullName().c_str(),
							nodeToConnectTo->getNodeID(),
							nodeToConnectTo->getFullName().c_str());
					}

					if (nodeToConnectTo->getTypeName() == "BlendTree")
					{
						if (!nodeToConnectTo->getInputPin(passDownPinName))
							nodeToConnectTo->createInputPin(passDownPinName);
					}

					NodeEditor::Pin* nodeToConnectToPin = nodeToConnectTo->getInputPin(passDownPinName);
					NodeEditor::BlendTree* graphForConnection = multiplyConnectedNode->getParentGraph()->asType<NodeEditor::BlendTree>();

					if (!graphForConnection->hasConnectionBetween(multiplyConnectedTargetPin, nodeToConnectToPin))
						multiplyConnectedTargetPin->connectTo(nodeToConnectToPin);
				}

				NodeEditor::Pin* passDownPin = passDownPinNode->getOutputPin(passDownPinName);
				if (!passDownPin)
					INVOKE_PANIC(
						"Failed to find pass down pin '%s' in blend tree with ID %d (%s) for multiply connected node ID %d (%s).\n",
						passDownPinName,
						blendTree->getGraphID(),
						blendTree->getFullName().c_str(),
						multiplyConnectedNode->getNodeID(),
						multiplyConnectedNode->getFullName().c_str());

				passDownPin->connectTo(sourceNode->getInputDataPin(inputIdx));
			}
		}
	}
}

void NodeProcessor::processNodeConnectionsInBlendTree(NodeEditor::BlendTree* blendTree, MR::NodeDef* ownerNodeDef, std::vector<MR::NodeDef*>& childNodes)
{
	if (childNodes.size() == 0)
		INVOKE_PANIC("NodeProcessor::processNodeConnectionsInBlendTree: Invalid blend tree '%s'. No children node are present.\n", blendTree->getName().c_str());

	MR::NetworkDef* netDef = ownerNodeDef->getOwningNetworkDef();
	for (MR::NodeDef* childNodeDef : childNodes)
	{
		NodeEditor::Node* sourceNode = blendTree->getNode(childNodeDef->getNodeID());

		if (!sourceNode)
		{
			INVOKE_PANIC("NodeProcessor::processNodeConnectionsInBlendTree: Failed to find source node '%s' in blend tree '%s'.\n", getNodeName(childNodeDef->getNodeID()).c_str(), blendTree->getName().c_str());
			continue;
		}

		if (isNodeBlendTreeOutput(childNodeDef, blendTree))
			blendTree->connectToOutput(sourceNode->getOutputPin(0));
		else if (isNodeBlendTree(childNodeDef))
			continue;

		if (childNodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE))
			continue;

		for (uint32_t i = 0; i < childNodeDef->getNumChildNodes(); ++i)
		{
			MR::NodeDef* targetNodeDef = childNodeDef->getChildNodeDef(i);

			if (targetNodeDef->getNodeID() == MR::INVALID_NODE_ID)
				continue;

			NodeEditor::Node* targetNode = blendTree->getNode(targetNodeDef->getNodeID());

			// This is a pass down pin. We will handle it later.
			if (!targetNode)
				continue;

			targetNode->getOutputPin(0)->connectTo(sourceNode->getInputPin(i));
		}

		bool hasUnusuedPins = sourceNode->getNumInputDataPins() != childNodeDef->getNumInputCPConnections();

		int pinIndex = 0;
		for (uint32_t i = 0; i < childNodeDef->getNumInputCPConnections(); ++i)
		{
			if (!hasUnusuedPins)
				pinIndex = i;

			const MR::CPConnection* cpConnection = childNodeDef->getInputCPConnection(i);

			if (cpConnection->m_sourceNodeID == MR::INVALID_NODE_ID)
				continue;

			MR::NodeDef* targetNodeDef = netDef->getNodeDef(cpConnection->m_sourceNodeID);

			if (!targetNodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM))
			{
				NodeEditor::Node* targetNode = blendTree->getNode(targetNodeDef->getNodeID());

				// This is a pass down pin. We will handle it later.
				if (!targetNode)
				{
					pinIndex++;
					continue;
				}

				targetNode->getOutputDataPin(cpConnection->m_sourcePinIndex)->connectTo(sourceNode->getInputDataPin(pinIndex));
			}
			else
			{
				NodeEditor::ControlParameter* controlParam = blendTree->getOwnerEditor()->getControlParameter(targetNodeDef->getNodeID());

				if (!controlParam)
					INVOKE_PANIC("NodeProcessor::processNodeConnectionsInBlendTree: Failed to find control parameter '%s' in blend tree '%s'.\n", getNodeName(targetNodeDef->getNodeID()).c_str(), blendTree->getName().c_str());

				blendTree->getControlParameterDataPin(controlParam->getName())->connectTo(sourceNode->getInputDataPin(pinIndex));
			}

			pinIndex++;
		}
	}
}

bool NodeProcessor::setBlendTreeLayout(NodeEditor::BlendTree* blendTree, MR::NodeDef* btNodeDef, std::vector<MR::NodeDef*>& childNodes)
{
	if (!m_blendTreeLayouterStrategy)
	{
		g_appLog->debugMessage(MsgLevel_Warn, "NodeProcessor::setBlendTreeLayout: No blend tree layouter strategy set.\n");
		return false;
	}

	return m_blendTreeLayouterStrategy->setLayout(blendTree, btNodeDef, childNodes);
}

void NodeProcessor::processNodeTransitionsInStateMachine(NodeEditor::StateMachine* stateMachine, MR::NodeDef* nodeDef)
{
	for (size_t i = 0; i < nodeDef->getNumChildNodes(); i++)
	{
		MR::NodeDef* childNodeDef = nodeDef->getChildNodeDef(i);
		if (!childNodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_TRANSITION))
			continue;

		const MR::NodeID sourceNodeID = childNodeDef->getChildNodeID(0);
		const MR::NodeID targetNodeID = childNodeDef->getChildNodeID(1);

		NodeEditor::Node* sourceNode = stateMachine->getNode(sourceNodeID);
		NodeEditor::Node* targetNode = stateMachine->getNode(targetNodeID);

		// Active state transition
		if (sourceNodeID == MR::INVALID_NODE_ID)
		{
			bool existsStateToNode = false;
			for (size_t j = 0; j < stateMachine->getNumTransitions(); j++)
			{
				NodeEditor::Transition* transition = stateMachine->getTransitionAt(j);

				if (transition->getDestinationNode()->getNodeID() == targetNodeID && transition->getSourceNode()->isOfType<NodeEditor::StateNode>())
				{
					existsStateToNode = true;
					sourceNode = transition->getSourceNode();
					break;
				}
			}

			if (!existsStateToNode)
				sourceNode = stateMachine->createStateNode();
		}

		NodeEditor::Transition* transition = stateMachine->createTransition(childNodeDef->getNodeID(), transitTypeAsManifestName(childNodeDef->getNodeTypeID()), sourceNode, targetNode);
		
		if (!transition)
			INVOKE_PANIC("NodeProcessor::processNodeTransitionsInStateMachine: Failed to create transition '%s' in state machine '%s'.\n", getNodeName(childNodeDef->getNodeID()).c_str(), stateMachine->getName().c_str());
	}
}

bool NodeProcessor::setStateMachineLayout(NodeEditor::StateMachine* stateMachine, MR::NodeDef* smNodeDef)
{
	if (!m_stateMachineLayouterStrategy)
	{
		g_appLog->debugMessage(MsgLevel_Warn, "NodeProcessor::setStateMachineLayout: No state machine layouter strategy set.\n");
		return false;
	}

	std::vector<MR::NodeDef*> childNodes;
	for (size_t i = 0; i < smNodeDef->getNumChildNodes(); i++)
		childNodes.push_back(smNodeDef->getChildNodeDef(i));

	return m_stateMachineLayouterStrategy->setLayout(stateMachine, smNodeDef, childNodes);
}

bool NodeProcessor::isNetworkNodeNameMapComplete(MR::NetworkDef* netDef)
{
	for (uint32_t i = 0; i < netDef->getNumNodeDefs(); ++i)
	{
		MR::NodeDef* nodeDef = netDef->getNodeDef(i);

		if (strcmp(netDef->getNodeNameFromNodeID(nodeDef->getNodeID()), "") == 0)
			return false;
	}

	return true;
}

std::string NodeProcessor::getNodeName(const MR::NodeID nodeID)
{
	if (nodeID == MR::INVALID_NODE_ID)
		return "";

	auto it = m_nodeNameMap.find(nodeID);

	if (it != m_nodeNameMap.end())
		return it->second;

	return "";
}

std::string NodeProcessor::getBlendTreeNodeName(const MR::NodeID nodeID)
{
	if (nodeID == MR::INVALID_NODE_ID)
		return "";

	auto it = m_blendTreeNodeNames.find(nodeID);
	if (it != m_blendTreeNodeNames.end())
		return it->second;

	return "";
}

const std::string NodeProcessor::nodeTypeAsManifestName(const MR::NodeType type)
{
	switch (type)
	{
	case NODE_TYPE_STATE_MACHINE:							return "StateMachine";
	case NODE_TYPE_ANIMATION:								return "Animation";
	case NODE_TYPE_ANIM_EVENTS:								return "AnimWithEvents";
	case NODE_TYPE_FILTER_TRANSFORMS:						return "FilterTransforms";
	case NODE_TYPE_BLEND_2:									return "Blend2";
	case NODE_TYPE_BLEND_N:									return "BlendN";
	case NODE_TYPE_SINGLEFRAME:								return "SingleFrame";
	case NODE_TYPE_FREEZE:									return "Freeze";
	case NODE_TYPE_BLEND_2X2:								return "Blend2x2";
	case NODE_TYPE_BLEND_NXM:								return "BlendNxM";
	case NODE_TYPE_BLEND_ALL:								return "BlendAll";
	case NODE_TYPE_SUBTRACTIVE_BLEND:						return "SubtractiveBlend";
	case NODE_TYPE_CP_OP_FUNCTION:							return "OperatorFunction";
	case NODE_TYPE_CP_OP_ARITHMETIC:						return "OperatorArithmetic";
	case NODE_TYPE_CP_OP_ARITHMETIC_VECTOR3:				return "OperatorVector3Arithmetic";
	case NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC:				return "OperatorOneInputArithmetic";
	case NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC_VECTOR3:		return "OperatorOneInputVector3Arithmetic";
	case NODE_TYPE_CP_OP_NOISE_GEN:							return "OperatorNoiseGen";
	case NODE_TYPE_FEATHER_BLEND_2:							return "FeatherBlend2";
	case NODE_TYPE_APPLY_GLOBAL_TIME:						return "ApplyGlobalTime";
	case NODE_TYPE_TWO_BONE_IK:								return "TwoBoneIK";
	case NODE_TYPE_LOCK_FOOT:								return "LockFoot";
	case NODE_TYPE_HEAD_LOOK:								return "HeadLook";
	case NODE_TYPE_HIPS_IK:									return "HipsIK";
	case NODE_TYPE_PLAY_SPEED_MODIFIER:						return "PlaySpeedModifier";
	case NODE_TYPE_SCALE_TO_DURATION:						return "ScaleToDuration";
	case NODE_TYPE_EMIT_JOINT_CP_INFO:						return "ExtractJointInfo";
	case NODE_TYPE_EMIT_MESSAGE_ON_DISCRETE_EVENT:			return "EmitRequestOnDiscreteEvent";
	case NODE_TYPE_CLOSEST_ANIM:							return "ClosestAnim";
	case NODE_TYPE_SWITCH:									return "Switch";
	case NODE_TYPE_SEQUENCE: 								return "Sequence";
	case NODE_TYPE_PASSTHROUGH:								return "PassThrough";
	case NODE_MIRROR_TRANSFORMS_ID: 						return "MirrorTransforms";
	case NODE_TYPE_BASIC_UNEVEN_TERRAIN:					return "BasicUnevenTerrain";
	case NODE_TYPE_ACTIVE_STATE:							return "ActiveState";
	case NODE_TYPE_PREDICTIVE_UNEVEN_TERRAIN:				return "PredictiveUnevenTerrain";
	case NODE_TYPE_CP_OP_RAY_CAST:							return "OperatorRayCast";
	case NODE_TYPE_CP_OP_SMOOTH_FLOAT:						return "OperatorSmoothFloat";
	case NODE_TYPE_CP_OP_RAMP_FLOAT:						return "OperatorRampFloat";
	case NODE_TYPE_CP_OP_FLOATS_TO_VECTOR3:					return "OperatorFloatsToVector3";
	case NODE_TYPE_CP_OP_RANDOM_FLOAT:						return "OperatorRandomFloat";
	case NODE_TYPE_CP_OP_VECTOR3_TO_FLOATS:					return "OperatorVector3ToFloats";
	case NODE_TYPE_CP_OP_VECTOR3_DOT:						return "OperatorVector3Dot";
	case NODE_TYPE_CP_OP_VECTOR3_DISTANCE:					return "OperatorVector3Distance";
	case NODE_TYPE_CP_OP_VECTOR3_CROSSPRODUCT:				return "OperatorVector3CrossProduct";
	case NODE_TYPE_CP_OP_RATE_OF_CHANGE:					return "OperatorRateOfChange";
	case NODE_TYPE_CP_OP_VECTOR3_ANGLE:						return "OperatorVector3Angle";
	case NODE_TYPE_CP_OP_VECTOR3_NORMALISE:					return "OperatorVector3Normalise";
	case NODE_TYPE_GUN_AIM_IK:								return "GunAimIK";
	case NODE_TYPE_SMOOTH_TRANSFORMS:						return "SmoothTransforms";
	}

	INVOKE_PANIC("NodeProcessor::nodeTypeAsManifestName: Unsupported node type ID %d.\n", type);
}

const std::string NodeProcessor::transitTypeAsManifestName(const MR::NodeType type)
{
	switch (type)
	{
	case NODE_TYPE_TRANSIT:					return "Transit";
	case NODE_TYPE_TRANSIT_SYNC_EVENTS:		return "TransitMatchEvents";
	}

	INVOKE_PANIC("NodeProcessor::transitTypeAsManifestName: Unsupported transition type ID.\n");
}

bool NodeProcessor::isNodeBlendTreeOutput(MR::NodeDef* nodeDef, NodeEditor::BlendTree* bt)
{
	return nodeDef->getNodeID() == bt->getGraphID();
}

bool NodeProcessor::isNodeBlendTree(MR::NodeDef* nodeDef)
{
	return (m_blendTreeNodes.find(nodeDef->getNodeID()) != m_blendTreeNodes.end());
}

bool NodeProcessor::isNodeInBlendTree(MR::NodeDef* nodeDef)
{
	for (const auto& blendTreeNodePair : m_blendTreeNodeMap)
	{
		for (const auto& childNode : blendTreeNodePair.second)
		{
			if (childNode->getNodeID() == nodeDef->getNodeID())
				return true;
		}
	}

	return false;
}

MR::NodeDef* NodeProcessor::getCommonAncestor(MR::NetworkDef* netDef, const std::vector<MR::NodeDef*>& referencingNodes)
{
	if (referencingNodes.empty())
		return nullptr;

	auto collectBlendTreeChain = [&](MR::NodeDef* node)
		{
			std::vector<MR::NodeDef*> chain;

			if (isNodeBlendTree(node))
				chain.push_back(node);

			MR::NodeDef* parent = getParentNodeContainer(node);
			while (parent)
			{
				if (isNodeBlendTree(parent))
					chain.push_back(parent);

				parent = getParentNodeContainer(parent);
			}

			return chain;
		};

	std::vector<std::vector<MR::NodeDef*>> chains;
	chains.reserve(referencingNodes.size());

	// Build chains
	for (MR::NodeDef* node : referencingNodes)
	{
		auto chain = collectBlendTreeChain(node);

		if (chain.empty())
		{
			INVOKE_PANIC(
				"NodeProcessor::getCommonAncestor: Node %d (%s) has no BlendTree ancestor.\n",
				node->getNodeID(),
				netDef->getNodeNameFromNodeID(node->getNodeID()));
		}

		chains.push_back(std::move(chain));
	}

	// Intersect chains using the first as reference
	const auto& baseChain = chains.front();

	for (MR::NodeDef* candidate : baseChain)
	{
		bool isCommon = true;

		for (size_t i = 1; i < chains.size(); ++i)
		{
			const auto& chain = chains[i];

			if (std::find(chain.begin(), chain.end(), candidate) == chain.end())
			{
				isCommon = false;
				break;
			}
		}

		if (isCommon)
			return candidate;
	}

	return nullptr;
}


void NodeProcessor::collectContainerNodes(MR::NetworkDef* netDef)
{
	collectBlendTreeNodes(netDef);

	m_containerNodes.clear();

	const MR::NodeIDsArray* smArray = netDef->getStateMachineNodeIDs();
	for (size_t i = 0; i < smArray->getNumEntries(); i++)
	{
		const MR::NodeID nodeID = smArray->getEntry(i);
		MR::NodeDef* smNode = netDef->getNodeDef(nodeID);

		g_appLog->debugMessage(MsgLevel_Debug, "NodeProcessor::collectContainerNodes: Found container node %d (name=\"%s\").\n", smNode->getNodeID(), netDef->getNodeNameFromNodeID(smNode->getNodeID()));

		m_containerNodes[nodeID] = smNode;

		std::vector<MR::NodeDef*> subStateNodes;
		for (size_t j = 0; j < smNode->getNumChildNodes(); j++)
		{
			MR::NodeDef* childNode = smNode->getChildNodeDef(j);

			if (!childNode->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_TRANSITION))
				subStateNodes.push_back(childNode);
		}

		m_stateMachineNodeMap[nodeID] = subStateNodes;
	}

	for (const auto& blendTreeNode : m_blendTreeNodes)
	{
		g_appLog->debugMessage(MsgLevel_Debug, "NodeProcessor::collectContainerNodes: Found container node %d (name=\"%s\").\n", blendTreeNode.second->getNodeID(), netDef->getNodeNameFromNodeID(blendTreeNode.second->getNodeID()));

		m_containerNodes[blendTreeNode.first] = blendTreeNode.second;
	}
}

void NodeProcessor::collectBlendTreeNodes(MR::NetworkDef* netDef)
{
	m_blendTreeNodes.clear();

	MR::NodeDef* rootNodeDef = netDef->getNodeDef(netDef->getRootNodeID());

	m_blendTreeNodes[rootNodeDef->getNodeID()] = rootNodeDef;

	const MR::NodeIDsArray* smArray = netDef->getStateMachineNodeIDs();

	for (size_t i = 0; i < smArray->getNumEntries(); i++)
	{
		const MR::NodeID nodeID = smArray->getEntry(i);
		MR::NodeDef* smNode = netDef->getNodeDef(nodeID);

		for (size_t j = 0; j < smNode->getNumChildNodes(); j++)
		{
			MR::NodeDef* childNode = netDef->getNodeDef(smNode->getChildNodeID(j));

			MR::NodeDef::NodeFlags childNodeFlags = childNode->getNodeFlags();

			// Substate nodes that are not state machines or transitions are always blend tree nodes.
			if (!childNodeFlags.isSet(MR::NodeDef::NODE_FLAG_IS_TRANSITION) && !childNodeFlags.isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE))
			{
				g_appLog->debugMessage(MsgLevel_Debug, "NodeProcessor::collectBlendTreeNodes: Found blend tree node %d (name=\"%s\").\n", childNode->getNodeID(), netDef->getNodeNameFromNodeID(childNode->getNodeID()));

				m_blendTreeNodes[childNode->getNodeID()] = childNode;
			}
		}
	}
}

void NodeProcessor::collectBlendTreeChildNodes(MR::NetworkDef* netDef)
{
	m_blendTreeNodeMap.clear();

		// Recursive helper
	std::function<void(MR::NodeDef*, std::vector<MR::NodeDef*>&, std::vector<MR::NodeDef*>&)> collectChildren;
	collectChildren = [&](MR::NodeDef* node, std::vector<MR::NodeDef*>& outList, std::vector<MR::NodeDef*>& promotedNodes)
		{
			if (node->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE))
				return;

			for (size_t i = 0; i < node->getNumChildNodes(); ++i)
			{
				MR::NodeDef* childNode = netDef->getNodeDef(node->getChildNodeID(i));

				// Check if this node consumes a multiply-connected input
				if (childNode->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_OUTPUT_REFERENCED))
				{
					const MR::NodeID nodeID = node->getNodeID();

					if (!m_blendTreeNodes.count(nodeID))
					{
						// Promote parent node as new blend tree root
						m_blendTreeNodes[nodeID] = node;

						// Add to promoted nodes list for further processing
						promotedNodes.push_back(node);

						g_appLog->debugMessage(
							MsgLevel_Debug,
							"NodeProcessor::collectBlendTreeChildNodes: Promoted node %d (name=\"%s\") to blend tree root due to multiply-connected input %d (%s).\n",
							nodeID,
							netDef->getNodeNameFromNodeID(nodeID),
							childNode->getNodeID(),
							netDef->getNodeNameFromNodeID(childNode->getNodeID()));

						addNodeToList(outList, childNode);

						// Recurse normally
						if (childNode->getNodeTypeID() != NODE_TYPE_STATE_MACHINE)
							collectChildren(childNode, outList, promotedNodes);

						return;
					}

					continue;
				}

				addNodeToList(outList, childNode);

				// Recurse normally
				if (childNode->getNodeTypeID() != NODE_TYPE_STATE_MACHINE)
					collectChildren(childNode, outList, promotedNodes);
			}
		};

	// Work list of root nodes to process
	std::vector<MR::NodeDef*> workList;
	for (const auto& entry : m_blendTreeNodes)
		workList.push_back(entry.second);

	// Process each root, dynamically adding promoted nodes
	for (size_t i = 0; i < workList.size(); ++i)
	{
		MR::NodeDef* rootNode = workList[i];
		std::vector<MR::NodeDef*> childNodeList;
		childNodeList.push_back(rootNode);

		std::vector<MR::NodeDef*> promotedNodes;
		collectChildren(rootNode, childNodeList, promotedNodes);

		// Append newly promoted nodes to work list to ensure they are processed
		for (MR::NodeDef* newRoot : promotedNodes)
			workList.push_back(newRoot);

		m_blendTreeNodeMap[rootNode->getNodeID()] = childNodeList;
	}

	const MR::NodeID rootNodeID = netDef->getRootNodeID();

	std::vector<MR::NodeDef*> cpOutputNodes;

	// Add the root nodes
	for (size_t i = 1; i < netDef->getNumNodeDefs(); i++)
	{
		MR::NodeDef* nodeDef = netDef->getNodeDef(i);
		MR::NodeDef::NodeFlags flags = nodeDef->getNodeFlags();

		if (flags.isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM) || flags.isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE))
			continue;

		bool isNodeInBlendTree = false;

		// Check the blend tree node child list
		for (auto& blendTreeNodePair : m_blendTreeNodeMap)
		{
			if (blendTreeNodePair.first == rootNodeID)
				continue;

			if (doesListContainNode(blendTreeNodePair.second, nodeDef))
			{
				isNodeInBlendTree = true;
				break;
			}
		}

		if (isNodeInBlendTree)
			continue;

		if (nodeDef->getNumOutputCPPins() > 0)
			cpOutputNodes.push_back(nodeDef);

		const MR::NodeID parentNodeID = nodeDef->getParentNodeID();

		if (parentNodeID == MR::INVALID_NODE_ID)
			continue;

		MR::NodeDef* parentNodeDef = nodeDef->getParentNodeDef();

		if (parentNodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE))
			continue;

		if ((nodeDef->getNodeID() != rootNodeID) && 
			(parentNodeID == 0 || (doesListContainNode(m_blendTreeNodeMap[rootNodeID], parentNodeDef) && !doesListContainNode(m_blendTreeNodeMap[rootNodeID], nodeDef))))
		{
			addNodeToList(m_blendTreeNodeMap[rootNodeID], nodeDef);

			std::vector<MR::NodeDef*> promotedNodes;
			collectChildren(nodeDef, m_blendTreeNodeMap[netDef->getRootNodeID()], promotedNodes);
		}
	}

	// We then must find where the output CP nodes belong. First, find all of them, then count the number of nodes that reference them as input.
	// If there's more than one, find the common ancestor of all referencing nodes and add the output CP node as a child of the common ancestor. 
	// If there's only one, add the output CP node as a child of the referencing node.
	for (MR::NodeDef* cpOutputNode : cpOutputNodes)
	{
		std::vector<MR::NodeDef*> referencingNodes;
		getNodesWithThisAsInputCP(referencingNodes, netDef, cpOutputNode->getNodeID());

		if (referencingNodes.size() == 0)
			continue;

		if (referencingNodes.size() == 1)
		{
			g_appLog->debugMessage(MsgLevel_Debug, "Control parameter output node ID %d is only referenced by one node (ID %d).\n", cpOutputNode->getNodeID(), referencingNodes[0]->getNodeID());
			
			addNodeToList(m_blendTreeNodeMap[referencingNodes[0]->getNodeID()], cpOutputNode);
			continue;
		}

		MR::NodeDef* commonAncestor = getCommonAncestor(netDef, referencingNodes);
		if (!commonAncestor)
		{
			g_appLog->alertMessage(MsgLevel_Warn, "Failed to find common ancestor for control parameter output node ID %d. Defaulting to root node.\n", cpOutputNode->getNodeID());
			//commonAncestor = netDef->getNodeDef(netDef->getRootNodeID());
		}

		g_appLog->debugMessage(MsgLevel_Debug, "Common ancestor for CP output node ID %d is node ID %d (name=\"%s\").\n", cpOutputNode->getNodeID(), commonAncestor->getNodeID(), netDef->getNodeNameFromNodeID(commonAncestor->getNodeID()));

		addNodeToList(m_blendTreeNodeMap[commonAncestor->getNodeID()], cpOutputNode);

		std::vector<MR::NodeDef*> promotedNodes;
		collectChildren(cpOutputNode, m_blendTreeNodeMap[commonAncestor->getNodeID()], promotedNodes);

		// Check if the ancestor contains both nodes and the output CP node. In which case the node is not a pass down node, and we don't need to add it to the list of multiply connected CP output nodes.
		bool isNodePassDown = false;
		
		for (MR::NodeDef* referencingNode : referencingNodes)
		{
			if (!doesListContainNode(m_blendTreeNodeMap[commonAncestor->getNodeID()], referencingNode))
			{
				isNodePassDown = true;
				break;
			}
		}

		if (isNodePassDown)
			m_multiplyConnectedCPOutputNodes.push_back(cpOutputNode);
	}
}

bool NodeProcessor::collectNodeNames(MR::NetworkDef* netDef)
{
	if (!m_namingStrategy)
	{
		g_appLog->alertMessage(MsgLevel_Warn, "NodeProcessor::collectNodeNames: No node naming strategy set.");
		return false;
	}

	if (!m_namingStrategy->collectNodeNames(netDef, m_blendTreeNodeMap, m_stateMachineNodeMap, m_nodeNameMap, m_blendTreeNodeNames))
	{
		g_appLog->alertMessage(MsgLevel_Warn, "NodeProcessor::collectNodeNames: Node naming strategy failed to collect node names.");
		return false;
	}

	return true;
}

MR::NodeDef* NodeProcessor::getParentNodeContainer(MR::NodeDef* nodeDef)
{
	MR::NodeDef* parentNodeDef = nodeDef->getParentNodeDef();

	if (parentNodeDef == nullptr)
		return nullptr;

	if (m_containerNodes.find(parentNodeDef->getNodeID()) != m_containerNodes.end())
		return parentNodeDef;
	else
		return getParentNodeContainer(parentNodeDef);
}