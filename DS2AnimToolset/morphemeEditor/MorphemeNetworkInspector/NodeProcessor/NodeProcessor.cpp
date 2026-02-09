#include <queue>
#include <filesystem>
#include <vector>
#include <unordered_set>

#include "NodeProcessor.h"

#include "RLog/RLog.h"
#include "extern.h"

#include "morpheme/mrNetworkDef.h"

#include "NodeNamingStrategy/DefaultNodeNamingStrategy.h"
#include "NodeNamingStrategy/ReconstructParentChildNameStrategy.h"
#include "NodeNamingStrategy/Utils/Utils.h"

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

bool NodeProcessor::preProcessNetwork(MR::NetworkDef* netDef, MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* animNamesTable)
{
	m_blendTreeNodes.clear();
	m_stateMachineNodes.clear();
	m_nodeNameMap.clear();
	m_multiplyConnectedCPOutputNodes.clear();

	if (isNetworkNodeNameMapComplete(netDef))
		m_namingStrategy = new DefaultNodeNamingStrategy();
	else
		m_namingStrategy = new ReconstructParentChildNameStrategy();

	m_blendTreeLayouterStrategy = new BTFanLayouterStrategy();

	collectContainerNodes(netDef);
	collectNodeNames(netDef);
	fixupAnimNodeNames(netDef, animNamesTable);

	g_appLog->debugMessage(MsgLevel_Debug, "Root Node: %d (name=\"%s\")\n", netDef->getRootNodeID(), netDef->getNodeNameFromNodeID(netDef->getRootNodeID()));

	std::map<BlendTreeID, std::vector<MR::NodeDef*>> containerNodeMap;

	for (auto& blendTreeNodePair : m_blendTreeNodes)
	{
		const BlendTreeID& btKey = blendTreeNodePair.first;
		ContainerNodeInfo& containerInfo = blendTreeNodePair.second;

		MR::NodeDef* outputNode = containerInfo.getOutputNodeDef();

		g_appLog->debugMessage(
			MsgLevel_Debug,
			"BlendTree node %d (layer %u, name '%s') has %zu child nodes:\n",
			btKey.getNodeID(),
			btKey.getLayerIndex(),
			containerInfo.getName().c_str(),
			containerInfo.getChildNodeDefs().size());

		for (MR::NodeDef* childNode : containerInfo.getChildNodeDefs())
		{
			if (!childNode)
				continue;

			g_appLog->debugMessage(
				MsgLevel_Debug,
				"\t- Child node %d (%s)\n",
				childNode->getNodeID(),
				getNodeName(childNode->getNodeID()).c_str());
		}
	}

	for (auto& smNodePair : m_stateMachineNodes)
	{
		const MR::NodeID smNodeID = smNodePair.first;
		ContainerNodeInfo& containerInfo = smNodePair.second;

		g_appLog->debugMessage(
			MsgLevel_Debug,
			"StateMachine node %d (%s) has %zu child nodes:\n",
			smNodeID,
			containerInfo.getName().c_str(),
			containerInfo.getChildNodeDefs().size());

		for (MR::NodeDef* childNode : containerInfo.getChildNodeDefs())
		{
			if (!childNode)
				continue;

			g_appLog->debugMessage(
				MsgLevel_Debug,
				"\t- Child node %d (%s)\n",
				childNode->getNodeID(),
				getNodeName(childNode->getNodeID()).c_str());
		}
	}

	int numNetworkNodes = 0;
	std::vector<MR::NodeDef*> notFoundNodes;

	for (size_t i = 1; i < netDef->getNumNodeDefs(); i++)
	{
		MR::NodeDef* nodeDef = netDef->getNodeDef(i);
		if (!nodeDef)
			continue;

		MR::NodeDef::NodeFlags flags = nodeDef->getNodeFlags();

		// Skip control params or transitions
		if (flags.isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM) ||
			flags.isSet(MR::NodeDef::NODE_FLAG_IS_TRANSITION))
			continue;

		numNetworkNodes++;

		bool found = false;

		// 1. Check blend tree containers
		for (const auto& pair : m_blendTreeNodes)
		{
			const ContainerNodeInfo& containerInfo = pair.second;
			if (containerInfo.hasChildNodeDef(nodeDef))
			{
				found = true;
				break;
			}
		}

		// 2. Check state machine containers if not found yet
		if (!found)
		{
			for (const auto& pair : m_stateMachineNodes)
			{
				const ContainerNodeInfo& containerInfo = pair.second;
				if (containerInfo.hasChildNodeDef(nodeDef))
				{
					found = true;
					break;
				}
			}
		}

		// 3. If still not found, store it
		if (!found)
			notFoundNodes.push_back(nodeDef);
	}

	for (const auto& nodeDef : notFoundNodes)
		g_appLog->debugMessage(MsgLevel_Error, "NodeProcessor::preProcessNetwork: Node %d (name=\"%s\", type=\"%s\") is not referenced as a child of any blend tree or state machine node.\n", nodeDef->getNodeID(), getNodeName(nodeDef->getNodeID()).c_str(), nodeTypeAsManifestName(nodeDef->getNodeTypeID()).c_str());

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
	/*
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
	*/

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
	/*
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
	*/
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
	/*
	if (nodeID == MR::INVALID_NODE_ID)
		return "";

	for (const auto& blendTreeNodePair : m_blendTreeNodeNames)
	{
		if (blendTreeNodePair.first.getNodeID() == nodeID)
			return blendTreeNodePair.second;
	}
	*/

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
	if (nodeDef == nullptr)
		return false;

	for (const auto& pair : m_blendTreeNodes)
	{
		const ContainerNodeInfo& containerInfo = pair.second;

		MR::NodeDef* outputNodeDef = containerInfo.getOutputNodeDef();
		if (outputNodeDef && outputNodeDef->getNodeID() == nodeDef->getNodeID())
			return true;
	}

	return false;
}

bool NodeProcessor::isNodeInBlendTree(MR::NodeDef* nodeDef)
{
	/*
	for (const auto& blendTreeNodePair : m_blendTreeNodeMap)
	{
		for (const auto& childNode : blendTreeNodePair.second)
		{
			if (childNode->getNodeID() == nodeDef->getNodeID())
				return true;
		}
	}
	*/

	return false;
}

void NodeProcessor::registerBTNode(MR::NodeDef* nodeDef)
{
	bool existsBtWithID = false;
	int layerIdx = 0;

	for (auto& blendTreeNodePair : m_blendTreeNodes)
	{
		if (blendTreeNodePair.first.getNodeID() == nodeDef->getNodeID())
		{
			existsBtWithID = true;

			if (blendTreeNodePair.first.getLayerIndex() > layerIdx)
				layerIdx = blendTreeNodePair.first.getLayerIndex();
		}
	}

	layerIdx++;

	if (existsBtWithID)
		g_appLog->debugMessage(MsgLevel_Warn, "NodeProcessor::registerBlendTreeNode: Blend tree with ID %d has %d layers. Adding a new one.\n", nodeDef->getNodeID(), layerIdx);

	BlendTreeID blendTreeID(nodeDef->getNodeID(), layerIdx);
	m_blendTreeNodes[blendTreeID] = nodeDef;
}

void NodeProcessor::registerNodeAsBTChild(const MR::NodeID btNodeID, MR::NodeDef* nodeDef)
{
	ContainerNodeInfo* containerInfo = getTopLevelBlendTreeInfo(btNodeID);

	if (containerInfo)
		containerInfo->addChildNodeDef(nodeDef);
	else
		INVOKE_PANIC("NodeProcessor::registerNodeAsBTChild: Failed to find blend tree with ID %d.\n", btNodeID);
}

std::vector<ContainerNodeInfo*> NodeProcessor::getBlendTreesForNode(const MR::NodeID btNodeID)
{
	std::vector<ContainerNodeInfo*> containerNodes;

	for (auto& blendTreeNodePair : m_blendTreeNodes)
	{
		if (blendTreeNodePair.first.getNodeID() == btNodeID)
			containerNodes.push_back(&blendTreeNodePair.second);
	}

	return containerNodes;
}

ContainerNodeInfo* NodeProcessor::getBlendTreeForNode(const MR::NodeID btNodeID, uint16_t layerIdx)
{
	for (auto& blendTreeNodePair : m_blendTreeNodes)
	{
		if (blendTreeNodePair.first.getNodeID() == btNodeID && blendTreeNodePair.first.getLayerIndex() == layerIdx)
			return &blendTreeNodePair.second;
	}

	return nullptr;
}

ContainerNodeInfo* NodeProcessor::getTopLevelBlendTreeInfo(const MR::NodeID btNodeID)
{
	int16_t layerIdx = -1;
	for (auto& blendTreeNodePair : m_blendTreeNodes)
	{
		if (blendTreeNodePair.first.getNodeID() == btNodeID && blendTreeNodePair.first.getLayerIndex() > layerIdx)
			layerIdx = blendTreeNodePair.first.getLayerIndex();
	}

	return getBlendTreeForNode(btNodeID, layerIdx);
}

void NodeProcessor::registerSMNode(MR::NodeDef* nodeDef)
{
	m_stateMachineNodes[nodeDef->getNodeID()] = nodeDef;
}

void NodeProcessor::registerNodeAsSMChild(const MR::NodeID smNodeID, MR::NodeDef* nodeDef)
{
	auto it = m_stateMachineNodes.find(smNodeID);

	if (it != m_stateMachineNodes.end())
		it->second.addChildNodeDef(nodeDef);
	else
		INVOKE_PANIC("NodeProcessor::registerNodeAsSMChild: Failed to find state machine with ID %d.\n", smNodeID);
}

MR::NodeDef* NodeProcessor::getCommonAncestor(
	MR::NetworkDef* netDef,
	const std::vector<MR::NodeDef*>& nodesToGroup)
{
	if (nodesToGroup.empty())
		return nullptr;

	/* ---------------------------------------------------------------------- */
	/* Local lambda: collect all ancestors of a node (including itself)        */
	/* ---------------------------------------------------------------------- */

	auto collectAncestors =
		[&](MR::NodeDef* start, std::unordered_set<MR::NodeID>& outAncestors)
		{
			std::vector<MR::NodeDef*> stack;
			stack.push_back(start);

			while (!stack.empty())
			{
				MR::NodeDef* node = stack.back();
				stack.pop_back();

				const MR::NodeID id = node->getNodeID();
				if (!outAncestors.insert(id).second)
					continue;

				const MR::NodeID parentID = node->getParentNodeID();
				if (parentID != MR::INVALID_NODE_ID)
					stack.push_back(netDef->getNodeDef(parentID));
			}
		};

	/* ---------------------------------------------------------------------- */
	/* 1. Build ancestor sets for all grouped nodes                             */
	/* ---------------------------------------------------------------------- */

	std::vector<std::unordered_set<MR::NodeID>> ancestorSets;
	ancestorSets.reserve(nodesToGroup.size());

	for (MR::NodeDef* node : nodesToGroup)
	{
		std::unordered_set<MR::NodeID> ancestors;
		collectAncestors(node, ancestors);
		ancestorSets.push_back(std::move(ancestors));
	}

	/* ---------------------------------------------------------------------- */
	/* 2. Intersect ancestor sets                                               */
	/* ---------------------------------------------------------------------- */

	std::unordered_set<MR::NodeID> intersection = ancestorSets[0];

	for (size_t i = 1; i < ancestorSets.size(); ++i)
	{
		std::unordered_set<MR::NodeID> next;

		for (MR::NodeID id : intersection)
		{
			if (ancestorSets[i].count(id))
				next.insert(id);
		}

		intersection.swap(next);

		if (intersection.empty())
			break;
	}

	if (intersection.empty())
		return nullptr;

	/* ---------------------------------------------------------------------- */
	/* 3. Choose the deepest valid ancestor                                     */
	/* ---------------------------------------------------------------------- */

	MR::NodeDef* bestNode = nullptr;
	int bestDepth = -1;

	for (MR::NodeID id : intersection)
	{
		MR::NodeDef* node = netDef->getNodeDef(id);
		MR::NodeDef::NodeFlags flags = node->getNodeFlags();

		if (flags.isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM) ||
			flags.isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE) ||
			flags.isSet(MR::NodeDef::NODE_FLAG_IS_TRANSITION))
			continue;

		int depth = 0;
		MR::NodeDef* cur = node;

		while (cur->getParentNodeID() != MR::INVALID_NODE_ID)
		{
			++depth;
			cur = netDef->getNodeDef(cur->getParentNodeID());
		}

		if (depth > bestDepth)
		{
			bestDepth = depth;
			bestNode = node;
		}
	}

	return bestNode;
}

void NodeProcessor::collectContainerNodes(MR::NetworkDef* netDef)
{
	m_stateMachineNodes.clear();
	m_blendTreeNodes.clear();

	MR::NodeDef* rootNodeDef = netDef->getNodeDef(netDef->getRootNodeID());

	registerBTNode(rootNodeDef);

	const MR::NodeIDsArray* smArray = netDef->getStateMachineNodeIDs();
	for (size_t i = 0; i < smArray->getNumEntries(); i++)
	{
		const MR::NodeID nodeID = smArray->getEntry(i);
		MR::NodeDef* smNode = netDef->getNodeDef(nodeID);

		g_appLog->debugMessage(MsgLevel_Debug, "NodeProcessor::collectContainerNodes: Found container node %d (name=\"%s\").\n", smNode->getNodeID(), netDef->getNodeNameFromNodeID(smNode->getNodeID()));

		registerSMNode(smNode);

		for (size_t j = 0; j < smNode->getNumChildNodes(); j++)
		{
			MR::NodeDef* childNode = smNode->getChildNodeDef(j);
			MR::NodeDef::NodeFlags childNodeFlags = childNode->getNodeFlags();

			if (childNodeFlags.isSet(MR::NodeDef::NODE_FLAG_IS_TRANSITION))
				continue;

			registerNodeAsSMChild(nodeID, childNode);

			if (!childNodeFlags.isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE))
				registerBTNode(childNode);
		}
	}

	collectBlendTreeChildNodes(netDef);

	// Then collect all blend trees that group nodes with multiply connected inputs.
	const MR::NodeIDsArray* multiplyConnectedNodes = netDef->getMultiplyConnectedNodeIDs();
	for (size_t i = 0; i < multiplyConnectedNodes->getNumEntries(); i++)
	{
		const MR::NodeID nodeID = multiplyConnectedNodes->getEntry(i);
		MR::NodeDef* nodeDef = netDef->getNodeDef(nodeID);

		if (nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM))
			continue;

		g_appLog->debugMessage(MsgLevel_Debug, "NodeProcessor::collectContainerNodes: Found multiply connected node %d (name=\"%s\").\n", nodeDef->getNodeID(), netDef->getNodeNameFromNodeID(nodeDef->getNodeID()));
		
		std::vector<MR::NodeDef*> referencingNodes;
		getNodesWithThisAsInput(referencingNodes, netDef, nodeDef->getNodeID());

		MR::NodeDef* commonAncestor = getCommonAncestor(netDef, referencingNodes);

		if (!commonAncestor)
		{
			INVOKE_PANIC(
				"NodeProcessor::collectContainerNodes: Failed to find common ancestor blend tree for node %d (%s) with %d referencing nodes.\n",
				nodeDef->getNodeID(),
				netDef->getNodeNameFromNodeID(nodeDef->getNodeID()),
				referencingNodes.size());
			continue;
		}

		g_appLog->debugMessage(
			MsgLevel_Debug,
			"NodeProcessor::collectContainerNodes: Common ancestor blend tree for node %d (%s) is node %d (%s).\n",
			nodeDef->getNodeID(),
			netDef->getNodeNameFromNodeID(nodeDef->getNodeID()),
			commonAncestor->getNodeID(),
			netDef->getNodeNameFromNodeID(commonAncestor->getNodeID()));

		ContainerNodeInfo* containerNodeInfo = getTopLevelBlendTreeInfo(commonAncestor->getNodeID());

		std::vector<ContainerNodeInfo*> blendTreesForNode = getBlendTreesForNode(commonAncestor->getNodeID());
		if (blendTreesForNode.size() < 2)
			registerBTNode(commonAncestor);

		registerNodeAsBTChild(commonAncestor->getNodeID(), nodeDef);
	}

	// Print all BT nodes and SM nodes for debugging
	for (const auto& blendTreeNodePair : m_blendTreeNodes)
		g_appLog->debugMessage(MsgLevel_Debug, "NodeProcessor::collectContainerNodes: Registered node ID %d as Blend Tree (layer %d).\n", blendTreeNodePair.first.getNodeID(), blendTreeNodePair.first.getLayerIndex());

	for (const auto& stateMachineNodePair : m_stateMachineNodes)
		g_appLog->debugMessage(MsgLevel_Debug, "NodeProcessor::collectContainerNodes: Registered node ID %d as State Machine.\n", stateMachineNodePair.first);
}

void NodeProcessor::collectBlendTreeChildNodes(MR::NetworkDef* netDef)
{
	std::function<void(MR::NodeDef*, std::vector<MR::NodeDef*>&)> collectChildren;

	collectChildren = [&](MR::NodeDef* referenceNode, std::vector<MR::NodeDef*>& childNodeList)
		{
			if (!referenceNode)
				return;

			if (referenceNode->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE))
				return;

			for (size_t i = 0; i < referenceNode->getNumChildNodes(); i++)
			{
				MR::NodeDef* childNode = referenceNode->getChildNodeDef(i);

				if (!childNode || childNode->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_OUTPUT_REFERENCED))
					continue;

				childNodeList.push_back(childNode);
				collectChildren(childNode, childNodeList);
			}
		};

	for (auto& blendTreeNodePair : m_blendTreeNodes)
	{
		MR::NodeDef* btNodeDef = blendTreeNodePair.second.getOutputNodeDef();

		std::vector<MR::NodeDef*> childNodes;
		collectChildren(btNodeDef, childNodes);

		for (MR::NodeDef* childNode : childNodes)
		{
			g_appLog->debugMessage(MsgLevel_Debug, "NodeProcessor::collectBlendTreeChildNodes: Registering node ID %d as child of blend tree node ID %d (layer %d).\n", childNode->getNodeID(), btNodeDef->getNodeID(), blendTreeNodePair.first.getLayerIndex());
			registerNodeAsBTChild(btNodeDef->getNodeID(), childNode);
		}
	}

	m_multiplyConnectedCPOutputNodes.clear();
	
	for (size_t i = 0; i < netDef->getNumNodeDefs(); i++)
	{
		MR::NodeDef* nodeDef = netDef->getNodeDef(i);

		if (nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM))
			continue;

		if (nodeDef->getNumOutputCPPins() == 0)
			continue;

		std::vector<MR::NodeDef*> referencingNodes;
		getNodesWithThisAsInputCP(referencingNodes, netDef, nodeDef->getNodeID());

		if (referencingNodes.size() == 0)
			continue;

		if (referencingNodes.size() == 1)
		{
			g_appLog->debugMessage(MsgLevel_Debug, "Control parameter output node ID %d is only referenced by one node (ID %d).\n", nodeDef->getNodeID(), referencingNodes[0]->getNodeID());

			ContainerNodeInfo* containerInfo = getTopLevelBlendTreeInfo(referencingNodes[0]->getNodeID());
			if (!containerInfo)
				INVOKE_PANIC("NodeProcessor::collectBlendTreeChildNodes: Failed to find blend tree for node ID %d.\n", referencingNodes[0]->getNodeID());

			containerInfo->addChildNodeDef(nodeDef);

			continue;
		}

		MR::NodeDef* commonAncestor = getCommonAncestor(netDef, referencingNodes);
		if (!commonAncestor)
			INVOKE_PANIC("Failed to find common ancestor for control parameter output node ID %d. Defaulting to root node.\n", nodeDef->getNodeID());

		g_appLog->debugMessage(MsgLevel_Debug, "Common ancestor for CP output node ID %d is node ID %d (name=\"%s\").\n", nodeDef->getNodeID(), commonAncestor->getNodeID(), netDef->getNodeNameFromNodeID(commonAncestor->getNodeID()));


		ContainerNodeInfo* containerInfo = getTopLevelBlendTreeInfo(referencingNodes[0]->getNodeID());
		if (!containerInfo)
			INVOKE_PANIC("NodeProcessor::collectBlendTreeChildNodes: Failed to find blend tree for node ID %d.\n", referencingNodes[0]->getNodeID());

		containerInfo->addChildNodeDef(nodeDef);

		collectChildren(nodeDef, containerInfo->getChildNodeDefs());

		// Check if the ancestor contains both nodes and the output CP node. In which case the node is not a pass down node, and we don't need to add it to the list of multiply connected CP output nodes.
		bool isNodePassDown = false;

		for (MR::NodeDef* referencingNode : referencingNodes)
		{
			if (!doesListContainNode(containerInfo->getChildNodeDefs(), referencingNode))
			{
				isNodePassDown = true;
				break;
			}
		}

		if (isNodePassDown)
			m_multiplyConnectedCPOutputNodes.push_back(nodeDef);
	}
}

bool NodeProcessor::collectNodeNames(MR::NetworkDef* netDef)
{
	if (!m_namingStrategy)
	{
		g_appLog->alertMessage(MsgLevel_Warn, "NodeProcessor::collectNodeNames: No node naming strategy set.");
		return false;
	}

	if (!m_namingStrategy->collectNodeNames(netDef, this))
	{
		g_appLog->alertMessage(MsgLevel_Warn, "NodeProcessor::collectNodeNames: Node naming strategy failed to collect node names.");
		return false;
	}

	return true;
}

void NodeProcessor::fixupAnimNodeNames(MR::NetworkDef* netDef, MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* animNamesTable)
{
	for (size_t i = 0; i < netDef->getNumNodeDefs(); i++)
	{
		const MR::NodeDef* nodeDef = netDef->getNodeDef(i);

		if (nodeDef->getNodeTypeID() != NODE_TYPE_ANIM_EVENTS)
			continue;

		MR::AttribDataSourceAnim* sourceAnim = static_cast<MR::AttribDataSourceAnim*>(nodeDef->getAttribData(MR::ATTRIB_SEMANTIC_SOURCE_ANIM));

		if (!sourceAnim)
			INVOKE_PANIC("NodeProcessor::fixupAnimNodeNames: Anim events node with ID %d does not have a source anim attribute.\n", nodeDef->getNodeID());

		std::filesystem::path animSourceName = animNamesTable->getSourceFilename(sourceAnim->m_animAssetID);
		std::string animName = animSourceName.filename().replace_extension("").string();

		m_nodeNameMap[nodeDef->getNodeID()] = animName;

		g_appLog->debugMessage(MsgLevel_Debug, "NodeProcessor::fixupAnimNodeNames: Set name of anim events node ID %d to \"%s\" based on source anim asset ID %d.\n", nodeDef->getNodeID(), animName.c_str(), sourceAnim->m_animAssetID);
	}
}

MR::NodeDef* NodeProcessor::getParentNodeContainer(MR::NodeDef* nodeDef)
{
	if (nodeDef == nullptr)
		return nullptr;

	MR::NodeDef* parentNodeDef = nodeDef->getParentNodeDef();
	if (parentNodeDef == nullptr)
		return nullptr;

	// 1. Search blend tree containers
	for (auto& pair : m_blendTreeNodes)
	{
		const ContainerNodeInfo& containerInfo = pair.second;

		if (containerInfo.hasChildNodeDef(nodeDef))
			return containerInfo.getOutputNodeDef();
	}

	// 2. Search state machine containers
	for (auto& pair : m_stateMachineNodes)
	{
		const ContainerNodeInfo& containerInfo = pair.second;

		if (containerInfo.hasChildNodeDef(nodeDef))
			return containerInfo.getOutputNodeDef();
	}

	// 3. Recurse upwards
	return getParentNodeContainer(parentNodeDef);
}

void NodeProcessor::registerNodeName(MR::NodeID	nodeID, const std::string& name)
{
	if ((m_nodeNameMap.find(nodeID) != m_nodeNameMap.end()) &&
		(name != "") &&
		(m_nodeNameMap[nodeID] != "") &&
		(m_nodeNameMap[nodeID] != name))
	{
		g_appLog->alertMessage(MsgLevel_Warn, "DefaultNodeNamingStrategy::registerNodeName: Duplicate node name entry for node ID %d. (currentName=%s, name=%s)\n", nodeID, m_nodeNameMap[nodeID].c_str(), name.c_str());
		//return;
	}

	//g_appLog->debugMessage(MsgLevel_Debug, "DefaultNodeNamingStrategy::registerNodeName: Registering node name '%s' for node ID %d.\n", name.c_str(), nodeID);

	m_nodeNameMap[nodeID] = name;
}

void NodeProcessor::registerBlendTreeName(MR::NodeID nodeID, const std::string& name)
{
	std::vector<ContainerNodeInfo*> blendTrees = getBlendTreesForNode(nodeID);

	ContainerNodeInfo* topLevelBT = getBlendTreeForNode(nodeID, 1);

	if (!topLevelBT)
		INVOKE_PANIC("DefaultNodeNamingStrategy::registerBlendTreeName: Failed to find blend tree with node ID %d to register name '%s'.\n", nodeID, name.c_str());

	topLevelBT->setName(name);
}

void NodeProcessor::registerStateMachineName(MR::NodeID nodeID, const std::string& name)
{
	auto it = m_stateMachineNodes.find(nodeID);
	if (it != m_stateMachineNodes.end())
	{
		it->second.setName(name);
		g_appLog->debugMessage(MsgLevel_Debug, "DefaultNodeNamingStrategy::registerStateMachineName: Registered state machine name '%s' for node ID %d.\n", name.c_str(), nodeID);
		return;
	}

	g_appLog->alertMessage(MsgLevel_Warn, "DefaultNodeNamingStrategy::registerStateMachineName: Failed to find state machine with node ID %d to register name '%s'.\n", nodeID, name.c_str());
}

bool NodeProcessor::isNodeStateNode(MR::NodeDef* nodeDef)
{
	for (auto& stateMachineNodePair : m_stateMachineNodes)
	{
		const ContainerNodeInfo& containerInfo = stateMachineNodePair.second;

		if (containerInfo.hasChildNodeDef(nodeDef))
			return true;
	}

	return false;
}