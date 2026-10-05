#include "NetworkHierarchy.h"

#include <algorithm>
#include <cstdarg>

#include "morpheme/mrNodeDef.h"

namespace
{
	std::vector<std::string> splitPath(const std::string& path)
	{
		std::vector<std::string> out;
		size_t start = 0;
		while (true)
		{
			size_t pos = path.find('|', start);
			out.push_back(path.substr(start, pos == std::string::npos ? std::string::npos : pos - start));
			if (pos == std::string::npos)
				break;
			start = pos + 1;
		}
		return out;
	}
}

NetworkHierarchy::NetworkHierarchy(MR::NetworkDef* netDef, std::function<std::string(MR::NodeDef*)> animName, std::function<std::string(MR::NodeType)> typeName)
	: m_netDef(netDef), m_rootID(netDef->getRootNodeID()), m_animName(animName), m_typeName(typeName)
{
	const uint32_t numNodes = netDef->getNumNodeDefs();

	for (uint32_t i = 0; i < numNodes; i++)
	{
		MR::NodeDef* nodeDef = node(i);
		if (!nodeDef)
			continue;

		if (isStateMachine(i))
		{
			for (uint32_t j = 0; j < nodeDef->getNumChildNodes(); j++)
			{
				MR::NodeID child = nodeDef->getChildNodeID(j);
				if (exists(child) && !isTransition(child))
				{
					m_stateOwner[child] = i;
					m_states[i].push_back(child);
				}
			}
			continue;
		}

		if (!isPlaceable(i))
			continue;

		std::vector<MR::NodeID>& inputs = m_inputs[i];
		for (uint32_t j = 0; j < nodeDef->getNumChildNodes(); j++)
		{
			MR::NodeID child = nodeDef->getChildNodeID(j);
			if (exists(child))
				inputs.push_back(child);
		}
		for (uint32_t j = 0; j < nodeDef->getNumInputCPConnections(); j++)
		{
			MR::NodeID src = nodeDef->getInputCPConnectionSourceNodeID(j);
			if (exists(src))
				inputs.push_back(src);
		}
		for (MR::NodeID in : inputs)
			m_consumers[in].push_back(i);
	}

	// Names: entries below the node count name nodes, the ones after name the container holding a state's root
	const NMP::IDMappedStringTable* names = netDef->getNodeIDNamesTable();
	if (names)
	{
		for (uint32_t e = 0; e < names->getNumEntries(); e++)
		{
			const MR::NodeID id = names->getEntryID(e);
			const char* str = names->getEntryString(e);
			if (!str || !*str || !exists(id))
				continue;

			if (e < numNodes)
			{
				if (!isCP(id) && !isNetwork(id))
					m_named[id] = str;
			}
			else
			{
				m_stateNames[{ node(id)->getParentNodeID(), id }] = str;
			}
		}
	}
}

MR::NodeDef* NetworkHierarchy::node(MR::NodeID id) const
{
	if (id == MR::INVALID_NODE_ID || id >= m_netDef->getNumNodeDefs())
		return nullptr;
	return m_netDef->getNodeDef(id);
}

bool NetworkHierarchy::isCP(MR::NodeID id) const
{
	MR::NodeDef* n = node(id);
	return n && n->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM);
}

bool NetworkHierarchy::isTransition(MR::NodeID id) const
{
	MR::NodeDef* n = node(id);
	return n && n->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_TRANSITION);
}

bool NetworkHierarchy::isNetwork(MR::NodeID id) const
{
	MR::NodeDef* n = node(id);
	return n && n->getNodeTypeID() == NODE_TYPE_NETWORK;
}

bool NetworkHierarchy::isStateMachine(MR::NodeID id) const
{
	MR::NodeDef* n = node(id);
	return n && n->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE);
}

bool NetworkHierarchy::isOperator(MR::NodeID id) const
{
	// Operators only produce control parameter outputs
	MR::NodeDef* n = node(id);
	return n && !isCP(id) && n->getNumOutputCPPins() > 0 && n->getNumChildNodes() == 0;
}

bool NetworkHierarchy::isMultiplyConnected(MR::NodeID id) const
{
	MR::NodeDef* n = node(id);
	return n && n->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_OUTPUT_REFERENCED);
}

bool NetworkHierarchy::isPlaceable(MR::NodeID id) const
{
	return exists(id) && !isCP(id) && !isTransition(id) && !isNetwork(id);
}

uint32_t NetworkHierarchy::getNestingDepth(MR::NodeID p, MR::NodeID r) const
{
	auto it = m_nbtDepth.find({ p, r });
	return it == m_nbtDepth.end() ? 0 : it->second;
}

void NetworkHierarchy::log(const char* fmt, ...)
{
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	m_log.push_back(buf);
}

//----------------------------------------------------------------------------------------------------------------------
// placement
//----------------------------------------------------------------------------------------------------------------------
NetworkHierarchy::Graph NetworkHierarchy::graph(MR::NodeID id, std::set<MR::NodeID>& stack)
{
	auto it = m_G.find(id);
	if (it != m_G.end())
		return it->second;

	auto ov = m_override.find(id);
	if (ov != m_override.end())
	{
		m_G[id] = ov->second;
		return ov->second;
	}

	if (stack.count(id))
	{
		log("placement cycle at node %d", id);
		return Graph::root();
	}
	stack.insert(id);

	Graph g;
	MR::NodeDef* n = node(id);
	const MR::NodeID parent = n->getParentNodeID();

	auto consumerGraphs = [&]() {
		std::vector<Graph> gs;
		for (MR::NodeID c : m_consumers[id])
			gs.push_back(graph(c, stack));
		return gs;
	};

	if (isStateRoot(id))
		g = Graph::state(m_stateOwner[id], id);
	else if (id == m_rootID)
		g = Graph::root();
	else if (isOperator(id) || !exists(parent))
	{
		std::vector<Graph> gs = consumerGraphs();
		g = gs.empty() ? Graph::root() : lca(gs);
	}
	else if (isNetwork(parent))
		g = Graph::root();
	else if (isStateMachine(parent))
	{
		// the requester is a state machine: the state of it that holds the consumers
		std::vector<Graph> gs = consumerGraphs();
		bool found = false;
		for (MR::NodeID st : m_states[parent])
		{
			Graph sg = Graph::state(parent, st);
			bool all = !gs.empty();
			for (const Graph& x : gs)
				all = all && within(x, sg);
			if (all)
			{
				g = sg;
				found = true;
				break;
			}
		}
		if (!found)
		{
			log("node %d: requester state machine %d has no state holding all consumers", id, parent);
			g = !gs.empty() ? lca(gs) : (m_states[parent].empty() ? Graph::root() : Graph::state(parent, m_states[parent][0]));
		}
	}
	else
		g = graph(parent, stack);

	stack.erase(id);
	m_G[id] = g;
	return g;
}

NetworkHierarchy::Graph NetworkHierarchy::parentGraph(const Graph& g)
{
	// callers check for the root first
	if (g.kind == kNested)
	{
		if (g.level > 1)
			return Graph::nested(g.a, g.b, g.level - 1);
		if (isNetwork(g.a))
			return Graph::root();
		if (isStateMachine(g.a))
			return Graph::state(g.a, g.b); // state nesting: inside the state rooted at R
		return graph(g.a);
	}

	const MR::NodeID s = g.a;
	if (isStateRoot(s))
		return Graph::state(m_stateOwner[s], s);
	return graph(s);
}

std::vector<NetworkHierarchy::Graph> NetworkHierarchy::ancestors(const Graph& g)
{
	std::vector<Graph> out;
	Graph cur = g;
	for (int guard = 0; guard < 512; guard++)
	{
		out.push_back(cur);
		if (cur.kind == kRoot)
			break;
		cur = parentGraph(cur);
	}
	return out;
}

bool NetworkHierarchy::within(const Graph& g, const Graph& outer)
{
	std::vector<Graph> a = ancestors(g);
	return std::find(a.begin(), a.end(), outer) != a.end();
}

NetworkHierarchy::Graph NetworkHierarchy::lca(const std::vector<Graph>& gs)
{
	std::vector<Graph> common;
	bool first = true;
	for (const Graph& g : gs)
	{
		std::vector<Graph> a = ancestors(g);
		if (first)
		{
			common = a;
			first = false;
			continue;
		}
		std::vector<Graph> next;
		for (const Graph& x : common)
			if (std::find(a.begin(), a.end(), x) != a.end())
				next.push_back(x);
		common.swap(next);
	}
	return common.empty() ? Graph::root() : common.front();
}

void NetworkHierarchy::nestedContainers()
{
	// named nodes are members too: build() moves one back beside its requester when its path says so
	std::map<MR::NodeID, std::vector<MR::NodeID>> groups;
	for (uint32_t i = 0; i < m_netDef->getNumNodeDefs(); i++)
	{
		if (!isPlaceable(i) || !isMultiplyConnected(i) || isOperator(i) || isStateRoot(i))
			continue;
		MR::NodeID p = node(i)->getParentNodeID();
		if (!exists(p) || isStateMachine(p))
			continue;
		groups[p].push_back(i);
	}

	for (auto& grp : groups)
	{
		const MR::NodeID P = grp.first;
		const std::vector<MR::NodeID>& members = grp.second;
		std::set<MR::NodeID> mset(members.begin(), members.end());

		// last node before P on c's parent chain (stops early at a group member)
		auto walk = [&](MR::NodeID c) -> MR::NodeID {
			std::set<MR::NodeID> seen;
			MR::NodeID x = c;
			while (exists(x) && !seen.count(x))
			{
				seen.insert(x);
				MR::NodeID px = node(x)->getParentNodeID();
				if (px == P || mset.count(x))
					return x;
				x = px;
			}
			return MR::INVALID_NODE_ID;
		};

		std::map<MR::NodeID, std::set<MR::NodeID>> inside; // member -> members it consumes (directly or via subtree)
		std::set<MR::NodeID> roots;
		for (MR::NodeID m : members)
		{
			for (MR::NodeID c : m_consumers[m])
			{
				MR::NodeID w = mset.count(c) ? c : walk(c);
				if (w == MR::INVALID_NODE_ID)
					continue;
				if (mset.count(w) && w != m)
					inside[w].insert(m);
				else if (!mset.count(w))
					roots.insert(w);
			}
		}
		if (roots.empty())
			continue;
		if (roots.size() > 1)
			log("requester %d: members reach several inputs", P);
		const MR::NodeID R = *roots.begin();

		std::map<MR::NodeID, uint32_t> level;
		std::function<uint32_t(MR::NodeID, std::set<MR::NodeID>&)> lev = [&](MR::NodeID m, std::set<MR::NodeID>& stk) -> uint32_t {
			auto it = level.find(m);
			if (it != level.end())
				return it->second;
			if (stk.count(m))
				return 1;
			stk.insert(m);
			uint32_t best = 0;
			for (MR::NodeID x : inside[m])
				best = std::max(best, lev(x, stk));
			stk.erase(m);
			level[m] = best + 1;
			return best + 1;
		};
		uint32_t L = 0;
		for (MR::NodeID m : members)
		{
			std::set<MR::NodeID> stk;
			L = std::max(L, lev(m, stk));
		}

		m_nbtDepth[{ P, R }] = L;
		for (MR::NodeID m : members)
		{
			if (level[m] > 1)
				m_override[m] = Graph::nested(P, R, level[m] - 1);
			else if (isNetwork(P))
				m_override[m] = Graph::root();
		}
		m_override[R] = Graph::nested(P, R, L);
	}
}

bool NetworkHierarchy::stateNesting()
{
	std::map<Graph, std::set<MR::NodeID>> outside;
	for (uint32_t i = 0; i < m_netDef->getNumNodeDefs(); i++)
	{
		if (!isPlaceable(i) || !isMultiplyConnected(i) || isOperator(i))
			continue;
		MR::NodeID p = node(i)->getParentNodeID();
		auto git = m_G.find(i);
		if (!isStateMachine(p) || isStateRoot(i) || git == m_G.end())
			continue;
		const Graph g = git->second;
		if (g.kind != kState || g.a != p)
			continue;
		if (m_nbtDepth.count({ p, g.b }) || m_noNest.count({ p, g.b }) || isStateMachine(g.b))
			continue;

		const std::vector<MR::NodeID>& cons = m_consumers[i];
		std::set<MR::NodeID> inside;
		bool deeper = false;
		for (MR::NodeID c : cons)
		{
			auto cg = m_G.find(c);
			if (cg != m_G.end() && cg->second == g) inside.insert(c); else deeper = true;
		}
		// feeds a node of the state and something deeper, or several nodes of the state: either way more than one
		// input, which only a pass-down pin into a nested blend tree can fan out to
		if (cons.size() > 1 && !inside.empty() && (deeper || inside.size() > 1))
			outside[g].insert(i);
	}

	for (auto& entry : outside)
	{
		const Graph g = entry.first;
		std::set<MR::NodeID>& keep = entry.second;

		// private upstream of the kept nodes stays beside them
		std::vector<MR::NodeID> todo(keep.begin(), keep.end());
		while (!todo.empty())
		{
			MR::NodeID m = todo.back();
			todo.pop_back();
			for (MR::NodeID u : m_inputs[m])
			{
				auto ug = m_G.find(u);
				if (keep.count(u) || ug == m_G.end() || ug->second != g || u == g.b)
					continue;
				bool priv = true;
				for (MR::NodeID c : m_consumers[u])
					priv = priv && keep.count(c);
				if (priv)
				{
					keep.insert(u);
					todo.push_back(u);
				}
			}
		}

		const MR::NodeID P = g.a, R = g.b;
		m_nbtDepth[{ P, R }] = 1;
		for (auto& gi : m_G)
			if (gi.second == g && !keep.count(gi.first) && !isOperator(gi.first))
				m_override[gi.first] = Graph::nested(P, R, 1);
		m_override[R] = Graph::nested(P, R, 1);
		log("state %d of %d: nested blend tree for multiply connected nodes", R, P);
	}

	return !outside.empty();
}

std::set<MR::NodeID> NetworkHierarchy::contradictedMembers()
{
	std::set<MR::NodeID> out;
	for (auto& nm : m_named)
	{
		auto ov = m_override.find(nm.first);
		if (ov == m_override.end() || ov->second.kind != kNested || nm.first == ov->second.b)
			continue;
		const MR::NodeID P = ov->second.a;
		Graph beside = isNetwork(P) ? Graph::root() : graph(P);
		if (chainOfGraph(beside).size() + 1 == splitPath(nm.second).size())
			out.insert(nm.first);
	}
	return out;
}

void NetworkHierarchy::compactNesting(std::pair<MR::NodeID, MR::NodeID> key)
{
	std::set<uint32_t> levels;
	for (auto& ov : m_override)
		if (ov.second.kind == kNested && ov.second.a == key.first && ov.second.b == key.second)
			levels.insert(ov.second.level);

	std::map<uint32_t, uint32_t> remap;
	uint32_t k = 1;
	for (uint32_t l : levels)
		remap[l] = k++;

	for (auto& ov : m_override)
		if (ov.second.kind == kNested && ov.second.a == key.first && ov.second.b == key.second)
			ov.second.level = remap[ov.second.level];

	m_nbtDepth[key] = (uint32_t)levels.size();
}

std::set<std::pair<MR::NodeID, MR::NodeID>> NetworkHierarchy::contradictedNesting()
{
	std::set<std::pair<MR::NodeID, MR::NodeID>> drop;
	std::vector<std::pair<std::vector<Entity>, std::string>> checks;

	for (auto& nm : m_named)
		if (isPlaceable(nm.first))
			checks.push_back({ chain(nm.first), nm.second });

	for (auto& sn : m_stateNames)
	{
		const MR::NodeID sm = sn.first.first, r = sn.first.second;
		if (isStateMachine(sm) && isStateRoot(r) && m_stateOwner[r] == sm && !isStateMachine(r))
			checks.push_back({ chainOfGraph(Graph::state(sm, r)), sn.second });
	}

	for (auto& check : checks)
	{
		const size_t n = splitPath(check.second).size();
		std::vector<Graph> nbts;
		for (const Entity& e : check.first)
			if (e.kind == Entity::kNestedEnt)
				nbts.push_back(e.graph);
		if (!nbts.empty() && check.first.size() > n && check.first.size() - nbts.size() == n)
			for (const Graph& g : nbts)
				drop.insert({ g.a, g.b });
	}
	return drop;
}

bool NetworkHierarchy::build()
{
	nestedContainers();
	m_wrapped.clear();
	m_noNest.clear();

	for (int iter = 0; iter < 8; iter++)
	{
		m_G.clear();
		for (uint32_t i = 0; i < m_netDef->getNumNodeDefs(); i++)
			if (isPlaceable(i))
				graph(i);

		if (stateNesting())
			continue;

		// names are the ground truth: a named member of a nesting chain whose path puts it beside its requester
		// stays there (the rest of the chain keeps its blend trees) ...
		std::set<MR::NodeID> unnest = contradictedMembers();
		if (!unnest.empty())
		{
			for (MR::NodeID i : unnest)
			{
				std::pair<MR::NodeID, MR::NodeID> key = { m_override[i].a, m_override[i].b };
				log("named node %d kept beside requester %d", i, key.first);
				m_override.erase(i);
				compactNesting(key);
			}
			continue;
		}

		// ... and nested blend trees that a named path shows were not there at all are dropped
		std::set<std::pair<MR::NodeID, MR::NodeID>> drop = contradictedNesting();
		if (drop.empty())
			break;
		for (auto& key : drop)
		{
			log("nesting for requester %d / input %d dropped: named paths have no level for it", key.first, key.second);
			m_nbtDepth.erase(key);
			m_noNest.insert(key);
			for (auto it = m_override.begin(); it != m_override.end();)
			{
				if (it->second.kind == kNested && it->second.a == key.first && it->second.b == key.second)
					it = m_override.erase(it);
				else
					++it;
			}
		}
	}

	// state roots that are state machines are only blend trees ("wrappers") when something else lives in their state
	std::map<Graph, int> occupied;
	for (auto& gi : m_G)
		if (!(gi.second.kind == kState && gi.second.b == gi.first))
			occupied[gi.second]++;
	for (auto& so : m_stateOwner)
		if (isStateMachine(so.first) && occupied[Graph::state(so.second, so.first)] > 0)
			m_wrapped.insert(so.first);

	// state machine states of one state machine whose paths all have one level more than their chain, with the same
	// component there (BT_Ladder|SM_Ladder|SM_LadderIdle, ...|SM_Ladder|SM_LadderFall): sibling states cannot share a
	// name, so the extra level is not a blend tree wrapping each of them but their owner, wrapped in a blend tree state
	// of its own state machine
	std::map<std::pair<int, std::string>, std::vector<int>> extra;
	for (auto& nm : m_named)
	{
		if (!isPlaceable(nm.first) || !isStateRoot(nm.first) || !isStateMachine(nm.first) || m_wrapped.count(nm.first))
			continue;
		std::vector<std::string> comps = splitPath(nm.second);
		if (comps.size() >= 2 && comps.size() == chain(nm.first).size() + 1)
			extra[std::make_pair(m_stateOwner[nm.first], comps[comps.size() - 2])].push_back(nm.first);
	}
	for (auto& e : extra)
	{
		const int sm = e.first.first;
		if (e.second.size() > 1 && isStateRoot(sm) && isStateMachine(sm) && !m_wrapped.count(sm))
		{
			m_wrapped.insert(sm);
			log("state machine %d wrapped in a blend tree state: %d of its states share the level %s", sm, (int)e.second.size(), e.first.second.c_str());
		}
	}

	// names along named paths
	m_smName.clear();
	m_stateName.clear();
	m_nbtName.clear();
	for (auto& nm : m_named)
	{
		if (!isPlaceable(nm.first))
			continue;
		std::vector<Entity> ch = chain(nm.first);
		std::vector<std::string> comps = splitPath(nm.second);
		if (comps.size() != ch.size())
		{
			// a named state whose blend tree wraps the state machine: SubAct|BT_MoveJump|SM_MoveJump
			if (comps.size() == ch.size() + 1 && isStateRoot(nm.first) && isStateMachine(nm.first))
			{
				m_wrapped.insert(nm.first);
				ch = chain(nm.first);
			}
			if (comps.size() != ch.size())
			{
				log("named node %d %s: %d path components but %d structural levels", nm.first, nm.second.c_str(), (int)comps.size(), (int)ch.size());
				continue;
			}
		}
		assignNames(ch, comps);
	}

	// state entries name the container holding the state's root: the blend tree state itself, or for a state machine
	// state the state machine that owns it (or, one level deeper, the blend tree wrapping it)
	for (auto& sn : m_stateNames)
	{
		const MR::NodeID sm = sn.first.first, r = sn.first.second;
		if (!isStateMachine(sm) || !isStateRoot(r) || m_stateOwner[r] != sm)
			continue;
		std::vector<std::string> comps = splitPath(sn.second);
		std::vector<Entity> ch;
		if (isStateMachine(r))
		{
			std::vector<Entity> base = chain(sm);
			if (comps.size() == base.size() + 1)
				m_wrapped.insert(r);
			ch = comps.size() == base.size() ? base : chainOfGraph(Graph::state(sm, r));
		}
		else
			ch = chainOfGraph(Graph::state(sm, r));
		if (comps.size() != ch.size())
		{
			log("state entry %d %s: %d path components but %d structural levels", r, sn.second.c_str(), (int)comps.size(), (int)ch.size());
			continue;
		}
		assignNames(ch, comps);
	}

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
// naming
//----------------------------------------------------------------------------------------------------------------------
std::vector<NetworkHierarchy::Entity> NetworkHierarchy::chain(MR::NodeID id)
{
	std::vector<Entity> out;
	if (isStateMachine(id))
	{
		if (isStateRoot(id) && !m_wrapped.count(id))
			out = chain(m_stateOwner[id]);
		else
			out = chainOfGraph(graph(id));
		Entity e;
		e.kind = Entity::kSM;
		e.id = id;
		out.push_back(e);
		return out;
	}

	out = chainOfGraph(graph(id));
	Entity e;
	e.kind = Entity::kNode;
	e.id = id;
	out.push_back(e);
	return out;
}

std::vector<NetworkHierarchy::Entity> NetworkHierarchy::chainOfGraph(const Graph& g)
{
	if (g.kind == kRoot)
		return {};

	std::vector<Entity> out;
	Entity e;
	e.graph = g;
	if (g.kind == kNested)
	{
		out = chainOfGraph(parentGraph(g));
		e.kind = Entity::kNestedEnt;
	}
	else
	{
		out = chain(g.a);
		e.kind = Entity::kStateEnt;
	}
	out.push_back(e);
	return out;
}

void NetworkHierarchy::assignNames(const std::vector<Entity>& ch, const std::vector<std::string>& comps)
{
	for (size_t i = 0; i < ch.size() && i < comps.size(); i++)
	{
		const Entity& e = ch[i];
		if (e.kind == Entity::kSM)
			m_smName[e.id] = comps[i];
		else if (e.kind == Entity::kStateEnt)
			m_stateName[{ e.graph.a, e.graph.b }] = comps[i];
		else if (e.kind == Entity::kNestedEnt)
			m_nbtName[e.graph] = comps[i];
	}
}

std::string NetworkHierarchy::smLeaf(MR::NodeID sm)
{
	auto it = m_smName.find(sm);
	if (it != m_smName.end())
		return it->second;
	return "StateMachine_" + std::to_string(sm);
}

std::string NetworkHierarchy::stateLeaf(MR::NodeID sm, MR::NodeID r)
{
	auto it = m_stateName.find({ sm, r });
	if (it != m_stateName.end())
		return it->second;
	if (isStateMachine(r) && !m_wrapped.count(r))
		return smLeaf(r);
	return "BlendTree_" + std::to_string(r) + "_0";
}

std::string NetworkHierarchy::graphName(const Graph& g)
{
	if (g.kind == kRoot)
		return "";
	if (g.kind == kState)
		return stateLeaf(g.a, g.b);

	auto it = m_nbtName.find(g);
	if (it != m_nbtName.end())
		return it->second;
	const uint32_t n = getNestingDepth(g.a, g.b) - g.level + (isStateMachine(g.a) ? 1 : 0);
	return "BlendTree_" + std::to_string(g.b) + "_" + std::to_string(n);
}

std::string NetworkHierarchy::leafName(MR::NodeID id)
{
	if (isStateMachine(id))
		return smLeaf(id);

	MR::NodeDef* n = node(id);
	if (n && n->getNodeTypeID() == NODE_TYPE_ANIM_EVENTS && m_animName)
	{
		std::string name = m_animName(n);
		if (!name.empty())
			return name;
	}

	std::string type = m_typeName && n ? m_typeName(n->getNodeTypeID()) : "Node";
	return type + "_" + std::to_string(id);
}

std::string NetworkHierarchy::part(const Entity& e)
{
	switch (e.kind)
	{
	case Entity::kSM:		return smLeaf(e.id);
	case Entity::kStateEnt:
	case Entity::kNestedEnt:	return graphName(e.graph);
	default:			return leafName(e.id);
	}
}

std::string NetworkHierarchy::path(MR::NodeID id)
{
	std::string out;
	for (const Entity& e : chain(id))
	{
		if (!out.empty())
			out += "|";
		out += part(e);
	}
	return out;
}
