#include "MorphemeNetworkInspector.h"

#include "morpheme/mrDefines.h"

#include "NodeProcessor/NodeNamingStrategy/Utils/Utils.h"

#include "RLog/RLog.h"
#include "extern.h"

bool MorphemeNetworkInspector::loadNetwork(MR::NetworkDef* networkDef, MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* animFileLookupTable)
{
	const int numControlParams = networkDef->getNumControlParameterNodes();

	if (networkDef->isPhysical())
	{
		g_appLog->alertMessage(MsgLevel_Info, "MorphemeNetworkInspector::loadNetwork: Physical networks are not supported.");
		return false;
	}

	m_nodeProcessor.preProcessNetwork(networkDef, animFileLookupTable);

	std::vector<MR::NodeID> controlParamNodeIDs(numControlParams);
	networkDef->getControlParameterNodeIDs(controlParamNodeIDs.data(), numControlParams);

	for (int i = 0; i < numControlParams; ++i)
	{
		MR::NodeDef* nodeDef = networkDef->getNodeDef(controlParamNodeIDs[i]);
		const char* nodeName = networkDef->getNodeNameFromNodeID(controlParamNodeIDs[i]);

		m_nodeProcessor.processControlParameter(this, nodeDef, NodeNameStrategyUtils::getNodeNameFromFullPath(nodeName));
	}

	for (uint32_t i = 0; i < networkDef->getNumMessages(); ++i)
	{
		const MR::MessageDistributor* messageDef = networkDef->getMessageDistributor(i);
		const char* name = networkDef->getMessageNameFromMessageID(messageDef->m_messageID);

		m_messageProcessor.processMessage(this, const_cast<MR::MessageDistributor*>(messageDef), name);
	}

	MR::NodeDef* rootNodeDef = networkDef->getNodeDef(networkDef->getRootNodeID());

	NodeEditor::Graph* root = m_nodeProcessor.buildRootGraph(this, rootNodeDef);

	return true;
}