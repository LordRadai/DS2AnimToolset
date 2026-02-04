#pragma once
#include <vector>
#include <map>

#include "morpheme/mrNodeDef.h"
#include "NodeEditor/NodeEditor.h"
#include "NodeEditor/Editor/ControlParameter/ControlParameter.h"
#include "NodeEditor/Editor/Node/Node.h"

class NodeProcessor
{
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
private:
	static const std::string getNodeNameFromFullPath(const std::string& name);
	static const std::string getNodeNameWithParentFromFullPath(const std::string& name);
	static const std::string getBlendTreeNodeName(const std::string& name);

	static const std::string nodeTypeAsManifestName(const MR::NodeType type);
	static const std::string transitTypeAsManifestName(const MR::NodeType type);

	bool isNodeBlendTreeOutput(MR::NodeDef* nodeDef);
	bool isNodeInBlendTree(MR::NodeDef* nodeDef);

	void collectContainerNodes(MR::NetworkDef* netDef);
	void collectBlendTreeNodes(MR::NetworkDef* netDef);
	void collectBlendTreeChildNodes(MR::NetworkDef* netDef);
	void collectNodeNames(MR::NetworkDef* netDef);
	void sanitizeNodeNames(MR::NetworkDef* netDef);

	MR::NodeDef* getParentNodeContainer(MR::NodeDef* nodeDef);

	void registerNodeName(MR::NodeID nodeID, const std::string& name);

	void populateGraph(NodeEditor::Graph* graph, MR::NodeDef* ownerNodeDef);
	void populateSubGraphs(NodeEditor::Graph* graph, MR::NodeDef* ownerNodeDef);
	void processNodeConnectionsInBlendTree(NodeEditor::BlendTree* blendTree, std::vector<MR::NodeDef*> childNodes);
	void processNodeTransitionsInStateMachine(NodeEditor::StateMachine* stateMachine, MR::NodeDef* nodeDef);
};
