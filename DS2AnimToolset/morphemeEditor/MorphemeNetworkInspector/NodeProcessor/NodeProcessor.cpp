#include <queue>
#include <unordered_map>
#include <cmath>

#include "NodeProcessor.h"

#include "RLog/RLog.h"
#include "extern.h"

#include "morpheme/mrNetworkDef.h"
#include "morpheme/Nodes/mrNodeStateMachine.h"

#include "NodeNamingStrategy/DefaultNodeNamingStrategy.h"
#include "NodeNamingStrategy/ReconstructParentChildNameStrategy.h"

#include "GraphLayouterStrategy/BTFanLayouterStrategy.h"

bool NodeProcessor::preProcessNetwork(MR::NetworkDef* netDef)
{
	if (isNetworkNodeNameMapComplete(netDef))
		m_namingStrategy = new DefaultNodeNamingStrategy();
	else
		m_namingStrategy = new ReconstructParentChildNameStrategy();

	m_blendTreeLayouterStrategy = new BTFanLayouterStrategy();

	collectContainerNodes(netDef);
	collectBlendTreeChildNodes(netDef);
	collectNodeNames(netDef);

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

	if (isNodeBlendTreeOutput(nodeDef) && (nodeDef->getNodeID() != graph->getGraphNodeID()))
	{
		g_appLog->debugMessage(MsgLevel_Info, "NodeProcessor::processNode: Creating blend tree node for node %d.\n", nodeDef->getNodeID());

		return graph->createBlendTree(nodeDef->getNodeID(), m_blendTreeNodeNames[graph->getGraphNodeID()]);
	}
	else if (nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE))
	{
		g_appLog->debugMessage(MsgLevel_Info, "NodeProcessor::processNode: Creating state machine node for node %d.\n", nodeDef->getNodeID());

		return graph->createStateMachine(nodeDef->getNodeID(), name);
	}

	if (!graph->isOfType<NodeEditor::BlendTree>())
		INVOKE_PANIC(
			"Non-container node '%s' inside state machine container",
			name.c_str());

	NodeEditor::Node* node = graph->asType<NodeEditor::BlendTree>()->createNode(nodeDef->getNodeID(), nodeTypeAsManifestName(nodeDef->getNodeTypeID()), name);

	// Set node attributes
	return node;
}

NodeEditor::Graph* NodeProcessor::buildRootGraph(NodeEditor::Editor* editor, MR::NodeDef* rootNodeDef)
{
	NodeEditor::Graph* rootGraph = nullptr;

	if (isNodeBlendTreeOutput(rootNodeDef))
		rootGraph = editor->createRootBlendTree(rootNodeDef->getNodeID());
	else
		rootGraph = editor->createRootStateMachine(rootNodeDef->getNodeID());

	populateGraph(rootGraph, rootNodeDef);
	populateSubGraphs(rootGraph, rootNodeDef);

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
		INVOKE_PANIC("NodeProcessor::populateGraph: Unsupported graph type for graph node ID %d.", graph->getGraphNodeID());
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

void NodeProcessor::processNodeConnectionsInBlendTree(NodeEditor::BlendTree* blendTree, MR::NodeDef* ownerNodeDef, std::vector<MR::NodeDef*>& childNodes)
{
	if (childNodes.size() == 0)
		INVOKE_PANIC("NodeProcessor::processNodeConnectionsInBlendTree: Invalid blend tree '%s'. No children node are present.", blendTree->getName().c_str());

	MR::NetworkDef* netDef = ownerNodeDef->getOwningNetworkDef();
	for (MR::NodeDef* childNodeDef : childNodes)
	{
		NodeEditor::Node* sourceNode = blendTree->getNode(childNodeDef->getNodeID());

		if (!sourceNode)
		{
			INVOKE_PANIC("NodeProcessor::processNodeConnectionsInBlendTree: Failed to find source node '%s' in blend tree '%s'.", getNodeName(childNodeDef->getNodeID()).c_str(), blendTree->getName().c_str());
			continue;
		}

		if (isNodeBlendTreeOutput(childNodeDef))
			blendTree->connectToOutput(sourceNode->getOutputPin(0));
		else if (childNodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE))
			continue;

		for (uint32_t i = 0; i < childNodeDef->getNumChildNodes(); ++i)
		{
			MR::NodeDef* targetNodeDef = netDef->getNodeDef(childNodeDef->getChildNodeID(i));

			if (targetNodeDef->getNodeID() == MR::INVALID_NODE_ID)
				continue;

			NodeEditor::Node* targetNode = blendTree->getNode(targetNodeDef->getNodeID());

			if (!targetNode)
			{
				INVOKE_PANIC("NodeProcessor::processNodeConnectionsInBlendTree: Failed to find target node '%s' in blend tree '%s'.", getNodeName(targetNodeDef->getNodeID()).c_str(), blendTree->getName().c_str());
				continue;
			}

			targetNode->getOutputPin(0)->connectTo(sourceNode->getInputPin(i));
		}

		for (uint32_t i = 0; i < childNodeDef->getNumInputCPConnections(); ++i)
		{
			const MR::CPConnection* cpConnection = childNodeDef->getInputCPConnection(i);
			MR::NodeDef* targetNodeDef = netDef->getNodeDef(cpConnection->m_sourceNodeID);

			if (targetNodeDef->getNodeID() == MR::INVALID_NODE_ID)
				continue;

			if (!targetNodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM))
			{
				NodeEditor::Node* targetNode = blendTree->getNode(targetNodeDef->getNodeID());

				if (!targetNode)
				{
					INVOKE_PANIC("NodeProcessor::processNodeConnectionsInBlendTree: Failed to find target node '%s' in blend tree '%s'.", getNodeName(targetNodeDef->getNodeID()).c_str(), blendTree->getName().c_str());
					continue;
				}

				targetNode->getOutputDataPin(cpConnection->m_sourcePinIndex)->connectTo(sourceNode->getInputDataPin(i));
			}
			else
			{
				NodeEditor::ControlParameter* controlParam = blendTree->getOwnerEditor()->getControlParameter(targetNodeDef->getNodeID());

				if (!controlParam)
				{
					INVOKE_PANIC("NodeProcessor::processNodeConnectionsInBlendTree: Failed to find control parameter '%s' in blend tree '%s'.", getNodeName(targetNodeDef->getNodeID()).c_str(), blendTree->getName().c_str());
					continue;
				}

				controlParam->getOutputPin()->connectTo(sourceNode->getInputDataPin(i));
			}
		}
	}
}

bool NodeProcessor::setBlendTreeLayout(NodeEditor::BlendTree* blendTree, MR::NodeDef* btNodeDef, std::vector<MR::NodeDef*>& childNodes)
{
	if (!m_blendTreeLayouterStrategy)
	{
		g_appLog->debugMessage(MsgLevel_Warn, "NodeProcessor::setBlendTreeLayout: No blend tree layouter strategy set.");
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
			sourceNode = stateMachine->getDefaultStateNode();

			if (!sourceNode)
				sourceNode = stateMachine->createStateNode();
		}

		NodeEditor::Transition* transition = stateMachine->createTransition(childNodeDef->getNodeID(), transitTypeAsManifestName(childNodeDef->getNodeTypeID()), sourceNode, targetNode);
		
		if (!transition)
			INVOKE_PANIC("NodeProcessor::processNodeTransitionsInStateMachine: Failed to create transition '%s' in state machine '%s'.", getNodeName(childNodeDef->getNodeID()).c_str(), stateMachine->getName().c_str());
	}
}

bool NodeProcessor::setStateMachineLayout(NodeEditor::StateMachine* stateMachine, MR::NodeDef* smNodeDef)
{
	if (!m_stateMachineLayouterStrategy)
	{
		g_appLog->debugMessage(MsgLevel_Warn, "NodeProcessor::setStateMachineLayout: No state machine layouter strategy set.");
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

	INVOKE_PANIC("NodeProcessor::nodeTypeAsManifestName: Unsupported node type ID.");
}

const std::string NodeProcessor::transitTypeAsManifestName(const MR::NodeType type)
{
	switch (type)
	{
	case NODE_TYPE_TRANSIT:					return "Transit";
	case NODE_TYPE_TRANSIT_SYNC_EVENTS:		return "TransitMatchEvents";
	}

	INVOKE_PANIC("NodeProcessor::transitTypeAsManifestName: Unsupported transition type ID.");
}

bool NodeProcessor::isNodeBlendTreeOutput(MR::NodeDef* nodeDef)
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

	if (rootNodeDef->getNodeTypeID() != NODE_TYPE_STATE_MACHINE)
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
	std::function<void(MR::NodeDef*, std::vector<MR::NodeDef*>&)> collectChildren;
	collectChildren = [&](MR::NodeDef* node, std::vector<MR::NodeDef*>& outList)
		{
			for (size_t i = 0; i < node->getNumChildNodes(); ++i)
			{
				MR::NodeDef* childNode = netDef->getNodeDef(node->getChildNodeID(i));

				outList.push_back(childNode);

				// Recurse into this child
				if (childNode->getNodeTypeID() != NODE_TYPE_STATE_MACHINE)
					collectChildren(childNode, outList);
			}
		};

	// Iterate over all root blend tree nodes
	for (const auto& blendTreeNode : m_blendTreeNodes)
	{
		std::vector<MR::NodeDef*> childNodeList;

		childNodeList.push_back(blendTreeNode.second);
		collectChildren(blendTreeNode.second, childNodeList);

		m_blendTreeNodeMap[blendTreeNode.first] = childNodeList;
	}

	for (const auto& blendTreeNodePair : m_blendTreeNodeMap)
	{
		for (const auto& childNode : blendTreeNodePair.second)
		{
			g_appLog->debugMessage(
				MsgLevel_Debug,
				"NodeProcessor::collectBlendTreeChildNodes: Blend tree node %d has child node %d (name=\"%s\").\n",
				blendTreeNodePair.first,
				childNode->getNodeID(),
				netDef->getNodeNameFromNodeID(childNode->getNodeID())
			);
		}
	}
}

bool NodeProcessor::collectNodeNames(MR::NetworkDef* netDef)
{
	if (!m_namingStrategy)
	{
		g_appLog->alertMessage(MsgLevel_Warn, "NodeProcessor::collectNodeNames: No node naming strategy set.");
		return false;
	}

	if (!m_namingStrategy->collectNodeNames(netDef, m_blendTreeNodes, m_nodeNameMap, m_blendTreeNodeNames))
	{
		g_appLog->alertMessage(MsgLevel_Warn, "NodeProcessor::collectNodeNames: Node naming strategy failed to collect node names.");
		return false;
	}

	sanitizeNodeNames(netDef);

	return true;
}

void NodeProcessor::sanitizeNodeNames(MR::NetworkDef* netDef)
{
	for (size_t i = 1; i < netDef->getNumNodeDefs(); i++)
	{
		MR::NodeDef* nodeDef = netDef->getNodeDef(i);

		MR::NodeDef::NodeFlags flags = nodeDef->getNodeFlags();
		if (flags.isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM) || flags.isSet(MR::NodeDef::NODE_FLAG_IS_TRANSITION))
			continue;

		char nameBuffer[256];
		sprintf_s(nameBuffer, "%s%d", nodeTypeAsManifestName(nodeDef->getNodeTypeID()).c_str(), nodeDef->getNodeID());

		if (m_nodeNameMap[nodeDef->getNodeID()].empty())
			m_nodeNameMap[nodeDef->getNodeID()] = nameBuffer;
	}
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