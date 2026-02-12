#pragma once
#include <vector>
#include <map>

#include "morpheme/mrNodeDef.h"
#include "simpleBundle/simpleAnimRuntimeIDtoFilenameLookup.h"

#include "NodeNamingStrategy/NodeNamingStrategy.inl"
#include "GraphLayouterStrategy/GraphLayouterStrategy.inl"
#include "NodeEditor/NodeEditor.h"
#include "NodeEditor/Editor/ControlParameter/ControlParameter.h"
#include "NodeEditor/Editor/Node/Node.h"

union BlendTreeID
{
	struct
	{
		MR::NodeID m_nodeID;      // 16 bits
		uint16_t m_layerIndex;     // 16 bits
	};
	uint32_t m_combined;          // full 32-bit representation

	// Constructors
	BlendTreeID() : m_combined(0) {}
	BlendTreeID(MR::NodeID nodeID, uint16_t layerIndex)
		: m_nodeID(nodeID), m_layerIndex(layerIndex) {
	}

	// Accessors
	MR::NodeID getNodeID() const { return m_nodeID; }
	uint16_t getLayerIndex() const { return m_layerIndex; }
	uint32_t getBlendTreeID() const { return m_combined; }

	// Comparison operator for std::map
	bool operator<(const BlendTreeID& other) const
	{
		return m_combined < other.m_combined;
	}
};

class ContainerNodeInfo
{
	MR::NodeDef* m_outputNodeDef;
	std::vector<MR::NodeDef*> m_childNodeDefs;
	std::string m_name;

public:
	ContainerNodeInfo() : m_outputNodeDef(nullptr) {}
	ContainerNodeInfo(MR::NodeDef* nodeDef) : m_outputNodeDef(nodeDef) {}

	MR::NodeDef* getOutputNodeDef() const { return m_outputNodeDef; }

	void addChildNodeDef(MR::NodeDef* childNodeDef)
	{
		if (!hasChildNode(childNodeDef))
			m_childNodeDefs.push_back(childNodeDef);
	}

	bool hasChildNode(MR::NodeDef* childNodeDef) const
	{
		for (size_t i = 0; i < m_childNodeDefs.size(); i++)
		{
			if (m_childNodeDefs[i]->getNodeID() == childNodeDef->getNodeID())
				return true;
		}

		return false;
	}

	bool hasChildNode(const MR::NodeID nodeID) const
	{
		for (size_t i = 0; i < m_childNodeDefs.size(); i++)
		{
			if (m_childNodeDefs[i]->getNodeID() == nodeID)
				return true;
		}

		return false;
	}

	std::string getName() const { return m_name; }
	void setName(const std::string& name) { m_name = name; }

	std::vector<MR::NodeDef*>& getChildNodeDefs() { return m_childNodeDefs; }
};

class NodeProcessor
{
	NodeNamingStrategy* m_namingStrategy;
	GraphLayouterStrategy* m_blendTreeLayouterStrategy;
	GraphLayouterStrategy* m_stateMachineLayouterStrategy;

	std::map<BlendTreeID, ContainerNodeInfo> m_blendTreeNodes;
	std::map<MR::NodeID, ContainerNodeInfo> m_stateMachineNodes;
	std::map<MR::NodeID, std::string> m_nodeNameMap;
	std::map<MR::NodeID, std::vector<MR::NodeDef*>> m_inputNodeLookupTable;
	std::map<MR::NodeID, std::vector<MR::NodeDef*>> m_inputCpLookupTable;
	std::vector<MR::NodeDef*> m_cpOutputNodes;
	std::vector<MR::NodeDef*> m_multiplyConnectedCPOutputNodes;

public:
	static const std::string nodeTypeAsManifestName(const MR::NodeType type);
	static const std::string transitTypeAsManifestName(const MR::NodeType type);

	bool preProcessNetwork(MR::NetworkDef* netDef, MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* animNamesTable);

	NodeEditor::ControlParameter* processControlParameter(NodeEditor::Editor* editor, MR::NodeDef* nodeDef, const std::string& name);
	NodeEditor::Node* processNode(NodeEditor::Graph* graph, MR::NodeDef* nodeDef, const std::string& name);
	NodeEditor::Graph* buildRootGraph(NodeEditor::Editor* editor, MR::NodeDef* rootNodeDef);

	std::string getNodeName(const MR::NodeID nodeID);
	std::string getBlendTreeNodeName(const MR::NodeID nodeID);

	bool isNodeBlendTreeOutput(MR::NodeDef* nodeDef, NodeEditor::BlendTree* bt);
	bool isNodeBlendTree(MR::NodeDef* nodeDef);
	bool isNodeInBlendTree(MR::NodeDef* nodeDef);

	void registerBTNode(MR::NodeDef* nodeDef);
	void registerNodeAsBTChild(const MR::NodeID btNodeID, MR::NodeDef* nodeDef);
	std::vector<ContainerNodeInfo*> getBlendTreesForNode(const MR::NodeID btNodeID);
	ContainerNodeInfo* getBlendTreeForNode(const MR::NodeID btNodeID, uint16_t layerIdx);
	ContainerNodeInfo* getTopLevelBlendTreeInfo(const MR::NodeID btNodeID);

	void registerSMNode(MR::NodeDef* nodeDef);
	void registerNodeAsSMChild(const MR::NodeID smNodeID, MR::NodeDef* nodeDef);
	ContainerNodeInfo* getStateMachineForNode(const MR::NodeID smNodeID);

	std::map<MR::NodeID, std::string>& getNodeNameMap() { return m_nodeNameMap; }
	std::map<BlendTreeID, ContainerNodeInfo>& getBlendTreeNodes() { return m_blendTreeNodes; }
	std::map<MR::NodeID, ContainerNodeInfo>& getStateMachineNodes() { return m_stateMachineNodes; }

	void registerNodeName(MR::NodeID nodeID, const std::string& name);
	void registerBlendTreeName(MR::NodeID nodeID, const std::string& name);
	void registerStateMachineName(MR::NodeID nodeID, const std::string& name);

	bool isNodeStateNode(MR::NodeDef* nodeDef);
	bool isStateMachineNestedInBT(MR::NodeDef* smNodeDef);

	void getStateMachinesWithThisAsChild(std::vector<MR::NodeDef*>& outNodes, MR::NodeID nodeID);

	void getNodesWithThisAsInput(std::vector<MR::NodeDef*>& outNodes, MR::NodeID nodeID);
	void getNodesWithThisAsInputCP(std::vector<MR::NodeDef*>& outNodes, MR::NodeID nodeID);

	void dumpNetworkLayout(const std::wstring& outPath, MR::NetworkDef* netDef);

	/*
	* \brief Get the parent node container (blend tree or state machine) for a given node for layout purposes. This is used to determine which container a node should be laid out within when dumping the network layout. For nodes that are referenced by multiple parent nodes, this will attempt to find the most appropriate parent container based on the structure of the network and the referencing nodes. For nodes that are not referenced by any parent nodes, this will return the root node of the network.
	* \param nodeDef The node to find the parent container for.
	*/
	MR::NodeDef* getParentNodeContainer(MR::NodeDef* nodeDef, uint16_t btLayer = 0);

	/*
	* \brief Get the parent node container (blend tree or state machine) for a given node. Needs all nodes to be laid out in the approriate containers.
	* \param nodeDef The node to find the parent container for.
	*/
	MR::NodeDef* getFirstContainerOfNode(MR::NodeDef* nodeDef);
private:
	/*
	* \brief Get the common ancestor container node (blend tree or state machine) for a set of referencing nodes.
	* \param netDef The network definition to search within.
	* \param referencingNodes The nodes that reference the node in question as a child node. These are the nodes we want to find the common ancestor container for.
	*/
	MR::NodeDef* getCommonAncestorContainer(MR::NetworkDef* netDef, const std::vector<MR::NodeDef*>& referencingNodes, bool excludeSelf);

	MR::NodeDef* getCommonAncestorForBTCreation(MR::NetworkDef* netDef, const std::vector<MR::NodeDef*>& referencingNodes, bool lookInSMs);

	/*
	* \brief Collect all container nodes (state machines and blend trees) in the network.
	* \param netDef The network definition to process.
	*/
	void collectContainerNodes(MR::NetworkDef* netDef);

	/*
	* \brief Collect all child nodes for each blend tree in the network. This must be done after having collected the blend tree nodes.
	* \param netDef The network definition to process.
	*/
	void collectBlendTreeChildNodes(MR::NetworkDef* netDef);

	/*
	* \brief Collect all node names in the network and register them.
	* \param netDef The network definition to process.
	*/
	bool collectNodeNames(MR::NetworkDef* netDef);

	/*
	* \brief Fix up the names of animation nodes in the network using the provided anim names table. This should be done after collecting all node names.
	* \param netDef The network definition to process.
	* \param animNamesTable The table to use for looking up animation names from runtime IDs.
	*/
	void fixupAnimNodeNames(MR::NetworkDef* netDef, MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* animNamesTable);

	void populateGraph(NodeEditor::Graph* graph, MR::NodeDef* ownerNodeDef);
	void populateSubGraphs(NodeEditor::Graph* graph, MR::NodeDef* ownerNodeDef);

	void createPassDownConnection(NodeEditor::BlendTree* blendTree, NodeEditor::PassDownPinsNode* passDownPinNode, NodeEditor::Node* multiplyConnectedNode, NodeEditor::Node* sourceNode, MR::NodeID nodeID, int inputIdx, bool isCPConnection);

	/*
	* \brief Process nodes that are multiply connected (i.e. referenced by more than 1 parent node) in the network. This involves creating duplicate nodes for each additional reference and connecting them appropriately. Must be done after having solved all other connections.
	* \param editor The editor to create nodes and connections in.
	* \param netDef The network definition to process.
	*/
	void processMultiplyConnectedNodes(NodeEditor::Editor* editor, MR::NetworkDef* netDef);

	/*
	* \brief Connect child nodes within a Blend Tree. Will skip pass down nodes. Must be done after populating all graphs and subgraphs.
	* \param blendTree The blend tree to process.
	* \param ownerNodeDef The node def owning the blend tree. This is needed to get the child nodes of the blend tree from the blend tree node map.
	* \param childNodes The child nodes of the blend tree, to avoid having to look them up again from the map. These are needed to find the connections between child nodes and to skip pass down nodes.
	*/
	void processNodeConnectionsInBlendTree(NodeEditor::BlendTree* blendTree, MR::NodeDef* ownerNodeDef, std::vector<MR::NodeDef*>& childNodes);
	bool setBlendTreeLayout(NodeEditor::BlendTree* blendTree, MR::NodeDef* btNodeDef, std::vector<MR::NodeDef*>& childNodes);

	void processNodeTransitionsInStateMachine(NodeEditor::StateMachine* stateMachine, MR::NodeDef* nodeDef);
	bool setStateMachineLayout(NodeEditor::StateMachine* stateMachine, MR::NodeDef* smNodeDef);

	bool isNetworkNodeNameMapComplete(MR::NetworkDef* netDef);
};
