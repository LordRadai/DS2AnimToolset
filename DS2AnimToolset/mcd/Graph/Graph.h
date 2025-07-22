#pragma once
#include "Node/Node.h"
#include "Attribute/Attribute.h"
#include "NodeContainer/NodeContainer.h"
#include "GraphNode/GraphNode.h"

namespace mcd
{
	class Graph : public db::Node
	{
	protected:
		float m_panX;
		float m_panY;
		std::string m_type;
		uint32_t m_version;

	public:
		Graph(db::Node* parent, std::string identifier, std::string label, std::string type, uint32_t version) :
			Node(parent, identifier, label),
			m_type(type),
			m_version(version)
		{};
		
		virtual ~Graph() override {};
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent) override;

		void setPan(float x, float y) { m_panX = x; m_panY = y; };
		float getPanX() const { return m_panX; };
		float getPanY() const { return m_panY; };
		std::string getType() const { return m_type; };
		uint32_t getVersion() const { return m_version; };
	};
}
