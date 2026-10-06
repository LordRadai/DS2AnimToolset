#include "GridStateMachineLayouterStrategy.h"

#include "RLog/RLog.h"
#include "extern.h"

#include "Utils.h"

#include <cmath>

bool GridStateMachineLayouterStrategy::setLayout(NodeEditor::Graph* graph, MR::NodeDef* graphNodeDef, std::vector<MR::NodeDef*>& childNodes)
{
	if (!graph->isOfType<NodeEditor::StateMachine>())
	{
		g_appLog->alertMessage(MsgLevel_Error, "GridStateMachineLayouterStrategy::setLayout: Graph '%s' is not a state machine.", graph->getName().c_str());
		return false;
	}

	NodeEditor::StateMachine* stateMachine = graph->asType<NodeEditor::StateMachine>();

	const std::vector<NodeEditor::Node*>& states = stateMachine->getNodes();
	const size_t n = states.size();

	// active states in a column on the left of the grid
	// (the editor's canvas starts at the origin: everything is shifted right by the widest active state)
	float shift = 0.f;
	for (size_t i = 0; i < stateMachine->getNumStateNodes(); i++)
		shift = std::max(shift, stateMachine->getStateNodeAt(i)->getSize().x + 2.f * LayouterUtils::COL_GAP);

	std::unordered_map<NodeEditor::Node*, LayouterUtils::Point2D> fixed;
	float y = 0.f;
	for (size_t i = 0; i < stateMachine->getNumStateNodes(); i++)
	{
		NodeEditor::StateNode* active = stateMachine->getStateNodeAt(i);
		ImVec2 size = active->getSize();

		active->setPosition(shift - size.x - 2.f * LayouterUtils::COL_GAP, y);
		fixed[active] = LayouterUtils::Point2D{ -size.x / 2.f - 2.f * LayouterUtils::COL_GAP, y + size.y / 2.f };

		y += size.y + LayouterUtils::ROW_GAP;
	}

	if (n == 0)
		return true;

	float maxW = 0.f, maxH = 0.f;
	for (NodeEditor::Node* state : states)
	{
		ImVec2 size = state->getSize();
		maxW = std::max(maxW, size.x);
		maxH = std::max(maxH, size.y);
	}

	const float cw = maxW + 2.f * LayouterUtils::COL_GAP;
	const float ch = maxH + 3.f * LayouterUtils::ROW_GAP;

	const int cols = (int)std::ceil(std::sqrt((double)n));
	int rows = (int)std::ceil((double)n / cols);
	if (n > 2 && (size_t)(cols * rows) == n)
		rows++;

	const int numCells = cols * rows;

	std::unordered_map<NodeEditor::Node*, int> idx;
	for (size_t i = 0; i < n; i++)
		idx[states[i]] = (int)i;

	// an endpoint is a state (>= 0) or an active state (-1 - index into fixedPos)
	std::vector<LayouterUtils::Point2D> fixedPos;
	std::unordered_map<NodeEditor::Node*, int> fixedIdx;
	auto endpoint = [&](NodeEditor::Node* node, int& out) -> bool
		{
			auto s = idx.find(node);
			if (s != idx.end())
			{
				out = s->second;
				return true;
			}

			auto f = fixed.find(node);
			if (f == fixed.end())
				return false;

			auto fi = fixedIdx.find(node);
			if (fi == fixedIdx.end())
			{
				fixedPos.push_back(f->second);
				fi = fixedIdx.emplace(node, -1 - (int)(fixedPos.size() - 1)).first;
			}

			out = fi->second;
			return true;
		};

	std::vector<std::pair<int, int>> edges;
	for (size_t i = 0; i < stateMachine->getNumTransitions(); i++)
	{
		NodeEditor::Transition* transition = stateMachine->getTransitionAt(i);
		int a, b;

		if (transition->getSourceNode() == transition->getDestinationNode())
			continue;

		if (endpoint(transition->getSourceNode(), a) && endpoint(transition->getDestinationNode(), b))
			edges.emplace_back(a, b);
	}

	std::vector<int> cell(n);
	for (size_t i = 0; i < n; i++)
		cell[i] = (int)i;

	std::vector<int> cellOwner(numCells, -1);
	for (size_t i = 0; i < n; i++)
		cellOwner[i] = (int)i;

	auto centre = [&](int e) -> LayouterUtils::Point2D
		{
			if (e < 0)
				return fixedPos[-1 - e];

			const int k = cell[e];
			return LayouterUtils::Point2D{ (k % cols) * cw + maxW / 2.f, (k / cols) * ch + maxH / 2.f };
		};

	std::vector<std::pair<LayouterUtils::Point2D, LayouterUtils::Point2D>> segs(edges.size());
	auto cost = [&]() -> double
		{
			for (size_t i = 0; i < edges.size(); i++)
				segs[i] = std::make_pair(centre(edges[i].first), centre(edges[i].second));

			double c = 0.0;
			for (size_t i = 0; i < edges.size(); i++)
			{
				const LayouterUtils::Point2D& p1 = segs[i].first;
				const LayouterUtils::Point2D& p2 = segs[i].second;

				c += std::hypot(p2.x - p1.x, p2.y - p1.y) / (cw + ch);

				for (size_t j = i + 1; j < edges.size(); j++)
				{
					if (edges[i].first == edges[j].first || edges[i].first == edges[j].second || edges[i].second == edges[j].first || edges[i].second == edges[j].second)
						continue;

					if (segmentsCross(p1, p2, segs[j].first, segs[j].second))
						c += 100.0;
				}

				for (int s = 0; s < (int)n; s++)
				{
					if (s == edges[i].first || s == edges[i].second)
						continue;

					if (segmentHits(p1, p2, centre(s), maxW / 2.f, maxH / 2.f))
						c += 60.0;
				}
			}

			return c;
		};

	if (!edges.empty() && n > 2)
	{
		const long long m = (long long)edges.size();
		const long long budget = std::min(40000LL, std::max(300LL, 4000000LL / (m * m + m * (long long)n + 1)));

		double best = cost();
		long long evals = 0;
		bool improved = true;

		while (improved && evals < budget)
		{
			improved = false;

			for (size_t i = 0; i < n && evals < budget; i++)
			{
				for (int target = 0; target < numCells; target++)
				{
					if (cell[i] == target)
						continue;

					const int other = cellOwner[target];
					const int old = cell[i];

					cell[i] = target;
					cellOwner[target] = (int)i;
					cellOwner[old] = other;
					if (other >= 0)
						cell[other] = old;

					const double c = cost();
					evals++;

					if (c < best - 1e-9)
					{
						best = c;
						improved = true;
					}
					else
					{
						cell[i] = old;
						cellOwner[old] = (int)i;
						cellOwner[target] = other;
						if (other >= 0)
							cell[other] = target;
					}

					if (evals >= budget)
						break;
				}
			}
		}
	}

	for (size_t i = 0; i < n; i++)
	{
		const int k = cell[i];
		states[i]->setPosition(shift + (k % cols) * cw, (k / cols) * ch);
	}

	return true;
}
