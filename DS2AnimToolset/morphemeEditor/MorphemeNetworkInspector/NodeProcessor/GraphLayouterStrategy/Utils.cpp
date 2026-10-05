#include "Utils.h"

namespace LayouterUtils
{
	int inputSlot(NodeEditor::Node* node, NodeEditor::Pin* pin)
	{
		for (size_t i = 0; i < node->getNumInputPins(); i++)
			if (node->getInputPin(i) == pin)
				return (int)i;

		for (size_t i = 0; i < node->getNumInputDataPins(); i++)
			if (node->getInputDataPin(i) == pin)
				return 100 + (int)i;

		return 1000;
	}

	int orient(const Point2D& a, const Point2D& b, const Point2D& c)
	{
		const float v = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
		return (v > 0) - (v < 0);
	}

	bool segmentsCross(const Point2D& p1, const Point2D& p2, const Point2D& q1, const Point2D& q2)
	{
		return orient(p1, p2, q1) * orient(p1, p2, q2) < 0 && orient(q1, q2, p1) * orient(q1, q2, p2) < 0;
	}

	// Does the segment a-b pass through the box of half size (hw, hh) centred on c
	bool segmentHits(const Point2D& a, const Point2D& b, const Point2D& c, float hw, float hh)
	{
		for (int s = 1; s < 12; s++)
		{
			const float x = a.x + (b.x - a.x) * s / 12.f;
			const float y = a.y + (b.y - a.y) * s / 12.f;

			if (std::fabs(x - c.x) < hw && std::fabs(y - c.y) < hh)
				return true;
		}

		return false;
	}
}