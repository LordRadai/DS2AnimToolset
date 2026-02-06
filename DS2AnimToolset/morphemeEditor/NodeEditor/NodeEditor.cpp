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

	Editor::Editor(int flags) : m_flags(flags), m_showStyleEditor(false), m_registry(nullptr), m_manifest(nullptr), m_rootGraph(nullptr)
	{
	}

	Editor::~Editor()
	{
		shutdown();
	}

	bool Editor::initialise()
	{
		g_appLog->debugMessage(MsgLevel_Info, "Initialising Node Editor\n");

		if (!ImNodes::CreateContext())
			return false;

		m_registry = new Registry(this);
		m_manifest = new Manifest::Manifest();

        if (!m_manifest->init())
			g_appLog->alertMessage(MsgLevel_Error, "Failed to initialise Manifest.\n");

		initStyle();

		return true;
	}

	void Editor::shutdown()
	{
        for (size_t i = 0; i < m_controlParameters.size(); i++)
            delete m_controlParameters[i];

        m_controlParameters.clear();
        
        m_manifest->shutdown();

		delete m_registry;
		delete m_manifest;

		ImNodes::DestroyContext();
	}

	void Editor::update(float dt)
	{
	}

    void Editor::reset()
    {
        for (size_t i = 0; i < m_controlParameters.size(); i++)
			delete m_controlParameters[i];

        for (size_t i = 0; i < m_messages.size(); i++)
			delete m_messages[i];

		m_controlParameters.clear();

		m_messages.clear();

        delete m_rootGraph;
		m_rootGraph = nullptr;

        while (!m_graphStack.empty())
            m_graphStack.pop();
	}

	void Editor::draw()
	{
		ImGui::Begin("Node Editor");

        Graph* currentGraph = getCurrentGraph();

        if (currentGraph)
        {
            handleUserInput();

            if (ImGui::Button(ICON_FA_ARROW_UP))
                popGraph();

            ImGui::SameLine();

		    ImGui::Label(currentGraph->getFullName().c_str());

		    ImNodes::BeginNodeEditor();

            currentGraph->draw();

            ImNodes::EndNodeEditor();

            currentGraph->updateNodePositions();
        }

		ImGui::End();
	}

    void Editor::handleUserInput()
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

    void Editor::infoGui()
    {
        Node* selectedNode = getSelectedNode();

        if (!selectedNode)
            selectedNode = getSelectedTransition();

        bool drawnInfo = false;

        if (selectedNode)
            drawnInfo = selectedNode->editorGUI();

        if (!drawnInfo)
        {
            if (ImGui::TreeNodeEx("Control Parameters", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow))
            {
                ImGui::BeginTable("##cpTable", 2, ImGuiTableFlags_BordersOuter);

                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Type");
                ImGui::TableHeadersRow();

                for (ControlParameter* parameter : m_controlParameters)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();

                    ImGui::TextUnformatted(parameter->getName().c_str());
                    ImGui::TableNextColumn();

                    ImGui::TextUnformatted(ControlParameter::parameterTypeToString(parameter->getParameterType()));
                }

                ImGui::EndTable();

				ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("Requests", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow))
            {
                ImGui::BeginTable("##requestTable", 2, ImGuiTableFlags_BordersOuter);

                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Type");
                ImGui::TableHeadersRow();

                for (Message* request : m_messages)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();

                    ImGui::TextUnformatted(request->getName().c_str());

                    ImGui::TableNextColumn();

                    ImGui::TextUnformatted(request->getType().c_str());
                }

                ImGui::EndTable();

				ImGui::TreePop();
            }
        }
    }

    void Editor::navigatorGui()
    {
		Graph* rootGraph = getRootGraph();

        if (rootGraph)
            rootGraph->navigatorGui();
	}

	ControlParameter* Editor::createControlParameter(int id, const std::string& name, ControlParameter::ParameterType parameterType)
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

    void Editor::addControlParameter(ControlParameter* parameter)
    {
        m_controlParameters.push_back(parameter);
    }

    ControlParameter* Editor::createControlParameterFloat(int id, const std::string& name)
    {
        return createControlParameter(id, name, ControlParameter::kParameterTypeFloat);
    }

    ControlParameter* Editor::createControlParameterInt(int id, const std::string& name)
    {
        return createControlParameter(id, name, ControlParameter::kParameterTypeInt);
	}

    ControlParameter* Editor::createControlParameterUInt(int id, const std::string& name)
    {
        return createControlParameter(id, name, ControlParameter::kParameterTypeUInt);
    }

    ControlParameter* Editor::createControlParameterBool(int id, const std::string& name)
    {
        return createControlParameter(id, name, ControlParameter::kParameterTypeBool);
	}

    ControlParameter* Editor::createControlParameterVector3(int id, const std::string& name)
    {
        return createControlParameter(id, name, ControlParameter::kParameterTypeVector3);
    }

    ControlParameter* Editor::createControlParameterVector4(int id, const std::string& name)
    {
        return createControlParameter(id, name, ControlParameter::kParameterTypeVector4);
    }

    ControlParameter* Editor::createControlParameterQuaternion(int id, const std::string& name)
    {
        return createControlParameter(id, name, ControlParameter::kParameterTypeQuaternion);
    }

    ControlParameter* Editor::getControlParameter(int id) const
    {
        for (ControlParameter* parameter : m_controlParameters)
        {
            if (parameter->getControlParameterID() == id)
                return parameter;
        }

        return nullptr;
    }

    ControlParameter* Editor::getControlParameter(const std::string& name) const
    {
        for (ControlParameter* parameter : m_controlParameters)
        {
            if (parameter->getName() == name)
                return parameter;
        }

        return nullptr;
	}

    bool Editor::removeControlParameter(ControlParameter* parameter)
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

    bool Editor::hasControlParameter(const std::string& name) const
    {
        for (ControlParameter* parameter : m_controlParameters)
        {
            if (parameter->getName() == name)
                return true;
        }

        return false;
	}

    Message* Editor::createMessage(int id, const std::string& name, const std::string& type)
    {
        if (hasRequest(id) || hasRequest(name))
        {
            g_appLog->debugMessage(MsgLevel_Warn, "NodeEditorBase::createMessage: Message with ID '%d' or name '%s' already exists.\n", id, name.c_str());
            return nullptr;
        }

        Message* message = new Message(id, name, type);
        addMessage(message);

        return message;
	}

    Message* Editor::createRequest(int id, const std::string& name)
    {
        if (hasRequest(id) || hasRequest(name))
        {
            g_appLog->debugMessage(MsgLevel_Warn, "NodeEditorBase::createRequest: Request with ID '%d' or name '%s' already exists.\n", id, name.c_str());
            return nullptr;
        }

        return createMessage(id, name, "Request");
    }

    Message* Editor::getRequest(int id)
    {
        for (Message* request : m_messages)
        {
            if (request->getRequestID() == id)
                return request;
        }

        return nullptr;
    }

    Message* Editor::getRequest(const std::string& name) const
    {
        for (Message* request : m_messages)
        {
            if (request->getName() == name)
                return request;
        }
        return nullptr;
	}

    bool Editor::removeRequest(Message* request)
    {
        auto it = std::find(m_messages.begin(), m_messages.end(), request);
        if (it != m_messages.end())
        {
            m_messages.erase(it);
            delete request;
            return true;
        }

        return false;
    }

    bool Editor::hasRequest(int id) const
    {
        for (Message* request : m_messages)
        {
            if (request->getRequestID() == id)
                return true;
        }

        return false;
	}

    bool Editor::hasRequest(const std::string& name) const
    {
        for (Message* request : m_messages)
        {
            if (request->getName() == name)
                return true;
        }

        return false;
    }

    void Editor::getAllNodes(std::vector<Node*>& outNodes) const
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

    void Editor::getAllGraphs(std::vector<Graph*>& outGraphs) const
    {
        for (size_t i = 0; i < m_registry->getNumRegisteredGraphs(); i++)
        {
            Graph* graph = m_registry->getGraphAtIndex(i);
            outGraphs.push_back(graph);
        }
    }

    void Editor::getAllPins(std::vector<Pin*>& outPins) const
    {
        for (size_t i = 0; i < m_registry->getNumRegisteredEntities(); i++)
        {
            Entity* entity = m_registry->getEntityAtIndex(i);

            if (entity->isOfType<Pin>())
            {
                Pin* pin = dynamic_cast<Pin*>(entity);
                outPins.push_back(pin);
            }
        }
	}

    void Editor::getAllAttributes(std::vector<Attribute*>& outAttributes) const
    {
        for (size_t i = 0; i < m_registry->getNumRegisteredEntities(); i++)
        {
            Entity* entity = m_registry->getEntityAtIndex(i);
            if (entity->isOfType<Attribute>())
            {
                Attribute* attribute = dynamic_cast<Attribute*>(entity);
                outAttributes.push_back(attribute);
            }
        }
    }

    void Editor::getAllTransitions(std::vector<Transition*>& outTransitions) const
    {
        for (size_t i = 0; i < m_registry->getNumRegisteredEntities(); i++)
        {
            Entity* entity = m_registry->getEntityAtIndex(i);
            if (entity->isOfType<Transition>())
            {
                Transition* transition = dynamic_cast<Transition*>(entity);
                outTransitions.push_back(transition);
            }
        }
    }

    Node* Editor::getNode(int nodeID) const
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

    Node* Editor::getNode(const std::string& name) const
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

    Node* Editor::findNodeByPath(const std::string& path) const
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

    Graph* Editor::getGraph(int graphID) const
    {
        for (size_t i = 0; i < m_registry->getNumRegisteredGraphs(); i++)
        {
            Graph* graph = m_registry->getGraphAtIndex(i);
            if (graph->getID() == graphID)
                return graph;
        }

        return nullptr;
    }

    Graph* Editor::getGraph(const std::string& name) const
    {
        for (size_t i = 0; i < m_registry->getNumRegisteredGraphs(); i++)
        {
            Graph* graph = m_registry->getGraphAtIndex(i);
            if (graph->getName() == name)
                return graph;
        }

        return nullptr;
	}

    Graph* Editor::findGraphByPath(const std::string& path) const
    {
        for (size_t i = 0; i < m_registry->getNumRegisteredGraphs(); i++)
        {
            Graph* graph = m_registry->getGraphAtIndex(i);
            if (graph->getFullName() == path)
                return graph;
        }

        return nullptr;
    }

    Attribute* Editor::findAttributeByPath(const std::string& path) const
    {
		std::vector<Attribute*> allAttributes;
		getAllAttributes(allAttributes);

        for (Attribute* attribute : allAttributes)
        {
            if (attribute->getFullName() == path)
                return attribute;
        }

        return nullptr;
	}

    Pin* Editor::findPinByPath(const std::string& path) const
    {
		std::vector<Pin*> allPins;
		getAllPins(allPins);

        for (Pin* pin : allPins)
        {
            if (pin->getFullName() == path)
                return pin;
		}

        return nullptr;
    }

    Transition* Editor::findTransitionByPath(const std::string& path) const
	{
		std::vector<Transition*> allTransitions;
        getAllTransitions(allTransitions);

        for (Transition* transition : allTransitions)
        {
            if (transition->getFullName() == path)
				return transition;
        }

		return nullptr;
	}

    Transition* Editor::getTransitionBetweenNodes(Node* sourceNode, Node* destinationNode) const
    {
		std::vector<Transition*> allTransitions;
		getAllTransitions(allTransitions);

        for (Transition* transition : allTransitions)
        {
            if ((transition->getSourceNode() == sourceNode && transition->getDestinationNode() == destinationNode) ||
                (transition->getSourceNode() == destinationNode && transition->getDestinationNode() == sourceNode))
                return transition;
		}

        return nullptr;
	}

    void Editor::getTransitionsFromNode(Node* sourceNode, std::vector<Transition*>& outTransitions) const
    {
		std::vector<Transition*> allTransitions;
		getAllTransitions(allTransitions);

        for (Transition* transition : allTransitions)
        {
            if (transition->getSourceNode() == sourceNode)
                outTransitions.push_back(transition);
		}
    }

    void Editor::getTransitionsToNode(Node* destinationNode, std::vector<Transition*>& outTransitions) const
    {
		std::vector<Transition*> allTransitions;
        getAllTransitions(allTransitions);

        for (Transition* transition : allTransitions)
        {
            if (transition->getDestinationNode() == destinationNode)
                outTransitions.push_back(transition);
        }
    }

    Node* Editor::getSelectedNode() const
    {
		int selectedNodeID = -1;

        if (ImNodes::NumSelectedNodes() != 1)
            return nullptr;

        ImNodes::GetSelectedNodes(&selectedNodeID);

        return dynamic_cast<Node*>(m_registry->findEntity(selectedNodeID));
    }

    Transition* Editor::getSelectedTransition() const
    {
        int selectedNodeID = -1;

        if (ImNodes::NumSelectedTransitions() != 1)
            return nullptr;

        ImNodes::GetSelectedTransition(&selectedNodeID);

        return dynamic_cast<Transition*>(m_registry->findEntity(selectedNodeID));
    }

    void Editor::clearSelection()
    {
        ImNodes::ClearLinkSelection();
        ImNodes::ClearNodeSelection();
		ImNodes::ClearTransitionSelection();
    }

    BlendTree* Editor::createRootBlendTree(int rootNodeID)
    {
        if (m_rootGraph)
            return nullptr;

		BlendTree* blendTree = new BlendTree(this, nullptr, "", nullptr, rootNodeID);

        pushGraph(blendTree);

		return blendTree;
	}

    StateMachine* Editor::createRootStateMachine(int rootNodeID)
    {
        if (m_rootGraph)
            return nullptr;

        StateMachine* stateMachine = new StateMachine(this, nullptr, "", nullptr, rootNodeID);

		pushGraph(stateMachine);

        return stateMachine;
	}

	void Editor::pushGraph(Graph* graph)
    {
        if (m_graphStack.empty())
			m_rootGraph = graph;
        else
		    getCurrentGraph()->onGraphClosed();

        m_graphStack.push(graph);

        graph->onGraphOpened();
	}

	void Editor::popGraph()
	{
		getCurrentGraph()->onGraphClosed();

        if (m_graphStack.size() > 1)
            m_graphStack.pop();

		getCurrentGraph()->onGraphOpened();
	}

    void Editor::initStyle()
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

    bool Editor::isNodeIDAvailable(int nodeID) const
    {
        for (size_t i = 0; i < m_controlParameters.size(); i++)
        {
            if (m_controlParameters[i]->getControlParameterID() == nodeID)
                return false;
        }

        return (getNode(nodeID) == nullptr);
	}

    void Editor::styleEditor()
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

            ImGui::BeginChild("##GridColorsChild", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

            ImGui::ColorEditUInt("Grid Background", &style.Colors[ImNodesCol_GridBackground]);
            ImGui::ColorEditUInt("Grid Line", &style.Colors[ImNodesCol_GridLine]);

			ImGui::EndChild();

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

			ImGui::BeginChild("##NodeColorsChild", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

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

			ImGui::EndChild();

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

			ImGui::BeginChild("##LinkColorsChild", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

            ImGui::ColorEditUInt("Link", &style.Colors[ImNodesCol_Link]);
            ImGui::ColorEditUInt("Link Hovered", &style.Colors[ImNodesCol_LinkHovered]);
            ImGui::ColorEditUInt("Link Selected", &style.Colors[ImNodesCol_LinkSelected]);

			ImGui::EndChild();

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

			ImGui::BeginChild("##PinColorsChild", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

            ImGui::ColorEditUInt("Pin", &style.Colors[ImNodesCol_Pin]);
            ImGui::ColorEditUInt("Pin Hovered", &style.Colors[ImNodesCol_PinHovered]);

			ImGui::ColorEditUInt("Float Data Pin", &customStyle.Colors[NodeEditorStyleCol_FloatDataPin]);
			ImGui::ColorEditUInt("Int Data Pin", &customStyle.Colors[NodeEditorStyleCol_IntDataPin]);
			ImGui::ColorEditUInt("UInt Data Pin", &customStyle.Colors[NodeEditorStyleCol_UIntDataPin]);
			ImGui::ColorEditUInt("Bool Data Pin", &customStyle.Colors[NodeEditorStyleCol_BoolDataPin]);
			ImGui::ColorEditUInt("Vector3 Data Pin", &customStyle.Colors[NodeEditorStyleCol_Vector3DataPin]);
			ImGui::ColorEditUInt("Vector4 Data Pin", &customStyle.Colors[NodeEditorStyleCol_Vector4DataPin]);
			ImGui::ColorEditUInt("Quaternion Data Pin", &customStyle.Colors[NodeEditorStyleCol_QuaternionDataPin]);

			ImGui::EndChild();

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

			ImGui::BeginChild("##TransitionColorsChild", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

            ImGui::ColorEditUInt("Transition", &style.Colors[ImNodesCol_Transition]);
            ImGui::ColorEditUInt("Transition Hovered", &style.Colors[ImNodesCol_TransitionHovered]);
            ImGui::ColorEditUInt("Transition Selected", &style.Colors[ImNodesCol_TransitionSelected]);

			ImGui::EndChild();

			ImGui::EndTabItem();
        }

        // -------------------- Mini-map --------------------
        if (ImGui::BeginTabItem("Mini-map"))
        {
            ImGui::SeparatorText("Sizes");

            ImGui::SliderFloat2("Padding", &style.MiniMapPadding.x, 0.0f, 32.0f);
            ImGui::SliderFloat2("Offset", &style.MiniMapOffset.x, -200.0f, 200.0f);

            ImGui::SeparatorText("Colors");

			ImGui::BeginChild("##MiniMapColorsChild", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

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

			ImGui::EndChild();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}