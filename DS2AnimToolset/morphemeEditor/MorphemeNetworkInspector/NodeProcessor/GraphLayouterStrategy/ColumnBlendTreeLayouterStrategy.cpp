#include "ColumnBlendTreeLayouterStrategy.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <map>
#include <set>
#include <tuple>
#include <unordered_map>

#include "RLog/RLog.h"
#include "extern.h"

#include "NodeEditor/Editor/Graph/BlendTree.h"
#include "NodeEditor/Editor/Graph/StateMachine.h"
#include "NodeEditor/Editor/Link/Link.h"

#include "Utils.h"

bool ColumnBlendTreeLayouterStrategy::setLayout(NodeEditor::Graph* graph, MR::NodeDef* graphNodeDef, std::vector<MR::NodeDef*>& childNodes)
{
	if (!graph->isOfType<NodeEditor::BlendTree>())
	{
		g_appLog->alertMessage(MsgLevel_Error, "ColumnBlendTreeLayouterStrategy::setLayout: Graph '%s' is not a blend tree.", graph->getName().c_str());
		return false;
	}

	NodeEditor::BlendTree* blendTree = graph->asType<NodeEditor::BlendTree>();
	const std::vector<NodeEditor::Node*>& items = blendTree->getNodes();

	std::unordered_map<NodeEditor::Node*, size_t> index;
	for (size_t i = 0; i < items.size(); i++)
		index[items[i]] = i;

	// consumers of each item inside this graph, with the lowest input slot used on that consumer
	std::vector<std::vector<size_t>> consumers(items.size());
	std::map<std::pair<size_t, size_t>, int> slot;

	for (NodeEditor::Link* link : blendTree->getLinks())
	{
		if (!link->getOutputPin() || !link->getInputPin())
			continue;

		auto src = index.find(link->getOutputPin()->getParentNode());
		auto dst = index.find(link->getInputPin()->getParentNode());

		if (src == index.end() || dst == index.end() || src->second == dst->second)
			continue;

		const int k = LayouterUtils::inputSlot(dst->first, link->getInputPin());
		auto key = std::make_pair(src->second, dst->second);
		auto it = slot.find(key);

		if (it == slot.end())
		{
			consumers[src->second].push_back(dst->second);
			slot[key] = k;
		}
		else
			it->second = std::min(it->second, k);
	}

	// column = longest path to the output
	std::vector<int> col(items.size(), 0);
	std::vector<bool> onStack(items.size(), false);

	std::function<int(size_t)> depth = [&](size_t i) -> int
	{
		if (col[i])
			return col[i];

		if (onStack[i])
			return 1;

		onStack[i] = true;

		int d = 0;
		for (size_t c : consumers[i])
			d = std::max(d, depth(c));

		onStack[i] = false;
		col[i] = d + 1;

		return col[i];
	};

	int maxCol = 1;
	for (size_t i = 0; i < items.size(); i++)
		maxCol = std::max(maxCol, depth(i));

	std::vector<ImVec2> sizes(items.size());
	std::map<int, float> colWidth;
	for (size_t i = 0; i < items.size(); i++)
	{
		sizes[i] = items[i]->getSize();
		colWidth[col[i]] = std::max(colWidth[col[i]], sizes[i].x);
	}

	std::map<int, float> colX;
	float x = 0.f;
	for (int c = 1; c <= maxCol; c++)
	{
		auto w = colWidth.find(c);
		x -= (w != colWidth.end() ? w->second : 210.f) + LayouterUtils::COL_GAP;
		colX[c] = x;
	}

	std::vector<LayouterUtils::Point2D> pos(items.size(), LayouterUtils::Point2D{ 0.f, 0.f });
	std::vector<float> ypos(items.size(), 0.f);

	for (int c = 1; c <= maxCol; c++)
	{
		// top-down: by the consumer's row, then by input slot (input 1 above input 2)
		std::vector<std::tuple<float, int, std::string, size_t>> members;

		for (size_t i = 0; i < items.size(); i++)
		{
			if (col[i] != c)
				continue;

			std::pair<float, int> key(0.f, 0);
			bool first = true;
			for (size_t d : consumers[i])
			{
				std::pair<float, int> k(ypos[d], slot[std::make_pair(i, d)]);
				if (first || k < key)
					key = k;

				first = false;
			}

			members.emplace_back(key.first, key.second, items[i]->getName(), i);
		}

		std::sort(members.begin(), members.end());

		float y = 0.f;
		for (auto& m : members)
		{
			const size_t i = std::get<3>(m);
			pos[i] = LayouterUtils::Point2D{ colX[c], y };
			ypos[i] = y;
			y += sizes[i].y + LayouterUtils::ROW_GAP;
		}
	}

	const float cpX = colX.count(maxCol) ? colX[maxCol] - 200.f - LayouterUtils::COL_GAP : -210.f - 200.f - LayouterUtils::COL_GAP;

	// the editor's canvas starts at the origin: shift everything so the leftmost node sits at x = 0
	const float shift = -cpX;

	for (size_t i = 0; i < items.size(); i++)
		items[i]->setPosition(pos[i].x + shift, pos[i].y);

	blendTree->getOutputNode()->setPosition(shift, 0.f);
	blendTree->setControlParamsNodePosition(cpX + shift, 0.f);

	// pass down pins sit under the control parameters, both feed the graph from the left
	if (blendTree->getNumPassDownPins() > 0)
		blendTree->setPassDownPinsNodePosition(cpX + shift, blendTree->getControlParametersNode()->getSize().y + LayouterUtils::ROW_GAP);

	return true;
}