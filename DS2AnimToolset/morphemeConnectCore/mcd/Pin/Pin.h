#pragma once
#include "NMDatabase/NMDatabase.h"

namespace mcd
{
	class Graph;
	class GraphNode;
	class FlowEdge;

	class Pin : public db::Node
	{
		static class CycleDetector
		{
			friend class mcd::Pin;

			static bool wouldCreateCycle(mcd::Pin* from, mcd::Pin* to);
			static mcd::GraphNode* findTreeRoot(mcd::GraphNode* node);
			static bool depthFirstSearchCycleDetect(mcd::GraphNode* currentNode, mcd::GraphNode* targetNode, std::set<mcd::GraphNode*>& visitedNodes);
			static bool depthFirstSearchRemoveFromList(mcd::GraphNode* currentNode, mcd::GraphNode* targetNode, std::vector<mcd::GraphNode*>& visitedNodes);
		};

		std::unique_ptr<db::BoolAttribute> m_isInput;
		std::unique_ptr<db::BoolAttribute> m_isArray;
		std::unique_ptr<db::StringAttribute> m_referenceTarget;
		std::unique_ptr<db::BoolAttribute> m_reference;

	public:
		Pin(db::Node* parent, const std::string& name, const std::string pinName)
			: db::Node(parent, name, pinName), 
			m_reference(std::make_unique<db::BoolAttribute>(this, "Reference", false)) {};

		virtual ~Pin() override {};
		virtual bool isCompatibleConnectionTarget(Pin* to) { return false; };
		virtual bool canStartConnection();
		virtual bool canStartConnectionInGraph(mcd::Graph* graph);
		virtual bool canReceiveConnection(Pin* from);
		virtual bool canConnectTo(Pin* to);
		virtual bool breakConnectionTo(Pin* to);

		void setIsInput(bool isInput);
		void setIsArray(bool isArray);
		void setReferenceTarget(const std::string& referenceTarget);
		void setReference(bool isReference);

		bool getIsInput() const { return m_isInput->getValue(); }
		bool getIsArray() const { return m_isArray->getValue(); }
		std::string getReferenceTarget() const { return m_referenceTarget->getValue(); }
		bool getIsReference() const { return m_reference->getValue(); }

		mcd::FlowEdge* connectTo(Pin* to);

		mcd::Graph* getParentOrGrandParentGraph();
		mcd::Graph* getOwnerGraphForConnection(Pin* to);

		bool isConnectedTo(Pin* other);
		bool isDirectlyConnectedTo(Pin* other);
	};
}