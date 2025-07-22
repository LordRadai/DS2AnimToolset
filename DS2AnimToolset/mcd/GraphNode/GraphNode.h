#pragma once
#include "Node/Node.h"
#include "NodeContainer/NodeContainer.h"

namespace mcd
{
	class GraphNode : public db::Node
	{
		db::NodeContainer m_attributes;
		db::NodeContainer m_pins;
		float m_xpos;
		float m_ypos;
		float m_width;
		float m_height;
		std::string m_type;
		uint32_t m_version;

	public:
		GraphNode(db::Node* parent, std::string identifier, std::string label, std::string type, uint32_t version) :
			db::Node(parent, identifier, label), 
			m_type(type), 
			m_version(version),
			m_attributes(this, "Attributes"), 
			m_pins(this, "Pins") 
		{};

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
	};
}