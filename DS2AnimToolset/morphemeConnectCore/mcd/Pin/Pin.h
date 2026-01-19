#pragma once
#include "NMDatabase/NMDatabase.h"

namespace mcd
{
	class Graph;
	class GraphNode;
	class FlowEdge;
	class BlendTree;

	class DataPin;
	class FunctionalPin;
	class PassDownPin;

	class Pin : public db::Node
	{
	protected:
		static class CycleDetector
		{
		public:
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
		Pin(db::Node* parent, const std::string& name, const std::string pinName);

		virtual ~Pin() override {};
		virtual bool isCompatibleConnectionTarget(Pin* to) { return false; };
		virtual bool canStartConnection();
		virtual bool canStartConnectionInGraph(mcd::Graph* graph);
		virtual bool canReceiveConnection(Pin* from);
		virtual bool canConnectTo(Pin* to);
		virtual bool breakConnectionTo(Pin* to);
		virtual void getPassedThroughFunctionalInterfaces(std::vector<std::string>& outInterfaces) {}

		void setIsInput(bool isInput);
		void setIsArray(bool isArray);
		void setReferenceTarget(const std::string& referenceTarget);
		void setReference(bool isReference);

		bool isInput() const { return m_isInput->getValue(); }
		bool isArray() const { return m_isArray->getValue(); }
		std::string getReferenceTarget() const { return m_referenceTarget->getValue(); }
		bool isReference() const { return m_reference->getValue(); }

		mcd::FlowEdge* connectTo(Pin* to);

		mcd::BlendTree* getParentOrGrandParentBlendTree();
		mcd::Graph* getParentOrGrandParentGraph();

		mcd::Graph* getOwnerGraphForConnection(Pin* to);

		mcd::Graph* getGrandParentGraph();

		bool isConnectedTo(Pin* other);
		bool isDirectlyConnectedTo(Pin* other);

		bool hasIncidentEdge();

		void getEdgesConnectedInGraph(std::vector<mcd::FlowEdge*>& outEdges, mcd::Graph* graph);

		void getConnectedFlowEdges(std::vector<mcd::FlowEdge*>& outEdges);

		mcd::FunctionalPin* asFunctionalPin();
		mcd::DataPin* asDataPin();
		mcd::PassDownPin* asPassDownPin();

		bool containsFunctionalInterfacesFor(Pin* other);
		void getAllFunctionalInterfaces(std::vector<std::string>& outInterfaces);

		void getConnectedPins(std::vector<mcd::Pin*>& outPins);
	};
}