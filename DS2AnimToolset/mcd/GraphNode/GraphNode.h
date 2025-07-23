#pragma once
#include "Node/Node.h"
#include "NodeContainer/NodeContainer.h"
#include "Attribute/Attribute.h"

namespace mcd
{
	class GraphNode : public db::Node
	{
	protected:
		db::NodeContainer m_attributes;
		db::NodeContainer m_pins;
		float m_xpos;
		float m_ypos;
		float m_width;
		float m_height;
		std::string m_type;
		uint32_t m_version;

		// Users should never use this base class. A node can either be created as a BlendTreeNode or a StateMachineNode.
		GraphNode(db::Node* parent, std::string identifier, std::string label, std::string type, uint32_t version) :
			db::Node(parent, identifier, label),
			m_type(type),
			m_version(version),
			m_attributes(this, "Attributes"),
			m_pins(this, "Pins")
		{
		};
	public:

		virtual ~GraphNode() override {};
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent) override;

		std::string getType() const { return m_type; };
		uint32_t getVersion() const { return m_version; };

		void setPosition(float x, float y) { m_xpos = x; m_ypos = y; };
		float getXPos() const { return m_xpos; };
		float getYPos() const { return m_ypos; };

		void setSize(float width, float height) { m_width = width; m_height = height; };
		float getWidth() const { return m_width; };
		float getHeight() const { return m_height; };

		void addAttribute(db::Node* attribute) { m_attributes.addNode(attribute); }
		Attribute* getAttribute(int idx) const { return dynamic_cast<Attribute*>(m_attributes.getNode(idx)); }
		Attribute* findAttribute(std::string name) const { return dynamic_cast<Attribute*>(m_attributes.findNode(name)); }
		size_t getNumAttributes() const { return m_attributes.getNumNodes(); }

		BoolAttribute* getBoolAttribute(std::string name) const { return dynamic_cast<BoolAttribute*>(findAttribute(name)); }
		FloatAttribute* getFloatAttribute(std::string name) const { return dynamic_cast<FloatAttribute*>(findAttribute(name)); }
		IntAttribute* getIntAttribute(std::string name) const { return dynamic_cast<IntAttribute*>(findAttribute(name)); }
		AnimationSetAttribute* getAnimationSetAttribute(std::string name) const { return dynamic_cast<AnimationSetAttribute*>(findAttribute(name)); }
	};
}