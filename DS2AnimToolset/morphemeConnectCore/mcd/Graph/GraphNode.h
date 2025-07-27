#pragma once
#include "NMDatabase/NMDatabase.h"
#include "NMDBExtensions/TypedNodeContainer.inl"
#include "mcd/Attribute/Attribute.h"
#include "mcd/Pin/Pin.inl"

namespace mcd
{
	class GraphNode : public db::Node
	{
	protected:
		std::unique_ptr<db::TypedNodeContainer<mcd::Attribute>> m_attributes;
		std::unique_ptr<db::TypedNodeContainer<mcd::Pin>> m_pins;
		std::unique_ptr<db::FloatAttribute> m_xPos;
		std::unique_ptr<db::FloatAttribute> m_yPos;
		std::unique_ptr<db::FloatAttribute> m_width;
		std::unique_ptr<db::FloatAttribute> m_height;

		GraphNode(Attribute* parent, std::string name, std::string nodeName, float xPos, float yPos, float width, float height);
	public:		
		virtual ~GraphNode() override;
		
		const mcd::Attribute* getAttribute(uint32_t index) const { return m_attributes->getNode(index); }
		const mcd::Attribute* findAttribute(const std::string& name) const { m_attributes->find(name); }
		const mcd::Pin* getPin(uint32_t index) const { return m_pins->getNode(index); }
		const mcd::Pin* findPin(const std::string& name) const { return m_pins->find(name); }

		float getXPos() const { return m_xPos->getValue(); }
		float getYPos() const { return m_yPos->getValue(); }
		float getWidth() const { return m_width->getValue(); }
		float getHeight() const { return m_height->getValue(); }

		void setXPos(float x) { m_xPos->setValue(x); }
		void setYPos(float y) { m_yPos->setValue(y); }
		void setWidth(float width) { m_width->setValue(width); }
		void setHeight(float height) { m_height->setValue(height); }
	};
}
