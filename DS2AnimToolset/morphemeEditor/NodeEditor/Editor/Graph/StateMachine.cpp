#include "StateMachine.h"

#include "NodeEditor/imnodes/imnodes_internal.h"
#include "NodeEditor/NodeEditor.h"

namespace NodeEditor
{
	StateMachine::StateMachine(Editor* editor, Graph* parent, const std::string& name, int graphNodeID) : Graph(editor, parent, name, graphNodeID), m_defaultNodeID(-1)
	{
	}

	StateMachine::~StateMachine()
	{
		for (Transition* transition : m_transitions)
			delete transition;
	}

	void StateMachine::draw()
	{
		ImNodesStyle style = ImNodes::GetStyle();

		ImNodes::PushColorStyle(ImNodesCol_NodeBackground, style.Colors[ImNodesCol_TitleBar]);
		ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundHovered, style.Colors[ImNodesCol_TitleBarHovered]);
		ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundSelected, style.Colors[ImNodesCol_TitleBarSelected]);

		Graph::draw();

		for (StateNode* stateNode : m_stateNodes)
			stateNode->draw();

		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();

		for (Transition* transition : m_transitions)
			transition->draw();

		drawDefaultStateMarker();
	}

	void StateMachine::updateNodePositions()
	{
		Graph::updateNodePositions();

		for (StateNode* stateNode : m_stateNodes)
		{
			ImVec2 stateNodePos = ImNodes::GetNodeGridSpacePos(stateNode->getID());
			stateNode->setPosition(stateNodePos.x, stateNodePos.y);
		}
	}

	StateNode* StateMachine::getStateNode(const std::string& name) const
	{
		for (StateNode* stateNode : m_stateNodes)
		{
			if (stateNode->getName() == name)
				return stateNode;
		}

		return nullptr;
	}

	StateNode* StateMachine::getStateNodeAt(size_t index) const
	{
		if (index >= m_stateNodes.size())
			return nullptr;

		return m_stateNodes[index];
	}

	StateNode* StateMachine::getDefaultStateNode() const
	{
		if (m_stateNodes.size())
			return m_stateNodes[0];

		return nullptr;
	}

	StateNode* StateMachine::createStateNode(const std::string& name)
	{
		std::string nodeName = makeNameValid(name, "ActiveState");

		Manifest::MMStateMachineNode* manifestNode = m_ownerEditor->getManifest()->findStateMachineNodeManifest("ActiveState");

		StateNode* stateNode = manifestNode->makeNode(m_ownerEditor, this, -1, nodeName);
		m_stateNodes.push_back(stateNode);

		return stateNode;
	}

	Transition* StateMachine::getTransition(int nodeID) const
	{
		for (Transition* transition : m_transitions)
		{
			if (transition->getID() == nodeID)
				return transition;
		}

		return nullptr;
	}

	Transition* StateMachine::getTransition(const std::string& name) const
	{
		for (Transition* transition : m_transitions)
		{
			if (transition->getName() == name)
				return transition;
		}

		return nullptr;
	}

	Transition* StateMachine::createTransition(int nodeID, const std::string& typeName, Node* sourceNode, Node* destinationNode)
	{
		Manifest::MMTransition* manifestTransition = m_ownerEditor->getManifest()->findTransitionManifest(typeName);
		
		if (manifestTransition == nullptr)
			throw std::runtime_error("StateMachine::createTransition: Transition type '" + typeName + "' not found in manifest.");

		Transition* transit = manifestTransition->makeTransition(m_ownerEditor, nodeID, sourceNode, destinationNode);

		m_transitions.push_back(transit);

		return transit;
	}

	void StateMachine::drawDefaultStateMarker()
	{
		Node* defaultNode = getNode(m_defaultNodeID);

		ImRect rect = ImNodes::GetNodeGridSpaceRect(defaultNode->getID());

		ImVec2 arrowPoint = rect.Min - ImVec2(3.f, 0);
		ImVec2 arrowStart = ImVec2(arrowPoint.x - 40.f, arrowPoint.y);

		ImDrawList* drawList = ImNodes::GetCurrentContext()->CanvasDrawList;
		drawList->AddLine(arrowStart, arrowPoint, IM_COL32(255, 255, 255, 255), 2.0f);
		drawList->AddTriangleFilled(
			ImVec2(arrowPoint.x - 10.f, arrowPoint.y - 5.f),
			ImVec2(arrowPoint.x - 10.f, arrowPoint.y + 5.f),
			ImVec2(arrowPoint.x, arrowPoint.y),
			IM_COL32(255, 255, 255, 255)
		);

		drawList->AddCircleFilled(arrowStart, 5.f, IM_COL32(255, 255, 255, 255));
	}
}
