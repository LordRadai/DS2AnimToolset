#include "NodeEditorBase.h"
#include "imnodes/imnodes.h"
#include "Registry/Registry.h"
#include "IconsFontAwesome6.h"
#include "imgui_custom/imgui_custom_widget.h"
#include "extern.h"
#include "RLog/RLog.h"

namespace NodeEditor
{
	NodeEditorBase::NodeEditorBase()
	{
	}

	NodeEditorBase::~NodeEditorBase()
	{
		shutdown();
	}

	bool NodeEditorBase::initialise()
	{
		if (!ImNodes::CreateContext())
			return false;

		Registry::getInstance();

		initStyle();

		return true;
	}

	void NodeEditorBase::shutdown()
	{
		Registry::destroyInstance();
		ImNodes::DestroyContext();
	}

	void NodeEditorBase::update(float dt)
	{
		Graph* currentGraph = getCurrentGraph();

		if (currentGraph)
		{
			for (Node* node : currentGraph->getNodes())
			{
				ImVec2 nodePos = ImNodes::GetNodeGridSpacePos(node->getID());
				node->setPosition(nodePos.x, nodePos.y);
			}
		}

		int hoveredNodeId = -1;

		if (ImNodes::IsNodeHovered(&hoveredNodeId))
		{
			Registry* registry = Registry::getInstance();
			Node* hoveredNode = dynamic_cast<Node*>(registry->findEntity(hoveredNodeId));

			if (hoveredNode && hoveredNode->hasSubGraph() && ImGui::IsMouseDoubleClicked(0))
				pushGraph(hoveredNode->getSubGraph());
		}
		else if (ImGui::IsMouseDoubleClicked(0))
		{
			popGraph();
		}
	}

	void NodeEditorBase::draw()
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

		ImGui::Label(parentGraphList.c_str());

		ImNodes::BeginNodeEditor();

		Graph* currentGraph = getCurrentGraph();

		if (currentGraph)
			currentGraph->draw();

		ImNodes::EndNodeEditor();
	}

	void NodeEditorBase::popGraph()
	{
		if (m_graphStack.size() > 1)
			m_graphStack.pop();
	}

    void NodeEditorBase::initStyle()
    {
        ImNodes::StyleColorsDark();

        ImNodesStyle& style = ImNodes::GetStyle();
        style.NodePadding.x = 13.0f;
        style.NodePadding.y = 5.0f;
        style.NodeBorderThickness = 0.f;
        style.NodeCornerRounding = 0.f;
        style.PinOffset = -5.5f;

        style.LinkLineSegmentsPerLength = 1.f;
        style.LinkThickness = 2.f;

        style.PinTriangleSideLength = 8.f;

        style.Colors[ImNodesCol_NodeBackground] = IM_COL32(26, 55, 80, 255);
        style.Colors[ImNodesCol_NodeBackgroundHovered] = IM_COL32(26, 55, 80, 255);
        style.Colors[ImNodesCol_NodeBackgroundSelected] = IM_COL32(118, 113, 25, 255);
        style.Colors[ImNodesCol_TitleBar] = IM_COL32(47, 93, 160, 255);
        style.Colors[ImNodesCol_TitleBarHovered] = IM_COL32(47, 93, 160, 255);
		style.Colors[ImNodesCol_TitleBarSelected] = IM_COL32(156, 150, 35, 255);
		style.Colors[ImNodesCol_NodeOutline] = IM_COL32(60, 60, 60, 255);

		style.Colors[ImNodesCol_Pin] = IM_COL32(200, 200, 200, 255);
		style.Colors[ImNodesCol_PinHovered] = IM_COL32(255, 255, 255, 255);

        style.Colors[ImNodesCol_Link] = IM_COL32(255, 255, 255, 200);
        style.Colors[ImNodesCol_LinkHovered] = IM_COL32(255, 255, 255, 255);
        style.Colors[ImNodesCol_LinkSelected] = IM_COL32(255, 255, 255, 255);
	}

    void NodeEditorBase::styleEditor()
    {
        ImNodesStyle& style = ImNodes::GetStyle();

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

            ImGui::SeparatorText("Colors");

            ImGui::ColorEditUInt("Node Background", &style.Colors[ImNodesCol_NodeBackground]);
            ImGui::ColorEditUInt("Node Background Hovered", &style.Colors[ImNodesCol_NodeBackgroundHovered]);
            ImGui::ColorEditUInt("Node Background Selected", &style.Colors[ImNodesCol_NodeBackgroundSelected]);
            ImGui::ColorEditUInt("Node Outline", &style.Colors[ImNodesCol_NodeOutline]);
            ImGui::ColorEditUInt("Title Bar", &style.Colors[ImNodesCol_TitleBar]);
            ImGui::ColorEditUInt("Title Bar Hovered", &style.Colors[ImNodesCol_TitleBarHovered]);
            ImGui::ColorEditUInt("Title Bar Selected", &style.Colors[ImNodesCol_TitleBarSelected]);

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
            ImGui::SliderFloat("Pin Offset", &style.PinOffset, -8.0f, 8.0f);

            ImGui::SeparatorText("Colors");

            ImGui::ColorEditUInt("Pin", &style.Colors[ImNodesCol_Pin]);
            ImGui::ColorEditUInt("Pin Hovered", &style.Colors[ImNodesCol_PinHovered]);
            ImGui::ColorEditUInt("Box Selector", &style.Colors[ImNodesCol_BoxSelector]);
            ImGui::ColorEditUInt("Box Selector Outline", &style.Colors[ImNodesCol_BoxSelectorOutline]);

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