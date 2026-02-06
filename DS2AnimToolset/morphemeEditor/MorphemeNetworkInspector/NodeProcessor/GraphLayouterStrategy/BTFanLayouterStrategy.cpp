#include "BTFanLayouterStrategy.h"

#include "RLog/RLog.h"
#include "extern.h"

#include "NodeEditor/Editor/Graph/BlendTree.h"

#include "morpheme/mrNetworkDef.h"

constexpr float xStride = 300.f;
constexpr float yStride = 100.f;

void layoutNode(NodeEditor::BlendTree* blendTree, NodeEditor::Node* node, MR::NodeDef* nodeDef, float x, float y)
{
	if (!node || !nodeDef)
		return;

	MR::NetworkDef* netDef = nodeDef->getOwningNetworkDef();

	float xPos = x - xStride;
	float yPos = y - yStride;

	for (uint32_t i = 0; i < nodeDef->getNumChildNodes(); ++i)
	{
		NodeEditor::Node* childNode = blendTree->getNode(nodeDef->getChildNodeID(i));

		if (!childNode)
			continue;

		MR::NodeDef* childNodeDef = nodeDef->getChildNodeDef(i);

		int numInputNodes = childNodeDef->getNumChildNodes() + 1;

		if (childNodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE))
			numInputNodes = 1;

		const float vOffset = yStride * numInputNodes;

		ImVec2 nodeSize = childNode->getSize();
		childNode->setPosition(xPos - nodeSize.x, yPos);

		if (!childNodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE))
			layoutNode(blendTree, childNode, childNodeDef, xPos, yPos);

		yPos += vOffset;
	}

	for (uint32_t i = 0; i < nodeDef->getNumInputCPConnections(); ++i)
	{
		NodeEditor::Node* inputNode = blendTree->getNode(nodeDef->getInputCPConnection(i)->m_sourceNodeID);
		MR::NodeDef* inputNodeDef = netDef->getNodeDef(nodeDef->getInputCPConnection(i)->m_sourceNodeID);

		if (inputNode)
		{
			const float vOffset = yStride * (inputNodeDef->getNumInputCPConnections() + 1);

			ImVec2 nodeSize = inputNode->getSize();
			inputNode->setPosition(xPos - nodeSize.x, yPos + 100.f);

			layoutNode(blendTree, inputNode, inputNodeDef, xPos, yPos);

			yPos += vOffset;
		}
	}
}

bool BTFanLayouterStrategy::setLayout(NodeEditor::Graph* graph, MR::NodeDef* graphNodeDef, std::vector<MR::NodeDef*>& childNodes)
{
	if (!graph->isOfType<NodeEditor::BlendTree>())
	{
		g_appLog->alertMessage(MsgLevel_Error, "BTFanLayouterStrategy::setBlendTreeLayout: Graph '%s' is not a blend tree.", graph->getName().c_str());
		return false;
	}

	NodeEditor::BlendTree* blendTree = graph->asType<NodeEditor::BlendTree>();

	if (childNodes.empty())
		INVOKE_PANIC("NodeProcessor::setBlendTreeLayout: Invalid blend tree '%s'. No children nodes are present.", blendTree->getName().c_str());

	NodeEditor::Node* sourceNode = blendTree->getNode(graphNodeDef->getNodeID());
	MR::NodeDef* sourceNodeDef = graphNodeDef;

	if (!sourceNode)
		INVOKE_PANIC("NodeProcessor::setBlendTreeLayout: Failed to find source node %d in blend tree '%s'.",
			graphNodeDef->getNodeID(), blendTree->getName().c_str());

	ImVec2 startingPos(500.f, 550.f);

	sourceNode->setPosition(startingPos.x, startingPos.y);

	layoutNode(blendTree, sourceNode, sourceNodeDef, startingPos.x, startingPos.y);

	float minX = FLT_MAX;
	float maxY = 0.f;
	for (NodeEditor::Node* node : blendTree->getNodes())
	{
		ImVec2 pos = node->getPosition();
		if (pos.x < minX)
			minX = pos.x;
		if (pos.y > maxY)
			maxY = pos.y;
	}

	const float cpNodeOffsetX = 300.f;
	const float cpNodeOffsetY = 200.f;

	blendTree->setControlParamsNodePosition(minX - cpNodeOffsetX, maxY + cpNodeOffsetY);
}