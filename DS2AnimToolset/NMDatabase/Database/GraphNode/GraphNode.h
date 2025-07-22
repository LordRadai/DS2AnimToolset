#pragma once
#include "RCore.h"
#include "../Node/Node.h"
#include "../NodeContainer/NodeContainer.h"
#include "../Attribute/Attribute.h"
#include "../Pin/Pin.h"

namespace db
{
	class GraphNode : public Node
	{
	protected:
		uint32_t m_nodeType;
		int m_manifestVersion;
		NodeContainer m_attributes;
		NodeContainer m_pins;

		GraphNode(Node* parent, std::string name, std::string identifier, uint32_t nodeType, int manifestVersion) : Node(parent, identifier, name), m_nodeType(nodeType), m_manifestVersion(manifestVersion) {};
		
		virtual ~GraphNode() {};

	public:
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		uint32_t getNodeType() const { return m_nodeType; }
		int getManifestVersion() const { return m_manifestVersion; }

		Attribute* getAttribute(const std::string& name) const { return dynamic_cast<Attribute*>(m_attributes.findNodeByName(name)); }
		void addAttribute(Attribute* attribute) { m_attributes.addNode(attribute); }

		Pin* getPin(const std::string& name) const { return dynamic_cast<Pin*>(m_pins.findNodeByName(name)); }
		void addPin(Pin* pin) { m_pins.addNode(pin); }
	};
}