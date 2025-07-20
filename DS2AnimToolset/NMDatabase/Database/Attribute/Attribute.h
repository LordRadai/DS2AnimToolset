#pragma once
#include "RCore.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMMatrix.h"
#include "NMPlatform/NMMatrix34.h"
#include "../Node/Node.h"

namespace db
{
	class Attribute : public Node
	{
	protected:
		std::string m_type;
		bool m_bPerAnimSet;

		Attribute(Node* parent, std::string name, std::string type, bool bPerAnimSet) : Node(parent, type, name), m_type(type), m_bPerAnimSet(bPerAnimSet) {};
		virtual ~Attribute() {};

	public:
		virtual bool isValid() const;
		virtual tinyxml2::XMLElement* serialize();

		std::string getType() const { return m_type; };
		bool isPerAnimSet() const { return m_bPerAnimSet; };
	};

	class BoolAttribute : public Attribute
	{
		bool m_value;

	public:
		BoolAttribute(Node* parent, std::string name, bool value) : Attribute(parent, name, "BoolAttribute", false), m_value(value) {};
		
		virtual ~BoolAttribute() {};
		virtual bool isValid() const;
		virtual tinyxml2::XMLElement* serialize();

		bool getValue() const { return m_value; };
		void setValue(bool value) { m_value = value; };
	};

	class FloatAttribute : public Attribute
	{
		float m_value;
	public:
		FloatAttribute(Node* parent, std::string name, float value) : Attribute(parent, name, "FloatAttribute", false), m_value(value) {};
		
		virtual ~FloatAttribute() {};
		virtual bool isValid() const;
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		float getValue() const { return m_value; };
		void setValue(float value) { m_value = value; };
	};
}