#include "Transition.h"
#include "NodeEditor/Node/Node.h"
#include "NodeEditor/imnodes/imnodes.h"
#include "NodeEditor/imnodes/imnodes_internal.h"
#include "imgui/imgui_internal.h"
#include <cmath>

namespace NodeEditor
{
    struct Anchors 
    {
        ImVec2 src;
        ImVec2 dst;
    };

    namespace
    {
        void AddArrow(ImDrawList* drawList, ImVec2 src, ImVec2 dst, ImU32 color, float thickness = 2.0f, float arrowSize = 10.0f)
        {
            // Draw main line
            drawList->AddLine(src, dst, color, thickness);

            // Compute direction vector of the line
            ImVec2 dir = dst - src;
            float length = sqrtf(dir.x * dir.x + dir.y * dir.y);
            if (length == 0.0f) return; // avoid division by zero
            dir.x /= length;
            dir.y /= length;

            // Perpendicular vector for arrowhead
            ImVec2 perp(-dir.y, dir.x);

            // Two arrowhead points
            ImVec2 arrowP1 = dst - dir * arrowSize + perp * (arrowSize * 0.5f);
            ImVec2 arrowP2 = dst - dir * arrowSize - perp * (arrowSize * 0.5f);

            // Draw arrowhead lines
            drawList->AddLine(dst, arrowP1, color, thickness);
            drawList->AddLine(dst, arrowP2, color, thickness);
        }

        ImVec2 ClosestPointSymmetric(const ImRect& rectA, const ImRect& rectB)
        {
            // X-axis
            float x;
            if (rectA.Max.x < rectB.Min.x) x = rectA.Max.x; // A left of B
            else if (rectB.Max.x < rectA.Min.x) x = rectA.Min.x; // A right of B
            else x = (rectA.GetCenter().x + rectB.GetCenter().x) * 0.5f; // overlap: pick center

            // Y-axis
            float y;
            if (rectA.Max.y < rectB.Min.y) y = rectA.Max.y; // A above B
            else if (rectB.Max.y < rectA.Min.y) y = rectA.Min.y; // A below B
            else y = (rectA.GetCenter().y + rectB.GetCenter().y) * 0.5f; // overlap: pick center

            return ImVec2(x, y);
        }

    }

	Transition::Transition(Graph* parent, int nodeID, Node* sourceNode, Node* destinationNode) : Node(parent, nodeID, sourceNode->getName() + "_" + destinationNode->getName(), nullptr),
		m_sourceNode(sourceNode), m_destinationNode(destinationNode)
	{
	}

    void Transition::draw()
    {
        if (!m_sourceNode || !m_destinationNode)
            return;

        // ------------------------------------------------------------
        // Node geometry
        // ------------------------------------------------------------
        const ImVec2 srcPos = ImNodes::GetNodeScreenSpacePos(m_sourceNode->getID());
        const ImVec2 srcSize = ImNodes::GetNodeDimensions(m_sourceNode->getID());
        const ImVec2 dstPos = ImNodes::GetNodeScreenSpacePos(m_destinationNode->getID());
        const ImVec2 dstSize = ImNodes::GetNodeDimensions(m_destinationNode->getID());

		ImRect srcRect(srcPos, ImVec2(srcPos.x + srcSize.x, srcPos.y + srcSize.y));
		ImRect dstRect(dstPos, ImVec2(dstPos.x + dstSize.x, dstPos.y + dstSize.y));

        ImVec2 srcAnchor = ClosestPointSymmetric(srcRect, dstRect);
        ImVec2 dstAnchor = ClosestPointSymmetric(dstRect, srcRect);
        
        // ------------------------------------------------------------
        // Draw straight line
        // ------------------------------------------------------------
        ImDrawList* drawList = ImGui::GetWindowDrawList();
		AddArrow(drawList, srcAnchor, dstAnchor, IM_COL32(255, 255, 255, 255), 2.0f, 12.0f);

		drawList->AddCircleFilled(srcAnchor, 4.0f, IM_COL32(0, 255, 0, 255));
		drawList->AddCircleFilled(dstAnchor, 4.0f, IM_COL32(255, 0, 0, 255));
    }
}
