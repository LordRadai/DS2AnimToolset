#include "Graph.h"
#include "mcd/Network/Network.h"

namespace mcd
{
	Graph::Graph(CompositeAttribute* parent, std::string name, std::string graphName)
		: Node(parent, name, graphName),
		m_flowEdges(std::make_unique<db::TypedNodeContainer<mcd::FlowEdge>>(this, "FlowEdges")),
		m_panX(std::make_unique<db::FloatAttribute>(this, "PanX", 0.0f)),
		m_panY(std::make_unique<db::FloatAttribute>(this, "PanY", 0.0f))
	{
	}

	void Graph::updateCommonPinForAncestorStateMachine()
	{
		db::Node* parentNode = getParentNode();

		while (parentNode != nullptr && parentNode->isOfType<mcd::GraphNode>())
		{
			mcd::StateMachine* parentSM = static_cast<mcd::StateMachine*>(parentNode->getParentNode());

			if (parentSM->isOfType<mcd::StateMachine>())
			{
				parentSM->updateCommonPin();
				return;
			}

			parentNode = parentNode->getParentNode();
		}
	}

	void Graph::setPan(float x, float y)
	{
		removeAttribute(m_panX.get());
		removeAttribute(m_panY.get());

		m_panX->setValue(x);
		m_panY->setValue(y);

		if (x != 0.f)
			addAttribute(m_panX.get());

		if (y != 0.f)
			addAttribute(m_panY.get());
	}

	void Graph::removeFlowEdge(mcd::FlowEdge* edge)
	{
		m_flowEdges->remove(edge);

		if (m_flowEdges->size() == 0)
			removeAttribute(m_flowEdges.get());
	}

	void Graph::addFlowEdge(mcd::FlowEdge* edge)
	{
		m_flowEdges->add(edge);

		if (findAttribute("FlowEdges") == nullptr)
			insertAttribute(0, m_flowEdges.get());
	}

	bool Graph::removeConnection(mcd::Edge* edge)
	{
		for (size_t i = 0; i < m_flowEdges->size(); i++)
		{
			mcd::FlowEdge* currentEdge = m_flowEdges->getNode(i);

			if (currentEdge == edge)
			{
				mcd::Pin* toPin = currentEdge->getDstPin();

				removeFlowEdge(currentEdge);

				mcd::Graph* parentGraph = toPin->getParentOrGrandParentGraph();

				if (parentGraph)
					parentGraph->updateCommonPinForAncestorStateMachine();

				return true;
			}
		}
	}

	bool Graph::removeConnection(mcd::Pin* scr, mcd::Pin* dst)
	{
		for (size_t i = 0; i < m_flowEdges->size(); i++)
		{
			FlowEdge* currentEdge = m_flowEdges->getNode(i);

			if (currentEdge->getSrcPin() == scr && currentEdge->getDstPin() == dst)
			{
				removeFlowEdge(currentEdge);

				mcd::Graph* parentGraph = dst->getParentOrGrandParentGraph();

				if (parentGraph)
					parentGraph->updateCommonPinForAncestorStateMachine();

				return true;
			}
		}

		return false;
	}

	bool Graph::isAncestorOf(mcd::AttributePinNodeBase* node)
	{
		mcd::AttributePinNodeBase* currentNode = node;

		do
		{
			if (currentNode == nullptr || currentNode->isOfType<mcd::Network>())
				return false;

			if (node->hasParentNode<mcd::AttributePinNodeBase>())
				currentNode = dynamic_cast<mcd::AttributePinNodeBase*>(currentNode->getParentNode());
			else
				currentNode = nullptr;

		} while (reinterpret_cast<mcd::Graph*>(currentNode) != this);

		return true;
	}
}
