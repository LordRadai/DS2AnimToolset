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

        ImVec2 ClosestPoint(const ImRect& a, const ImRect& b)
        {
            ImVec2 center = a.GetCenter();
            ImVec2 halfSize(a.GetWidth() * 0.5f, a.GetHeight() * 0.5f);

            ImVec2 otherCenter = b.GetCenter();
            ImVec2 closestPoint;

            // Compute min/max bounds
            float minX = center.x - halfSize.x;
            float maxX = center.x + halfSize.x;
            float minY = center.y - halfSize.y;
            float maxY = center.y + halfSize.y;

            // Clamp to rectangle bounds
            closestPoint.x = std::fmax(minX, std::fmin(otherCenter.x, maxX));
            closestPoint.y = std::fmax(minY, std::fmin(otherCenter.y, maxY));

            return closestPoint;
        }

        ImVec2 ClosestPointToRect(const ImRect& rect, const ImVec2& point, bool clampToEdge = true)
        {
            // Clamp the point to rectangle bounds
            float x = std::fmax(rect.Min.x, std::fmin(point.x, rect.Max.x));
            float y = std::fmax(rect.Min.y, std::fmin(point.y, rect.Max.y));

            ImVec2 closest(x, y);

            if (clampToEdge && rect.Contains(point))
            {
                // Push to the nearest edge if point is inside
                float leftDist = point.x - rect.Min.x;
                float rightDist = rect.Max.x - point.x;
                float topDist = point.y - rect.Min.y;
                float bottomDist = rect.Max.y - point.y;

                float minDist = std::min({ leftDist, rightDist, topDist, bottomDist });

                if (minDist == leftDist)       closest.x = rect.Min.x;
                else if (minDist == rightDist) closest.x = rect.Max.x;
                else if (minDist == topDist)   closest.y = rect.Min.y;
                else                           closest.y = rect.Max.y;
            }

            return closest;
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

        ImVec2 centerSrc = srcRect.GetCenter();
        ImVec2 centerDst = dstRect.GetCenter();
        ImVec2 dir = centerDst - centerSrc;

        int selectedNodeID = -1;
		ImNodes::GetSelectedNodes(&selectedNodeID);

        ImVec2 srcAnchor;
        ImVec2 dstAnchor;

        if (selectedNodeID == m_destinationNode->getID())
        {
            srcAnchor = ClosestPointToRect(srcRect, dstRect.GetCenter());
            dstAnchor = ClosestPointToRect(dstRect, srcAnchor);
        }
        else
        {
            dstAnchor = ClosestPointToRect(dstRect, srcRect.GetCenter());
            srcAnchor = ClosestPointToRect(srcRect, dstAnchor);
        }
        
        // ------------------------------------------------------------
        // Draw straight line
        // ------------------------------------------------------------
        ImDrawList* drawList = ImGui::GetWindowDrawList();
		AddArrow(drawList, srcAnchor, dstAnchor, IM_COL32(255, 255, 255, 255), 2.0f, 12.0f);

		drawList->AddCircleFilled(srcAnchor, 4.0f, IM_COL32(0, 255, 0, 255));
		drawList->AddCircleFilled(dstAnchor, 4.0f, IM_COL32(255, 0, 0, 255));
    }
}
