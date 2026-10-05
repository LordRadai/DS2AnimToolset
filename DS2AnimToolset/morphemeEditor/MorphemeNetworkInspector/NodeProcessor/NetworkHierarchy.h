#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>
#include <functional>

#include "morpheme/mrNetworkDef.h"

/*
* Recovers the morphemeConnect hierarchy (which blend tree / state machine every node lives in) from a compiled
* network. Port of Tools/mcnGen/hierarchy.py, which round-trips DS2's networks node for node.
*
* Rules:
*  - state machines list their states' root nodes; a state is a blend tree (identified by its output node) or a
*    state machine
*  - a node with one consumer lives in its consumer's graph (its downstreamParentID)
*  - a multiply connected node N lives next to its requester P (its downstreamParentID). Connect makes the requester
*    whatever the pass-down container's result feeds, so N's consumers sit in a blend tree nested in P's graph whose
*    output node R is P's input; chains of such nodes with the same requester nest one level per link.
*    When the requester is a state machine, N lives in the state holding its consumers; if N also feeds a node of
*    that state directly, the rest of the state is a blend tree nested in it.
*  - operators (CP output nodes) live in the lowest graph above all their consumers
*  - names (the NMB string table and its state entries) win over these rules
*/
class NetworkHierarchy
{
public:
	enum GraphKind : uint8_t { kRoot = 0, kState, kNested };

	// A graph: the root blend tree, a state (sm, root), or a nested blend tree (requester, output node, level 1 = outermost)
	struct Graph
	{
		GraphKind kind = kRoot;
		MR::NodeID a = 0, b = 0;
		uint32_t level = 0;

		static Graph root() { return Graph(); }
		static Graph state(MR::NodeID sm, MR::NodeID r) { Graph g; g.kind = kState; g.a = sm; g.b = r; return g; }
		static Graph nested(MR::NodeID p, MR::NodeID r, uint32_t l) { Graph g; g.kind = kNested; g.a = p; g.b = r; g.level = l; return g; }

		bool operator<(const Graph& o) const
		{
			if (kind != o.kind) return kind < o.kind;
			if (a != o.a) return a < o.a;
			if (b != o.b) return b < o.b;
			return level < o.level;
		}
		bool operator==(const Graph& o) const { return kind == o.kind && a == o.a && b == o.b && level == o.level; }
		bool operator!=(const Graph& o) const { return !(*this == o); }
	};

	// A structural entity on a node's path: a state machine, a state, a nested blend tree or the node itself
	struct Entity
	{
		enum Kind : uint8_t { kSM, kStateEnt, kNestedEnt, kNode } kind;
		Graph graph;       // for kStateEnt / kNestedEnt
		MR::NodeID id = 0; // for kSM / kNode
	};

	explicit NetworkHierarchy(MR::NetworkDef* netDef, std::function<std::string(MR::NodeDef*)> animName = nullptr,
		std::function<std::string(MR::NodeType)> typeName = nullptr);

	bool build();

	MR::NetworkDef* getNetworkDef() const { return m_netDef; }
	MR::NodeID getRootNodeID() const { return m_rootID; }

	bool isPlaced(MR::NodeID id) const { return m_G.find(id) != m_G.end(); }
	const Graph& getGraph(MR::NodeID id) const { return m_G.at(id); }
	Graph parentGraph(const Graph& g);

	// state root that is a state machine shown directly as the state (no wrapping blend tree)
	bool isUnwrappedStateMachineState(MR::NodeID id) const { return isStateMachine(id) && isStateRoot(id) && !m_wrapped.count(id); }
	bool isStateRoot(MR::NodeID id) const { return m_stateOwner.count(id) != 0; }
	MR::NodeID getStateOwner(MR::NodeID id) const { return m_stateOwner.at(id); }
	bool isStateMachine(MR::NodeID id) const;

	uint32_t getNestingDepth(MR::NodeID p, MR::NodeID r) const;
	const std::map<std::pair<MR::NodeID, MR::NodeID>, uint32_t>& getNestings() const { return m_nbtDepth; }

	std::string leafName(MR::NodeID id);
	std::string graphName(const Graph& g);
	std::string path(MR::NodeID id);

	const std::vector<std::string>& getLog() const { return m_log; }

private:
	MR::NetworkDef* m_netDef;
	MR::NodeID m_rootID;
	std::function<std::string(MR::NodeDef*)> m_animName;
	std::function<std::string(MR::NodeType)> m_typeName;

	std::map<MR::NodeID, MR::NodeID> m_stateOwner;                 // state root -> owning state machine
	std::map<MR::NodeID, std::vector<MR::NodeID>> m_states;         // state machine -> state roots
	std::map<MR::NodeID, std::vector<MR::NodeID>> m_consumers;      // node -> nodes taking it as an input
	std::map<MR::NodeID, std::vector<MR::NodeID>> m_inputs;         // node -> its inputs (child nodes and CP sources)
	std::map<MR::NodeID, std::string> m_named;                      // node -> full path from the string table
	std::map<std::pair<MR::NodeID, MR::NodeID>, std::string> m_stateNames; // (sm, state root) -> container path

	std::map<MR::NodeID, Graph> m_G;
	std::map<MR::NodeID, Graph> m_override;
	std::map<std::pair<MR::NodeID, MR::NodeID>, uint32_t> m_nbtDepth;
	std::set<std::pair<MR::NodeID, MR::NodeID>> m_noNest;
	std::set<MR::NodeID> m_wrapped;

	std::map<MR::NodeID, std::string> m_smName;
	std::map<std::pair<MR::NodeID, MR::NodeID>, std::string> m_stateName;
	std::map<Graph, std::string> m_nbtName;

	std::vector<std::string> m_log;

	MR::NodeDef* node(MR::NodeID id) const;
	bool exists(MR::NodeID id) const { return node(id) != nullptr; }
	bool isCP(MR::NodeID id) const;
	bool isTransition(MR::NodeID id) const;
	bool isNetwork(MR::NodeID id) const;
	bool isOperator(MR::NodeID id) const;
	bool isMultiplyConnected(MR::NodeID id) const;
	bool isPlaceable(MR::NodeID id) const;

	Graph graph(MR::NodeID id, std::set<MR::NodeID>& stack);
	Graph graph(MR::NodeID id) { std::set<MR::NodeID> s; return graph(id, s); }
	std::vector<Graph> ancestors(const Graph& g);
	bool within(const Graph& g, const Graph& outer);
	Graph lca(const std::vector<Graph>& gs);

	void nestedContainers();
	bool stateNesting();
	std::set<MR::NodeID> contradictedMembers();
	void compactNesting(std::pair<MR::NodeID, MR::NodeID> key);
	std::set<std::pair<MR::NodeID, MR::NodeID>> contradictedNesting();

	std::vector<Entity> chain(MR::NodeID id);
	std::vector<Entity> chainOfGraph(const Graph& g);
	void assignNames(const std::vector<Entity>& chain, const std::vector<std::string>& comps);
	std::string part(const Entity& e);
	std::string smLeaf(MR::NodeID sm);
	std::string stateLeaf(MR::NodeID sm, MR::NodeID r);

	void log(const char* fmt, ...);
};
