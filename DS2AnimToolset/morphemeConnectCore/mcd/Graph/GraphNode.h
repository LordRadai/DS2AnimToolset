#pragma once
#include "mcd/AttributePinNodeBase/AttributePinNodeBase.h"
#include "NMDBExtensions/TypedNodeContainer.inl"
#include "mcd/Attribute/Attribute.h"
#include "mcd/Pin/Pin.h"

namespace mcd
{
	class Graph;

	class GraphNode : public mcd::AttributePinNodeBase
	{
	protected:
		std::unique_ptr<db::NodeContainer> m_graphEntry;
		std::unique_ptr<db::TypedNodeContainer<mcd::Attribute>> m_attributes;
		std::unique_ptr<db::TypedNodeContainer<mcd::Pin>> m_pins;
		std::unique_ptr<db::FloatAttribute> m_xPos;
		std::unique_ptr<db::FloatAttribute> m_yPos;
		std::unique_ptr<db::FloatAttribute> m_width;
		std::unique_ptr<db::FloatAttribute> m_height;
		std::unique_ptr<db::TypedNodeContainer<mcd::Graph>> m_ownerGraphs;

		GraphNode(db::CompositeAttribute* parent, std::string name, std::string nodeName, float xPos, float yPos, float width, float height);
	public:		
		virtual ~GraphNode() override {}
		virtual mcd::Pin* getPin(int index) override { return m_pins->getNode(index); }
		virtual mcd::Pin* getPin(const std::string& name) override { return m_pins->find(name); }

		db::TypedNodeContainer<mcd::Attribute>* getAttributes() const { return m_attributes.get(); }
		db::TypedNodeContainer<mcd::Pin>* getPins() const { return m_pins.get(); }

		void addMcdAttribute(mcd::Attribute* attribute);
		mcd::Attribute* getAttribute(uint32_t index) const { return m_attributes->getNode(index); }
		mcd::Attribute* findAttribute(const std::string& name) const { return m_attributes->find(name); }

		void addPin(mcd::Pin* pin);

		float getXPos() const { return m_xPos->getValue(); }
		float getYPos() const { return m_yPos->getValue(); }
		float getWidth() const { return m_width->getValue(); }
		float getHeight() const { return m_height->getValue(); }

		void setXPos(float x) { m_xPos->setValue(x); }
		void setYPos(float y) { m_yPos->setValue(y); }
		void setWidth(float width) { m_width->setValue(width); }
		void setHeight(float height) { m_height->setValue(height); }

		db::NodeContainer* getGraphEntry() const { return m_graphEntry.get(); }
		void setGraphEntry(db::Node* node);

		db::Node* getGraphEntryNode() const;

		mcd::Graph* getGraph();
		void setOwnerGraph(mcd::Graph* graph);
	};
}
