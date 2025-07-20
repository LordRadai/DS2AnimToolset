#pragma once
#include "RCore.h"
#include "../Node/Node.h"

namespace db
{
	class NodeDef;

	class FlowEdge : public Node
	{
		NodeDef* m_source;
		NodeDef* m_destination;

	public:
		FlowEdge(Node* parent, NodeDef* from, NodeDef* to, std::string name) : Node(parent, "FlowEdge", name), m_source(from), m_destination(to) {}

		virtual ~FlowEdge() {};
		virtual bool isValid() const { return Node::isValid() && m_source != nullptr && m_destination != nullptr; };
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		NodeDef* getFrom() const { return m_source; }
		NodeDef* getTo() const { return m_destination; }

		void setFrom(NodeDef* from) { m_source = from; }
		void setTo(NodeDef* to) { m_destination = to; }
	};
}