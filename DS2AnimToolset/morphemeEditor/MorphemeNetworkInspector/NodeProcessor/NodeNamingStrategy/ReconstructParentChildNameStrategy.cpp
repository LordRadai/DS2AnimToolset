#include <functional>
#include <unordered_set>

#include "ReconstructParentChildNameStrategy.h"

#include "Utils/Utils.h"

#include "RLog/RLog.h"
#include "extern.h"

#include "../NodeProcessor.h"

bool ReconstructParentChildNameStrategy::collectNodeNames(
    MR::NetworkDef* netDef,
    NodeProcessor* processor)
{
    std::map<MR::NodeID, std::string>& nodeNameMap =
        processor->getNodeNameMap();

    nodeNameMap.clear();

    MR::NodeDef* rootNodeDef =
        netDef->getNodeDef(netDef->getRootNodeID());

    std::unordered_set<MR::NodeID> visited;

    // -------------------------------------------------------------
    // Classification helper (single source of truth)
    // -------------------------------------------------------------
    auto classifyNode =
        [&](MR::NodeDef* node, const std::string& fullPath)
        {
            if (!node)
                return;

            const MR::NodeID id = node->getNodeID();

            const bool isBT =
                processor->isNodeBlendTree(node);

            const bool isSM =
                node->getNodeFlags().isSet(
                    MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE);

            const bool isSMNestedInBT =
                processor->isStateMachineNestedInBT(node);

            const std::string currentName =
                NodeNameStrategyUtils::getNodeNameFromFullPath(fullPath);

            const std::string parentPath =
                NodeNameStrategyUtils::getParentNodeNameFromFullPath(fullPath);

            const std::string parentName =
                NodeNameStrategyUtils::getNodeNameFromFullPath(parentPath);

            // -----------------------------------------------------
            // CASE 3: State Machine nested in Blend Tree
            // -----------------------------------------------------
            if (isSM && isSMNestedInBT)
            {
                processor->registerBlendTreeName(id, parentName);
                processor->registerStateMachineName(id, currentName);
                processor->registerNodeName(id, currentName);
            }
            // -----------------------------------------------------
            // CASE 1: Pure Blend Tree
            // -----------------------------------------------------
            else if (isBT)
            {
                processor->registerBlendTreeName(id, currentName);
                processor->registerNodeName(id, "");
            }
            // -----------------------------------------------------
            // CASE 2: Pure State Machine
            // -----------------------------------------------------
            else if (isSM)
            {
                processor->registerStateMachineName(id, currentName);
                processor->registerNodeName(id, currentName);
            }
            // -----------------------------------------------------
            // Normal Node
            // -----------------------------------------------------
            else
            {
                processor->registerNodeName(id, currentName);
            }
        };

    // -------------------------------------------------------------
    // Recursive traversal
    // -------------------------------------------------------------
    std::function<void(MR::NodeDef*)> collectNames;
    collectNames = [&](MR::NodeDef* nodeDef)
        {
            if (!nodeDef)
                return;

            const MR::NodeID id = nodeDef->getNodeID();

            // Prevent double processing
            if (!visited.insert(id).second)
                return;

            if (nodeDef->getNodeFlags().isSet(
                MR::NodeDef::NODE_FLAG_IS_TRANSITION))
                return;

            std::string nodePath =
                netDef->getNodeNameFromNodeID(id);

            // Classify this node
            classifyNode(nodeDef, nodePath);

            // Walk parent containers upward
            MR::NodeDef* parent =
                processor->getFirstContainerOfNode(nodeDef);

            std::string parentPath =
                NodeNameStrategyUtils::getParentNodeNameFromFullPath(nodePath);

            if (processor->isStateMachineNestedInBT(nodeDef))
            {
                parentPath =
                    NodeNameStrategyUtils::getParentNodeNameFromFullPath(parentPath);
            }

            while (parent &&
                parent->getNodeID() != netDef->getRootNodeID())
            {
                classifyNode(parent, parentPath);

                parentPath =
                    NodeNameStrategyUtils::getParentNodeNameFromFullPath(parentPath);

                if (processor->isStateMachineNestedInBT(parent))
                {
                    parentPath =
                        NodeNameStrategyUtils::getParentNodeNameFromFullPath(parentPath);
                }

                parent =
                    processor->getFirstContainerOfNode(parent);
            }

            // Recurse children
            for (size_t i = 0; i < nodeDef->getNumChildNodes(); ++i)
            {
                const MR::NodeID childID =
                    nodeDef->getChildNodeID(i);

                if (childID != MR::INVALID_NODE_ID)
                {
                    MR::NodeDef* childNode =
                        netDef->getNodeDef(childID);

                    collectNames(childNode);
                }
            }

            // Recurse CP sources
            for (size_t i = 0;
                i < nodeDef->getNumInputCPConnections();
                ++i)
            {
                const MR::CPConnection* cp =
                    nodeDef->getInputCPConnection(i);

                if (cp->m_sourceNodeID != MR::INVALID_NODE_ID)
                {
                    MR::NodeDef* sourceNode =
                        netDef->getNodeDef(cp->m_sourceNodeID);

                    if (!sourceNode->getNodeFlags().isSet(
                        MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM))
                    {
                        collectNames(sourceNode);
                    }
                }
            }
        };

    // Start traversal
    collectNames(rootNodeDef);

    // -------------------------------------------------------------
    // Final fallback name resolution
    // -------------------------------------------------------------
    for (auto& pair : nodeNameMap)
    {
        MR::NodeDef* nodeDef =
            netDef->getNodeDef(pair.first);

        // Blend trees without names
        if (processor->isNodeBlendTree(nodeDef))
        {
            std::vector<ContainerNodeInfo*> blendTrees =
                processor->getBlendTreesForNode(pair.first);

            for (size_t i = 0; i < blendTrees.size(); ++i)
            {
                if (blendTrees[i]->getName().empty())
                {
                    char buffer[256];
                    sprintf_s(buffer,
                        "BlendTree_%d_%d",
                        pair.first,
                        (int)i);

                    blendTrees[i]->setName(buffer);
                }
            }
        }

        // Empty node names get fallback
        if (pair.second.empty())
        {
            char buffer[256];
            sprintf_s(buffer,
                "%s_%d",
                NodeProcessor::nodeTypeAsManifestName(
                    nodeDef->getNodeTypeID()).c_str(),
                pair.first);

            pair.second = buffer;

            if (nodeDef->getNodeFlags().isSet(
                MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE))
            {
                processor->getStateMachineForNode(pair.first)
                    ->setName(buffer);
            }
        }

        g_appLog->debugMessage(
            MsgLevel_Debug,
            "ReconstructParentChildNameStrategy::collectNodeNames: "
            "Associated node name '%s' for node ID %d (type=%d).\n",
            pair.second.c_str(),
            pair.first,
            nodeDef->getNodeTypeID());
    }

    return true;
}