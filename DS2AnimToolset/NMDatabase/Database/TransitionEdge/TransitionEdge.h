#pragma once
#include "../Node/Node.h"
#include "../NodeDef/NodeDef.h"
#include "../NodeContainer/NodeContainer.h"
#include "../Attribute/Attribute.h"
#include "Condition/Condition.h"

namespace db
{
	class TransitionEdge : public Node
	{
		NodeDef* m_source;
		NodeDef* m_destination;
		NodeContainer m_conditions;
		NodeContainer m_attributes;
		std::string m_edgeType;
		int m_manifestVersion;

	protected:
		TransitionEdge(Node* parent, NodeDef* from, NodeDef* to, std::string name, std::string edgeType, int manifestVersion);

	public:
		virtual ~TransitionEdge() {};
		virtual bool isValid() const { return Node::isValid() && m_source != nullptr && m_destination != nullptr; };
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);
	};

	class Transit : public TransitionEdge
	{
	public:
		Transit(Node* parent, NodeDef* from, NodeDef* to, std::string name) : TransitionEdge(parent, from, to, name, "Transit", 4) {};
		virtual ~Transit() {};
		virtual bool isValid() const { return TransitionEdge::isValid(); };
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);
	};

	class TransitMatchEvents : public TransitionEdge
	{
	public:
		TransitMatchEvents(Node* parent, NodeDef* from, NodeDef* to, std::string name) : TransitionEdge(parent, from, to, name, "TransitMatchEvents", 5) {};
		virtual ~TransitMatchEvents() {};
		virtual bool isValid() const { return TransitionEdge::isValid(); };
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);
	};
}
