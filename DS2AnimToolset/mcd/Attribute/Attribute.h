#pragma once
#include "RCore.h"
#include "Node/Node.h"
#include "NodeContainer/NodeContainer.h"

namespace mcd
{
	class Attribute : public db::Node
	{
	protected:
		std::string m_type;

		Attribute(db::Node* parent, std::string name, std::string type) : db::Node(parent, type, name), m_type(type) {};

	public:
		virtual ~Attribute() {};

		std::string getType() const { return m_type; };
	};

	class AttributePlaceholder : public Attribute
	{
		Attribute* m_attribute;

	public:
		AttributePlaceholder(db::Node* parent, std::string name, Attribute* attribute) : Attribute(parent, name, "AttributePlaceholder"), m_attribute(attribute) {};

		virtual ~AttributePlaceholder() override {};
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent) override;

		Attribute* getAttribute() const { return m_attribute; };
		void setAttribute(Attribute* attribute);
	};

	class AnimationSetAttribute : public Attribute
	{
		db::NodeContainer m_attributePlaceholders;
		std::string m_targetAnimSet;

		void addAttributePlaceholder(AttributePlaceholder* placeholder) { m_attributePlaceholders.addNode(placeholder); }
		AttributePlaceholder* getAttributePlaceholder(int index) const { return dynamic_cast<AttributePlaceholder*>(m_attributePlaceholders.getNode(index)); }
		size_t getNumAttributePlaceholders() const { return m_attributePlaceholders.getNumNodes(); }
	public:
		AnimationSetAttribute(db::Node* parent, std::string name, std::string targetAnimSet) : Attribute(parent, name, "AnimationSetAttribute"), m_targetAnimSet(targetAnimSet), m_attributePlaceholders(this, "AttributePlaceholders") {};

		virtual ~AnimationSetAttribute() override {};
		virtual bool isValid() const override { return Attribute::isValid() && getNumAttributePlaceholders() == 1; } // AnimationSetAttribute should always have exactly one AttributePlaceholder, containing one Attribute.
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent) override;

		void setAttribute(Attribute* attribute);
		Attribute* getAttribute() const { return getAttributePlaceholder(0)->getAttribute(); }
	};

	class BoolAttribute : public Attribute
	{
		bool m_value;

	public:
		BoolAttribute(db::Node* parent, std::string name, bool value) : Attribute(parent, name, "BoolAttribute"), m_value(value) {};

		virtual ~BoolAttribute() override {};
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* element) override;

		bool getValue() const { return m_value; };
		void setValue(bool value) { m_value = value; };
	};

	class FloatAttribute : public Attribute
	{
		float m_value;

	public:
		FloatAttribute(db::Node* parent, std::string name, float value) : Attribute(parent, name, "FloatAttribute"), m_value(value) {};

		virtual ~FloatAttribute() override {};
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent) override;

		float getValue() const { return m_value; };
		void setValue(float value) { m_value = value; };
	};

	class IntAttribute : public Attribute
	{
		int m_value;

	public:
		IntAttribute(db::Node* parent, std::string name, int value) : Attribute(parent, name, "IntAttribute"), m_value(value) {};

		virtual ~IntAttribute() override {};
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent) override;

		int getValue() const { return m_value; };
		void setValue(int value) { m_value = value; }
	};
}