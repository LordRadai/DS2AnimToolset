#include "NodeEditor.h"
#include "imnodes/imnodes.h"
#include "Registry/Registry.h"
#include "IconsFontAwesome6.h"
#include "imgui_custom/imgui_custom_widget.h"
#include "Graph/BlendTree.h"
#include "extern.h"
#include "RLog/RLog.h"

namespace NodeEditor
{
    StyleSettings::StyleSettings() : NodeMinWidth(150.f), NodeMinContentHeight(30.f), StateNodeCornerRounding(2.f), StateNodeOutlineThickness(2.f), Colors();
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

		initStyle();

		return true;
	}

	void NodeEditor::shutdown()
	{
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
            }

			for (Node* node : currentGraph->getNodes())
			{
				ImVec2 nodePos = ImNodes::GetNodeGridSpacePos(node->getID());
				node->setPosition(nodePos.x, nodePos.y);
			}
		}

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

	void NodeEditor::draw()
	{
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
	}

	ControlParameter* NodeEditor::createControlParameter(const std::string& name, ControlParameter::ParameterType parameterType)
    {
        if (hasControlParameter(name))
        {
            g_appLog->debugMessage(MsgLevel_Warn, "NodeEditorBase::createControlParameter: Control parameter with name '%s' already exists.\n", name.c_str());
            return nullptr;
        }

        ControlParameter* parameter = new ControlParameter(this, name, parameterType);
        m_controlParameters.push_back(parameter);
		m_controlParametersNode->addOutputPin(parameter->getOutputPin());

        return parameter;
	}

    ControlParameter* NodeEditor::createControlParameterFloat(const std::string& name)
    {
        return createControlParameter(name, ControlParameter::kParameterTypeFloat);
    }

    ControlParameter* NodeEditor::createControlParameterInt(const std::string& name)
    {
        return createControlParameter(name, ControlParameter::kParameterTypeInt);
	}

    ControlParameter* NodeEditor::createControlParameterUInt(const std::string& name)
    {
        return createControlParameter(name, ControlParameter::kParameterTypeUInt);
    }

    ControlParameter* NodeEditor::createControlParameterBool(const std::string& name)
    {
        return createControlParameter(name, ControlParameter::kParameterTypeBool);
	}

    ControlParameter* NodeEditor::createControlParameterVector3(const std::string& name)
    {
        return createControlParameter(name, ControlParameter::kParameterTypeVector3);
    }

    ControlParameter* NodeEditor::createControlParameterVector4(const std::string& name)
    {
        return createControlParameter(name, ControlParameter::kParameterTypeVector4);
    }

    ControlParameter* NodeEditor::createControlParameterQuaternion(const std::string& name)
    {
        return createControlParameter(name, ControlParameter::kParameterTypeQuaternion);
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

	void NodeEditor::popGraph()
	{
		if (m_graphStack.size() > 1)
			m_graphStack.pop();
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
        style.NodeCornerRounding = 0.f;
        style.PinOffset = -5.5f;

        style.LinkLineSegmentsPerLength = 1.f;
        style.LinkThickness = 2.f;

        style.PinTriangleSideLength = 8.f;

        style.Colors[ImNodesCol_GridBackground] = IM_COL32(19, 18, 17, 255);

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

		m_styleSettings.Colors[NodeEditorStyleCol_FloatDataPin] = IM_COL32(150, 150, 250, 255);
		m_styleSettings.Colors[NodeEditorStyleCol_IntDataPin] = IM_COL32(150, 250, 150, 255);
		m_styleSettings.Colors[NodeEditorStyleCol_UIntDataPin] = IM_COL32(150, 250, 250, 255);
		m_styleSettings.Colors[NodeEditorStyleCol_BoolDataPin] = IM_COL32(250, 150, 150, 255);
		m_styleSettings.Colors[NodeEditorStyleCol_Vector3DataPin] = IM_COL32(250, 250, 150, 255);
		m_styleSettings.Colors[NodeEditorStyleCol_Vector4DataPin] = IM_COL32(250, 150, 250, 255);
		m_styleSettings.Colors[NodeEditorStyleCol_QuaternionDataPin] = IM_COL32(200, 200, 200, 255);
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