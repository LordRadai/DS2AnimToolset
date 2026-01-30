#pragma once
#include "NodeEditor/Node/Node.h"

namespace NodeEditor
{
	class Node;

	class Transition : public Node
	{
		Node* m_sourceNode;
		Node* m_destinationNode;

	public:
		Transition(NodeEditor* editor, Graph* parent, int nodeID, Node* sourceNode, Node* destinationNode);
		virtual ~Transition() override {}

		virtual void draw() override;

		Node* getSourceNode() const { return m_sourceNode; }
		Node* getDestinationNode() const { return m_destinationNode; }
	};
}
