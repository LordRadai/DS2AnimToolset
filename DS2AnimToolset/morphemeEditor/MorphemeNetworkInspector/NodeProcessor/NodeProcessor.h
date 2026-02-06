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
	std::map<MR::NodeID, MR::NodeDef*> m_containerNodes;
	std::map<MR::NodeID, std::string> m_nodeNameMap;

public:
	bool preProcessNetwork(MR::NetworkDef* netDef);

	NodeEditor::ControlParameter* processControlParameter(NodeEditor::Editor* editor, MR::NodeDef* nodeDef, const std::string& name);
	NodeEditor::Node* processNode(NodeEditor::Graph* graph, MR::NodeDef* nodeDef, const std::string& name);
	NodeEditor::Graph* buildRootGraph(NodeEditor::Editor* editor, MR::NodeDef* rootNodeDef);

	std::string getNodeName(const MR::NodeID nodeID);
	std::string getBlendTreeNodeName(const MR::NodeID nodeID);
private:
	static const std::string nodeTypeAsManifestName(const MR::NodeType type);
	static const std::string transitTypeAsManifestName(const MR::NodeType type);

	bool isNodeBlendTreeOutput(MR::NodeDef* nodeDef, NodeEditor::BlendTree* bt);
	bool isNodeBlendTree(MR::NodeDef* nodeDef);
	bool isNodeInBlendTree(MR::NodeDef* nodeDef);

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

	/*
	* \brief Get the source node and graph node for a pass down connection. This is used to create the corresponding pass down pin in the editor.
	* \param targetNode The node that is the target of the pass down connection. This is the node that will receive the pass down pin.
	* \param graphNode The node that represents the blend tree containing the target node. Should be used as target connector.
	* \param blendTree The blend tree containing the target node. Should be used to find the pass down pins node.
	* \param targetNodeDef The node def for the target node. Should be used to find the input pin index for the pass down connection.
	* \param sourceNode The node that is the source of the pass down connection. This is the node that will be connected to the pass down pin.
	* \param inputPinIndex The index of the input pin on the source node that should be connected to the pass down pin.
	* \param isTargetDataPin Whether the pass down connection is to a data pin on the target node. If false, the connection is to an execution pin.
	*/
	void getNodesForPassDownConnection(NodeEditor::Node** targetNode, NodeEditor::Node** graphNode, NodeEditor::BlendTree* blendTree, MR::NodeDef* targetNodeDef, NodeEditor::Node* sourceNode, size_t inputPinIndex, bool isTargetDataPin);

	/*
	* \brief Sanitize node names in the network to ensure there are no empty names. Must be done after collecting node names.
	* \param netDef The network definition to process.
	*/
	void sanitizeNodeNames(MR::NetworkDef* netDef);

	MR::NodeDef* getParentNodeContainer(MR::NodeDef* nodeDef);

	void populateGraph(NodeEditor::Graph* graph, MR::NodeDef* ownerNodeDef);
	void populateSubGraphs(NodeEditor::Graph* graph, MR::NodeDef* ownerNodeDef);

	void processNodeConnectionsInBlendTree(NodeEditor::BlendTree* blendTree, MR::NodeDef* ownerNodeDef, std::vector<MR::NodeDef*>& childNodes);
	bool setBlendTreeLayout(NodeEditor::BlendTree* blendTree, MR::NodeDef* btNodeDef, std::vector<MR::NodeDef*>& childNodes);

	void processNodeTransitionsInStateMachine(NodeEditor::StateMachine* stateMachine, MR::NodeDef* nodeDef);
	bool setStateMachineLayout(NodeEditor::StateMachine* stateMachine, MR::NodeDef* smNodeDef);

	bool isNetworkNodeNameMapComplete(MR::NetworkDef* netDef);
};
