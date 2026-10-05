#pragma once
#include "NodeEditor/NodeEditor.h"
#include <cmath>

namespace LayouterUtils
{
	constexpr float COL_GAP = 80.f;
	constexpr float ROW_GAP = 40.f;

	struct Point2D
	{
		float x, y;
	};

	// Input slot of the pin a link ends on: control pins first in pin order, data pins after them.
	int inputSlot(NodeEditor::Node* node, NodeEditor::Pin* pin);

	int orient(const Point2D& a, const Point2D& b, const Point2D& c);

	bool segmentsCross(const Point2D& p1, const Point2D& p2, const Point2D& q1, const Point2D& q2);

	// Does the segment a-b pass through the box of half size (hw, hh) centred on c
	bool segmentHits(const Point2D& a, const Point2D& b, const Point2D& c, float hw, float hh);
}