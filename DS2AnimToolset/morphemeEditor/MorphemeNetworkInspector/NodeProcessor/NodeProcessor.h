#pragma once
#include <vector>
#include <map>

#include "morpheme/mrNodeDef.h"
#include "NodeNamingStrategy/NodeNamingStrategy.inl"
#include "GraphLayouterStrategy/GraphLayouterStrategy.inl"
#include "NodeEditor/NodeEditor.h"
#include "NodeEditor/Editor/ControlParameter/ControlParameter.h"
#include "NodeEditor/Editor/Node/Node.h"

class NodeProcessor
{
	NodeNamingStrategy* m_namingStrategy;
	GraphLayouterStrategy* m_blendTreeLayouterStrategy;
	GraphLayouterStrategy* m_stateMachineLayouterStrategy;

	std::map<MR::NodeID, MR::NodeDef*> m_blendTreeNodes;
	std::map<MR::NodeID, std::string> m_blendTreeNodeNames;
	std::map<MR::NodeID, std::vector<MR::NodeDef*>> m_blendTreeNodeMap;
	std::map<MR::NodeID, std::vector<MR::NodeDef*>> m_stateMachineNodeMap;
	std::map<MR::NodeID, MR::NodeDef*> m_containerNodes;
	std::map<MR::NodeID, std::string> m_nodeNameMap;
	std::vector<MR::NodeDef*> m_multiplyConnectedCPOutputNodes;

public:
	static const std::string nodeTypeAsManifestName(const MR::NodeType type);
	static const std::string transitTypeAsManifestName(const MR::NodeType type);

	bool preProcessNetwork(MR::NetworkDef* netDef);

	NodeEditor::ControlParameter* processControlParameter(NodeEditor::Editor* editor, MR::NodeDef* nodeDef, const std::string& name);
	NodeEditor::Node* processNode(NodeEditor::Graph* graph, MR::NodeDef* nodeDef, const std::string& name);
	NodeEditor::Graph* buildRootGraph(NodeEditor::Editor* editor, MR::NodeDef* rootNodeDef);

	std::string getNodeName(const MR::NodeID nodeID);
	std::string getBlendTreeNodeName(const MR::NodeID nodeID);
private:
	bool isNodeBlendTreeOutput(MR::NodeDef* nodeDef, NodeEditor::BlendTree* bt);
	bool isNodeBlendTree(MR::NodeDef* nodeDef);
	bool isNodeInBlendTree(MR::NodeDef* nodeDef);

	MR::NodeDef* getCommonAncestor(MR::NetworkDef* netDef, const std::vector<MR::NodeDef*>& referencingNodes);

	/*
	* \brief Collect all container nodes (state machines and blend trees) in the network.
	* \param netDef The network definition to process.
	*/
	void collectContainerNodes(MR::NetworkDef* netDef);

	/*
	* \brief Collect all blend tree nodes in the network.
	* \param netDef The network definition to process.
	*/
	void collectBlendTreeNodes(MR::NetworkDef* netDef);

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

	MR::NodeDef* getParentNodeContainer(MR::NodeDef* nodeDef);

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
