#pragma once
#include "../../Node/Node.h"
#include "../../Attribute/Attribute.h"
#include "../../NodeContainer/NodeContainer.h"

namespace db
{
	class Condition : public Node
	{
		NodeContainer m_attributes;

	protected:
		std::string m_type;
		int m_manifestVersion;

	public:
		Condition(Node* parent, std::string name, std::string type, int manifestVersion) : Node(parent, "Condition", name), m_type(type), m_manifestVersion(manifestVersion), m_attributes(this, "Attributes") {}
		
		virtual ~Condition() {};
		virtual bool isValid() const { return Node::isValid() && !m_type.empty(); };
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		std::string getType() const { return m_type; }
		int getManifestVersion() const { return m_manifestVersion; }

		void addAttribute(Attribute* attribute) { m_attributes.addNode(attribute); }
		Attribute* getAttribute(int index) const;
		Attribute* findAttribute(std::string name) const;
		size_t getNumAttributes() const { return m_attributes.getNumNodes(); }
	};
}