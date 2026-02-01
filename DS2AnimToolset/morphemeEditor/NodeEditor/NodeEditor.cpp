#include "NodeEditor.h"
#include "Editor/Graph/BlendTree.h"
#include "Editor/Graph/StateMachine.h"

#include "imnodes/imnodes.h"

#include "IconsFontAwesome6.h"
#include "imgui_custom/imgui_custom_widget.h"

#include "extern.h"
#include "RLog/RLog.h"

namespace NodeEditor
{
    StyleSettings::StyleSettings() : NodeMinWidth(150.f), NodeMinContentHeight(30.f), StateNodeCornerRounding(2.f), StateNodeOutlineThickness(2.f), Colors()
    {
    }

	NodeEditor::NodeEditor() : m_showStyleEditor(false), m_registry(nullptr), m_controlParametersNode(nullptr)
	{
	}

	NodeEditor::~NodeEditor()
	{
		shutdown();
	}

	bool NodeEditor::initialise()
	{
		if (!ImNodes::CreateContext())
			return false;

		m_registry = new Registry(this);
        m_controlParametersNode = new ControlParametersNode(this, "ControlParameters");
        m_controlParametersNode->setPosition(100.f, 400.f);

		initStyle();

		return true;
	}

	void NodeEditor::shutdown()
	{
        for (size_t i = 0; i < m_controlParameters.size(); i++)
            delete m_controlParameters[i];

        m_controlParameters.clear();

		delete m_controlParametersNode;
		delete m_registry;
		ImNodes::DestroyContext();
	}

	void NodeEditor::update(float dt)
	{
        Graph* currentGraph = getCurrentGraph();

        if (currentGraph)
        {
            if (currentGraph->isOfType<BlendTree>())
            {
                Node* cpNode = m_controlParametersNode;
                ImVec2 cpNodePos = ImNodes::GetNodeGridSpacePos(cpNode->getID());
                cpNode->setPosition(cpNodePos.x, cpNodePos.y);

				Node* outputNode = currentGraph->asType<BlendTree>()->getOutputNode();
                ImVec2 outputNodePos = ImNodes::GetNodeGridSpacePos(outputNode->getID());
				outputNode->setPosition(outputNodePos.x, outputNodePos.y);
            }

            for (Node* node : currentGraph->getNodes())
            {
                ImVec2 nodePos = ImNodes::GetNodeGridSpacePos(node->getID());
                node->setPosition(nodePos.x, nodePos.y);
            }
        }
	}

    void NodeEditor::reset()
    {
        for (size_t i = 0; i < m_controlParameters.size(); i++)
			delete m_controlParameters[i];

		m_controlParameters.clear();

        while (!m_graphStack.empty())
            m_graphStack.pop();

		delete m_controlParametersNode;
		m_controlParametersNode = new ControlParametersNode(this, "ControlParameters");
	}

	void NodeEditor::draw()
	{
		ImGui::Begin("Node Editor");

		std::string parentGraphList = "";
		std::stack<Graph*> tempStack = m_graphStack;

		while (!tempStack.empty())
		{
			Graph* graph = tempStack.top();
			tempStack.pop();

			if (!parentGraphList.empty())
				parentGraphList = graph->getName() + "|" + parentGraphList;
			else
				parentGraphList = graph->getName();
		}
		
		if (ImGui::Button(ICON_FA_ARROW_UP))
			popGraph();

		ImGui::SameLine();

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::Label(parentGraphList.c_str());

		ImNodes::BeginNodeEditor();

		Graph* currentGraph = getCurrentGraph();

        if (currentGraph)
        {
            currentGraph->draw();

            if (currentGraph->isOfType<BlendTree>())
				m_controlParametersNode->draw();
        }

		ImNodes::EndNodeEditor();

		ImGui::End();
	}

    void NodeEditor::handleUserInput()
    {
        if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows | ImGuiFocusedFlags_DockHierarchy))
            return;

        int hoveredNodeId = -1;

        if (ImNodes::IsNodeHovered(&hoveredNodeId))
        {
            Node* hoveredNode = dynamic_cast<Node*>(m_registry->findEntity(hoveredNodeId));

            if (hoveredNode && hoveredNode->hasSubGraph() && ImGui::IsMouseDoubleClicked(0))
                pushGraph(hoveredNode->getSubGraph());
        }
        else if (ImGui::IsMouseDoubleClicked(0))
        {
            popGraph();
        }
    }

	ControlParameter* NodeEditor::createControlParameter(int id, const std::string& name, ControlParameter::ParameterType parameterType)
    {
        if (hasControlParameter(name))
        {
            g_appLog->debugMessage(MsgLevel_Warn, "NodeEditorBase::createControlParameter: Control parameter with name '%s' already exists.\n", name.c_str());
            return nullptr;
        }

        ControlParameter* parameter = new ControlParameter(this, id, name, parameterType);
		addControlParameter(parameter);

        return parameter;
	}

    void NodeEditor::addControlParameter(ControlParameter* parameter)
    {
        m_controlParameters.push_back(parameter);
        m_controlParametersNode->addOutputPin(parameter->getOutputPin());
    }

    ControlParameter* NodeEditor::createControlParameterFloat(int id, const std::string& name)
    {
        return createControlParameter(id, name, ControlParameter::kParameterTypeFloat);
    }

    ControlParameter* NodeEditor::createControlParameterInt(int id, const std::string& name)
    {
        return createControlParameter(id, name, ControlParameter::kParameterTypeInt);
	}

    ControlParameter* NodeEditor::createControlParameterUInt(int id, const std::string& name)
    {
        return createControlParameter(id, name, ControlParameter::kParameterTypeUInt);
    }

    ControlParameter* NodeEditor::createControlParameterBool(int id, const std::string& name)
    {
        return createControlParameter(id, name, ControlParameter::kParameterTypeBool);
	}

    ControlParameter* NodeEditor::createControlParameterVector3(int id, const std::string& name)
    {
        return createControlParameter(id, name, ControlParameter::kParameterTypeVector3);
    }

    ControlParameter* NodeEditor::createControlParameterVector4(int id, const std::string& name)
    {
        return createControlParameter(id, name, ControlParameter::kParameterTypeVector4);
    }

    ControlParameter* NodeEditor::createControlParameterQuaternion(int id, const std::string& name)
    {
        return createControlParameter(id, name, ControlParameter::kParameterTypeQuaternion);
    }

    Node* NodeEditor::createNode(const std::string& parentPath, const std::string& typeName, int id, const std::string& name)
    {
        BlendTree* parentGraph = findGraphByPath(parentPath)->asType<BlendTree>();

        if (!parentGraph)
        {
            g_appLog->debugMessage(MsgLevel_Warn, "NodeEditorBase::createNode: Parent graph with path '%s' not found or is not a BlendTree\n", parentPath.c_str());
            return nullptr;
        }

        if (!isNodeIDAvailable(id))
        {
            g_appLog->debugMessage(MsgLevel_Warn, "NodeEditorBase::createNode: Node ID '%d' is already in use.\n", id);
            return nullptr;
        }

        Node* newNode = parentGraph->createNode(id, typeName, name);

		return newNode;
    }

    Node* NodeEditor::createBlendTree(const std::string& parentPath, int id, const std::string& name)
    {
        Graph* parentGraph = findGraphByPath(parentPath);

        if (!parentGraph)
        {
            g_appLog->debugMessage(MsgLevel_Warn, "NodeEditorBase::createBlendTree: Parent graph with path '%s' not found or is not a BlendTree\n", parentPath.c_str());
            return nullptr;
        }
        if (!isNodeIDAvailable(id))
        {
            g_appLog->debugMessage(MsgLevel_Warn, "NodeEditorBase::createBlendTree: Node ID '%d' is already in use.\n", id);
            return nullptr;
        }

        Node* newNode = parentGraph->createBlendTree(id, name);

        return newNode;
	}

    Node* NodeEditor::createStateMachine(const std::string& parentPath, int id, const std::string& name)
    {
        Graph* parentGraph = findGraphByPath(parentPath);

        if (!parentGraph)
        {
            g_appLog->debugMessage(MsgLevel_Warn, "NodeEditorBase::createStateMachine: Parent graph with path '%s' not found or is not a BlendTree\n", parentPath.c_str());
            return nullptr;
        }

        if (!isNodeIDAvailable(id))
        {
            g_appLog->debugMessage(MsgLevel_Warn, "NodeEditorBase::createStateMachine: Node ID '%d' is already in use.\n", id);
            return nullptr;
        }

        Node* newNode = parentGraph->createStateMachine(id, name);

        return newNode;
    }

    ControlParameter* NodeEditor::getControlParameter(const std::string& name) const
    {
        for (ControlParameter* parameter : m_controlParameters)
        {
            if (parameter->getName() == name)
                return parameter;
        }

        return nullptr;
	}

    bool NodeEditor::removeControlParameter(ControlParameter* parameter)
    {
        auto it = std::find(m_controlParameters.begin(), m_controlParameters.end(), parameter);
        if (it != m_controlParameters.end())
        {
            m_controlParameters.erase(it);
            delete parameter;
            return true;
        }

        return false;
	}

    bool NodeEditor::hasControlParameter(const std::string& name) const
    {
        for (ControlParameter* parameter : m_controlParameters)
        {
            if (parameter->getName() == name)
                return true;
        }

        return false;
	}

    void NodeEditor::getAllNodes(std::vector<Node*>& outNodes) const
    {
        for (size_t i = 0; i < m_registry->getNumRegisteredEntities(); i++)
        {
            Entity* entity = m_registry->getEntityAtIndex(i);

            if (entity->isOfType<Node>())
            {
                Node* node = dynamic_cast<Node*>(entity);
                outNodes.push_back(node);
            }
		}
    }

    Node* NodeEditor::getNode(int nodeID) const
    {
		std::vector<Node*> allNodes;
		getAllNodes(allNodes);

        for (Node* node : allNodes)
        {
            if (node->getNodeID() == nodeID)
				return node;
        }

        return nullptr;
	}

    Node* NodeEditor::getNode(const std::string& name) const
	{
		std::vector<Node*> allNodes;
		getAllNodes(allNodes);

        for (Node* node : allNodes)
        {
			if (node->getName() == name)
                return node;
        }

		return nullptr;
	}

    Node* NodeEditor::findNodeByPath(const std::string& path) const
    {
        std::vector<Node*> allNodes;
        getAllNodes(allNodes);

        for (Node* node : allNodes)
        {
            if (node->getFullName() == path)
                return node;
        }

        return nullptr;
    }

    Graph* NodeEditor::getGraph(int graphID) const
    {
        for (size_t i = 0; i < m_registry->getNumRegisteredGraphs(); i++)
        {
            Graph* graph = m_registry->getGraphAtIndex(i);
            if (graph->getID() == graphID)
                return graph;
        }

        return nullptr;
    }

    Graph* NodeEditor::getGraph(const std::string& name) const
    {
        for (size_t i = 0; i < m_registry->getNumRegisteredGraphs(); i++)
        {
            Graph* graph = m_registry->getGraphAtIndex(i);
            if (graph->getName() == name)
                return graph;
        }

        return nullptr;
	}

    Graph* NodeEditor::findGraphByPath(const std::string& path) const
    {
        for (size_t i = 0; i < m_registry->getNumRegisteredGraphs(); i++)
        {
            Graph* graph = m_registry->getGraphAtIndex(i);
            if (graph->getFullName() == path)
                return graph;
        }

        return nullptr;
    }

    Transition* NodeEditor::getTransitionBetweenNodes(Node* sourceNode, Node* destinationNode) const
    {
        for (size_t i = 0; i < m_registry->getNumRegisteredEntities(); i++)
        {
            Entity* entity = m_registry->getEntityAtIndex(i);

            if (entity->isOfType<Transition>())
            {
                Transition* transition = dynamic_cast<Transition*>(entity);

                if (transition->getSourceNode() == sourceNode && transition->getDestinationNode() == destinationNode)
                    return transition;
            }
        }

        return nullptr;
	}

    Node* NodeEditor::getSelectedNode() const
    {
		int selectedNodeID = -1;

        if (ImNodes::NumSelectedNodes() != 1)
            return nullptr;

        ImNodes::GetSelectedNodes(&selectedNodeID);

        return dynamic_cast<Node*>(m_registry->findEntity(selectedNodeID));
    }

    Transition* NodeEditor::getSelectedTransition() const
    {
        int selectedNodeID = -1;

        if (ImNodes::NumSelectedTransitions() != 1)
            return nullptr;

        ImNodes::GetSelectedTransition(&selectedNodeID);

        return dynamic_cast<Transition*>(m_registry->findEntity(selectedNodeID));
    }

    void NodeEditor::clearSelection()
    {
        ImNodes::ClearLinkSelection();
        ImNodes::ClearNodeSelection();
		ImNodes::ClearTransitionSelection();
    }

    BlendTree* NodeEditor::createRootBlendTree()
    {
        if (m_rootGraph)
            return nullptr;

		BlendTree* blendTree = new BlendTree(this, nullptr, "");

        pushGraph(blendTree);

		return blendTree;
	}

    StateMachine* NodeEditor::createRootStateMachine()
    {
        if (m_rootGraph)
            return nullptr;

        StateMachine* stateMachine = new StateMachine(this, nullptr, "");

		pushGraph(stateMachine);

        return stateMachine;
	}

	void NodeEditor::pushGraph(Graph* graph)
    {
        if (m_graphStack.empty())
			m_rootGraph = graph;

        m_graphStack.push(graph);

		clearSelection();
	}

	void NodeEditor::popGraph()
	{
        if (m_graphStack.size() > 1)
            m_graphStack.pop();

		clearSelection();
	}

    void NodeEditor::initStyle()
    {
        ImNodes::StyleColorsDark();

        ImNodesStyle& style = ImNodes::GetStyle();
        style.Flags = ImNodesStyleFlags_None;
		style.Flags |= ImNodesStyleFlags_NodeOutline;

		style.GridSpacing = 20.f;

        style.NodePadding.x = 13.0f;
        style.NodePadding.y = 5.0f;
        style.NodeBorderThickness = 0.f;
        style.NodeCornerRounding = 2.f;
		style.TransitionSpacingOffset = 10.f;
        style.TransitionNodeOffset = 1.5f;
        style.PinOffset = -5.5f;

        style.LinkLineSegmentsPerLength = 1.f;
        style.LinkThickness = 1.f;

        style.PinTriangleSideLength = 8.f;

        style.Colors[ImNodesCol_GridBackground] = IM_COL32(19, 18, 17, 255);
		style.Colors[ImNodesCol_BoxSelector] = IM_COL32(113, 99, 96, 20);
        style.Colors[ImNodesCol_BoxSelectorOutline] = IM_COL32(113, 99, 96, 100);

        style.Colors[ImNodesCol_NodeBackground] = IM_COL32(4, 86, 187, 255);
        style.Colors[ImNodesCol_NodeBackgroundHovered] = IM_COL32(4, 86, 187, 255);
        style.Colors[ImNodesCol_NodeBackgroundSelected] = IM_COL32(179, 120, 0, 255);
        style.Colors[ImNodesCol_TitleBar] = IM_COL32(3, 51, 109, 255);
        style.Colors[ImNodesCol_TitleBarHovered] = IM_COL32(3, 51, 109, 255);
		style.Colors[ImNodesCol_TitleBarSelected] = IM_COL32(215, 167, 0, 255);
		style.Colors[ImNodesCol_NodeOutline] = IM_COL32(3, 57, 124, 255);
        style.Colors[ImNodesCol_NodeOutlineHovered] = IM_COL32(3, 57, 124, 255);
        style.Colors[ImNodesCol_NodeOutlineSelected] = IM_COL32(179, 120, 0, 255);

		style.Colors[ImNodesCol_Pin] = IM_COL32(200, 200, 200, 255);
		style.Colors[ImNodesCol_PinHovered] = IM_COL32(255, 255, 255, 255);

        style.Colors[ImNodesCol_Link] = IM_COL32(148, 140, 137, 255);
        style.Colors[ImNodesCol_LinkHovered] = IM_COL32(148, 140, 137, 255);
        style.Colors[ImNodesCol_LinkSelected] = IM_COL32(148, 140, 137, 255);

		style.Colors[ImNodesCol_Transition] = IM_COL32(200, 200, 200, 255);
		style.Colors[ImNodesCol_TransitionHovered] = IM_COL32(200, 200, 200, 255);
		style.Colors[ImNodesCol_TransitionSelected] = IM_COL32(255, 255, 255, 255);

        m_styleSettings.NodeMinWidth = 150.0f;
        m_styleSettings.NodeMinContentHeight = 15.0f;
		m_styleSettings.StateNodeCornerRounding = 4.0f;
		m_styleSettings.StateNodeOutlineThickness = 2.0f;

		m_styleSettings.Colors[NodeEditorStyleCol_StateNodeBackground] = IM_COL32(3, 51, 109, 255);
		m_styleSettings.Colors[NodeEditorStyleCol_StateNodeBackgroundHovered] = IM_COL32(3, 51, 109, 255);
		m_styleSettings.Colors[NodeEditorStyleCol_StateNodeBackgroundSelected] = style.Colors[ImNodesCol_TitleBarSelected];
		m_styleSettings.Colors[NodeEditorStyleCol_StateNodeTitleBar] = IM_COL32(3, 51, 109, 255);
		m_styleSettings.Colors[NodeEditorStyleCol_StateNodeTitleBarHovered] = IM_COL32(3, 51, 109, 255);
		m_styleSettings.Colors[NodeEditorStyleCol_StateNodeTitleBarSelected] = style.Colors[ImNodesCol_TitleBarSelected];
		m_styleSettings.Colors[NodeEditorStyleCol_StateNodeOutline] = IM_COL32(255, 255, 255, 255);
		m_styleSettings.Colors[NodeEditorStyleCol_StateNodeOutlineHovered] = IM_COL32(255, 255, 255, 255);
		m_styleSettings.Colors[NodeEditorStyleCol_StateNodeOutlineSelected] = style.Colors[ImNodesCol_NodeOutlineSelected];

        m_styleSettings.Colors[NodeEditorStyleCol_ControlParamNodeBackground] = IM_COL32(103, 96, 94, 255);
        m_styleSettings.Colors[NodeEditorStyleCol_ControlParamNodeBackgroundHovered] = IM_COL32(103, 96, 94, 255);
        m_styleSettings.Colors[NodeEditorStyleCol_ControlParamNodeBackgroundSelected] = style.Colors[ImNodesCol_NodeBackgroundSelected];
        m_styleSettings.Colors[NodeEditorStyleCol_ControlParamNodeTitleBar] = IM_COL32(67, 65, 63, 255);
        m_styleSettings.Colors[NodeEditorStyleCol_ControlParamNodeTitleBarHovered] = IM_COL32(67, 65, 63, 255);
        m_styleSettings.Colors[NodeEditorStyleCol_ControlParamNodeTitleBarSelected] = style.Colors[ImNodesCol_TitleBarSelected];
        m_styleSettings.Colors[NodeEditorStyleCol_ControlParamNodeOutline] = IM_COL32(54, 49, 47, 255);
        m_styleSettings.Colors[NodeEditorStyleCol_ControlParamNodeOutlineHovered] = IM_COL32(54, 49, 47, 255);
        m_styleSettings.Colors[NodeEditorStyleCol_ControlParamNodeOutlineSelected] = style.Colors[ImNodesCol_NodeOutlineSelected];

		m_styleSettings.Colors[NodeEditorStyleCol_FloatDataPin] = IM_COL32(142, 142, 142, 255);
		m_styleSettings.Colors[NodeEditorStyleCol_IntDataPin] = IM_COL32(142, 232, 232, 255);
		m_styleSettings.Colors[NodeEditorStyleCol_UIntDataPin] = IM_COL32(239, 142, 239, 255);
		m_styleSettings.Colors[NodeEditorStyleCol_BoolDataPin] = IM_COL32(142, 142, 234, 255);
		m_styleSettings.Colors[NodeEditorStyleCol_Vector3DataPin] = IM_COL32(237, 143, 143, 255);
		m_styleSettings.Colors[NodeEditorStyleCol_Vector4DataPin] = IM_COL32(142, 234, 142, 255);
		m_styleSettings.Colors[NodeEditorStyleCol_QuaternionDataPin] = IM_COL32(240, 240, 142, 255);
	}

    bool NodeEditor::isNodeIDAvailable(int nodeID) const
    {
        for (size_t i = 0; i < m_controlParameters.size(); i++)
        {
            if (m_controlParameters[i]->getControlParameterID() == nodeID)
                return false;
        }

        return (getNode(nodeID) == nullptr);
	}

    void NodeEditor::styleEditor()
    {
        ImNodesStyle& style = ImNodes::GetStyle();
		StyleSettings& customStyle = m_styleSettings;

        bool nodeOutline = (style.Flags & ImNodesStyleFlags_NodeOutline) != 0;
        bool gridLines = (style.Flags & ImNodesStyleFlags_GridLines) != 0;

        if (ImGui::Checkbox("Node Outline", &nodeOutline))
        {
            if (nodeOutline) style.Flags |= ImNodesStyleFlags_NodeOutline;
            else             style.Flags &= ~ImNodesStyleFlags_NodeOutline;
        }

        ImGui::SameLine();

        if (ImGui::Checkbox("Grid Lines", &gridLines))
        {
            if (gridLines) style.Flags |= ImNodesStyleFlags_GridLines;
            else           style.Flags &= ~ImNodesStyleFlags_GridLines;
        }

		ImGui::SeparatorText("Box Selector Colors");

        ImGui::ColorEditUInt("Box Selector", &style.Colors[ImNodesCol_BoxSelector]);
        ImGui::ColorEditUInt("Box Selector Outline", &style.Colors[ImNodesCol_BoxSelectorOutline]);

        if (!ImGui::BeginTabBar("##ImNodesComponentList"))
            return;

        // -------------------- Grid --------------------
        if (ImGui::BeginTabItem("Grid"))
        {
            ImGui::SeparatorText("Sizes");

            ImGui::SliderFloat("Grid Spacing", &style.GridSpacing, 8.0f, 128.0f);

            ImGui::SeparatorText("Colors");

            ImGui::ColorEditUInt("Grid Background", &style.Colors[ImNodesCol_GridBackground]);
            ImGui::ColorEditUInt("Grid Line", &style.Colors[ImNodesCol_GridLine]);

            ImGui::EndTabItem();
        }

        // -------------------- Nodes --------------------
        if (ImGui::BeginTabItem("Nodes"))
        {
            ImGui::SeparatorText("Sizes");

            ImGui::SliderFloat("Corner Rounding", &style.NodeCornerRounding, 0.0f, 12.0f);
            ImGui::SliderFloat2("Padding", &style.NodePadding.x, 0.0f, 32.0f);
            ImGui::SliderFloat("Border Thickness", &style.NodeBorderThickness, 0.0f, 4.0f);

			ImGui::SliderFloat("Pin Offset", &style.PinOffset, -16.0f, 16.0f);
			ImGui::SliderFloat("Min Width", &customStyle.NodeMinWidth, 0.f, 300.0f);
			ImGui::SliderFloat("Min Content Height", &customStyle.NodeMinContentHeight, 0.f, 100.0f);

			ImGui::SliderFloat("State Node Corner Rounding", &customStyle.StateNodeCornerRounding, 0.0f, 12.0f);
			ImGui::SliderFloat("State Node Outline Thickness", &customStyle.StateNodeOutlineThickness, 0.0f, 8.0f);

            ImGui::SeparatorText("Colors");

            ImGui::ColorEditUInt("Node Background", &style.Colors[ImNodesCol_NodeBackground]);
            ImGui::ColorEditUInt("Node Background Hovered", &style.Colors[ImNodesCol_NodeBackgroundHovered]);
            ImGui::ColorEditUInt("Node Background Selected", &style.Colors[ImNodesCol_NodeBackgroundSelected]);
            ImGui::ColorEditUInt("Node Outline", &style.Colors[ImNodesCol_NodeOutline]);
			ImGui::ColorEditUInt("Node Outline Hovered", &style.Colors[ImNodesCol_NodeOutlineHovered]);
			ImGui::ColorEditUInt("Node Outline Selected", &style.Colors[ImNodesCol_NodeOutlineSelected]);
            ImGui::ColorEditUInt("Title Bar", &style.Colors[ImNodesCol_TitleBar]);
            ImGui::ColorEditUInt("Title Bar Hovered", &style.Colors[ImNodesCol_TitleBarHovered]);
            ImGui::ColorEditUInt("Title Bar Selected", &style.Colors[ImNodesCol_TitleBarSelected]);

			ImGui::ColorEditUInt("Control Param Node Background", &customStyle.Colors[NodeEditorStyleCol_ControlParamNodeBackground]);
			ImGui::ColorEditUInt("Control Param Node Background Hovered", &customStyle.Colors[NodeEditorStyleCol_ControlParamNodeBackgroundHovered]);
			ImGui::ColorEditUInt("Control Param Node Background Selected", &customStyle.Colors[NodeEditorStyleCol_ControlParamNodeBackgroundSelected]);
			ImGui::ColorEditUInt("Control Param Node Title Bar", &customStyle.Colors[NodeEditorStyleCol_ControlParamNodeTitleBar]);
			ImGui::ColorEditUInt("Control Param Node Title Bar Hovered", &customStyle.Colors[NodeEditorStyleCol_ControlParamNodeTitleBarHovered]);
			ImGui::ColorEditUInt("Control Param Node Title Bar Selected", &customStyle.Colors[NodeEditorStyleCol_ControlParamNodeTitleBarSelected]);
			ImGui::ColorEditUInt("Control Param Node Outline", &customStyle.Colors[NodeEditorStyleCol_ControlParamNodeOutline]);
			ImGui::ColorEditUInt("Control Param Node Outline Hovered", &customStyle.Colors[NodeEditorStyleCol_ControlParamNodeOutlineHovered]);
			ImGui::ColorEditUInt("Control Param Node Outline Selected", &customStyle.Colors[NodeEditorStyleCol_ControlParamNodeOutlineSelected]);

			ImGui::ColorEditUInt("State Node Background", &customStyle.Colors[NodeEditorStyleCol_StateNodeBackground]);
			ImGui::ColorEditUInt("State Node Background Hovered", &customStyle.Colors[NodeEditorStyleCol_StateNodeBackgroundHovered]);
			ImGui::ColorEditUInt("State Node Background Selected", &customStyle.Colors[NodeEditorStyleCol_StateNodeBackgroundSelected]);
			ImGui::ColorEditUInt("State Node Title Bar", &customStyle.Colors[NodeEditorStyleCol_StateNodeTitleBar]);
			ImGui::ColorEditUInt("State Node Title Bar Hovered", &customStyle.Colors[NodeEditorStyleCol_StateNodeTitleBarHovered]);
			ImGui::ColorEditUInt("State Node Title Bar Selected", &customStyle.Colors[NodeEditorStyleCol_StateNodeTitleBarSelected]);
			ImGui::ColorEditUInt("State Node Outline", &customStyle.Colors[NodeEditorStyleCol_StateNodeOutline]);
			ImGui::ColorEditUInt("State Node Outline Hovered", &customStyle.Colors[NodeEditorStyleCol_StateNodeOutlineHovered]);
			ImGui::ColorEditUInt("State Node Outline Selected", &customStyle.Colors[NodeEditorStyleCol_StateNodeOutlineSelected]);

            ImGui::EndTabItem();
        }

        // -------------------- Links --------------------
        if (ImGui::BeginTabItem("Links"))
        {
            ImGui::SeparatorText("Sizes");

            ImGui::SliderFloat("Thickness", &style.LinkThickness, 1.0f, 8.0f);
            ImGui::SliderFloat("Segments / Length", &style.LinkLineSegmentsPerLength, 0.1f, 1.0f);
            ImGui::SliderFloat("Hover Distance", &style.LinkHoverDistance, 1.0f, 20.0f);

            ImGui::SeparatorText("Colors");

            ImGui::ColorEditUInt("Link", &style.Colors[ImNodesCol_Link]);
            ImGui::ColorEditUInt("Link Hovered", &style.Colors[ImNodesCol_LinkHovered]);
            ImGui::ColorEditUInt("Link Selected", &style.Colors[ImNodesCol_LinkSelected]);

            ImGui::EndTabItem();
        }

        // -------------------- Pins --------------------
        if (ImGui::BeginTabItem("Pins"))
        {
            ImGui::SeparatorText("Sizes");

            ImGui::SliderFloat("Circle Radius", &style.PinCircleRadius, 2.0f, 12.0f);
            ImGui::SliderFloat("Quad Side Length", &style.PinQuadSideLength, 4.0f, 20.0f);
            ImGui::SliderFloat("Triangle Side Length", &style.PinTriangleSideLength, 4.0f, 20.0f);
            ImGui::SliderFloat("Line Thickness", &style.PinLineThickness, 1.0f, 4.0f);
            ImGui::SliderFloat("Hover Radius", &style.PinHoverRadius, 4.0f, 20.0f);

            ImGui::SeparatorText("Colors");

            ImGui::ColorEditUInt("Pin", &style.Colors[ImNodesCol_Pin]);
            ImGui::ColorEditUInt("Pin Hovered", &style.Colors[ImNodesCol_PinHovered]);

			ImGui::ColorEditUInt("Float Data Pin", &customStyle.Colors[NodeEditorStyleCol_FloatDataPin]);
			ImGui::ColorEditUInt("Int Data Pin", &customStyle.Colors[NodeEditorStyleCol_IntDataPin]);
			ImGui::ColorEditUInt("UInt Data Pin", &customStyle.Colors[NodeEditorStyleCol_UIntDataPin]);
			ImGui::ColorEditUInt("Bool Data Pin", &customStyle.Colors[NodeEditorStyleCol_BoolDataPin]);
			ImGui::ColorEditUInt("Vector3 Data Pin", &customStyle.Colors[NodeEditorStyleCol_Vector3DataPin]);
			ImGui::ColorEditUInt("Vector4 Data Pin", &customStyle.Colors[NodeEditorStyleCol_Vector4DataPin]);
			ImGui::ColorEditUInt("Quaternion Data Pin", &customStyle.Colors[NodeEditorStyleCol_QuaternionDataPin]);

            ImGui::EndTabItem();
        }

		// ------------------- Transition --------------------
        if (ImGui::BeginTabItem("Transition"))
        {
            ImGui::SeparatorText("Sizes");

			ImGui::SliderFloat("Spacing Offset", &style.TransitionSpacingOffset, 0.0f, 32.0f);
            ImGui::SliderFloat("Node Offset", &style.TransitionNodeOffset, 0.0f, 32.0f);
			ImGui::SliderFloat("Hover Distance", &style.TransitionHoverDistance, 1.0f, 20.0f);
			ImGui::SliderFloat("Thickness", &style.TransitionThickness, 1.0f, 8.0f);
			ImGui::SliderFloat("Arrow Size", &style.TransitionArrowSize, 4.0f, 20.0f);

            ImGui::SeparatorText("Colors");

            ImGui::ColorEditUInt("Transition", &style.Colors[ImNodesCol_Transition]);
            ImGui::ColorEditUInt("Transition Hovered", &style.Colors[ImNodesCol_TransitionHovered]);
            ImGui::ColorEditUInt("Transition Selected", &style.Colors[ImNodesCol_TransitionSelected]);

			ImGui::EndTabItem();
        }

        // -------------------- Mini-map --------------------
        if (ImGui::BeginTabItem("Mini-map"))
        {
            ImGui::SeparatorText("Sizes");

            ImGui::SliderFloat2("Padding", &style.MiniMapPadding.x, 0.0f, 32.0f);
            ImGui::SliderFloat2("Offset", &style.MiniMapOffset.x, -200.0f, 200.0f);

            ImGui::SeparatorText("Colors");

            ImGui::ColorEditUInt("MiniMap Background", &style.Colors[ImNodesCol_MiniMapBackground]);
            ImGui::ColorEditUInt("MiniMap Background Hovered", &style.Colors[ImNodesCol_MiniMapBackgroundHovered]);
            ImGui::ColorEditUInt("MiniMap Outline", &style.Colors[ImNodesCol_MiniMapOutline]);
            ImGui::ColorEditUInt("MiniMap Outline Hovered", &style.Colors[ImNodesCol_MiniMapOutlineHovered]);
            ImGui::ColorEditUInt("MiniMap Node Background", &style.Colors[ImNodesCol_MiniMapNodeBackground]);
            ImGui::ColorEditUInt("MiniMap Node Background Hovered", &style.Colors[ImNodesCol_MiniMapNodeBackgroundHovered]);
            ImGui::ColorEditUInt("MiniMap Node Background Selected", &style.Colors[ImNodesCol_MiniMapNodeBackgroundSelected]);
            ImGui::ColorEditUInt("MiniMap Node Outline", &style.Colors[ImNodesCol_MiniMapNodeOutline]);
            ImGui::ColorEditUInt("MiniMap Link", &style.Colors[ImNodesCol_MiniMapLink]);
            ImGui::ColorEditUInt("MiniMap Link Selected", &style.Colors[ImNodesCol_MiniMapLinkSelected]);
            ImGui::ColorEditUInt("MiniMap Canvas", &style.Colors[ImNodesCol_MiniMapCanvas]);
            ImGui::ColorEditUInt("MiniMap Canvas Outline", &style.Colors[ImNodesCol_MiniMapCanvasOutline]);

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}