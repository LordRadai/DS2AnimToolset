#include "MorphemeNetworkInspector.h"

#include "morpheme/mrDefines.h"

#include "NodeProcessor/NodeNamingStrategy/Utils/Utils.h"

#include "RLog/RLog.h"
#include "extern.h"

bool MorphemeNetworkInspector::loadNetwork(MR::NetworkDef* network)
{
	const int numControlParams = network->getNumControlParameterNodes();

	if (network->getNumMultiplyConnectedNodes() != numControlParams)
	{
		g_appLog->debugMessage(MsgLevel_Error, "MorphemeNetworkInspector::loadNetwork: Networks with multiply connected nodes are not yet supported.");
		return false;
	}

	m_nodeProcessor.preProcessNetwork(network);

	std::vector<MR::NodeID> controlParamNodeIDs(numControlParams);
	network->getControlParameterNodeIDs(controlParamNodeIDs.data(), numControlParams);

	for (int i = 0; i < numControlParams; ++i)
	{
		MR::NodeDef* nodeDef = network->getNodeDef(controlParamNodeIDs[i]);
		const char* nodeName = network->getNodeNameFromNodeID(controlParamNodeIDs[i]);

		m_nodeProcessor.processControlParameter(this, nodeDef, NodeNameStrategyUtils::getNodeNameFromFullPath(nodeName));
	}

	for (uint32_t i = 0; i < network->getNumMessages(); ++i)
	{
		const MR::MessageDistributor* messageDef = network->getMessageDistributor(i);
		const char* name = network->getMessageNameFromMessageID(messageDef->m_messageID);

		m_messageProcessor.processMessage(this, const_cast<MR::MessageDistributor*>(messageDef), name);
	}

	MR::NodeDef* rootNodeDef = network->getNodeDef(network->getRootNodeID());

	NodeEditor::Graph* root = m_nodeProcessor.buildRootGraph(this, rootNodeDef);

	return true;
}