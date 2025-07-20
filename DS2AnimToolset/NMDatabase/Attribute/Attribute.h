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

		Attribute(Node* parent, std::string name, std::string type, bool bPerAnimSet) : Node(parent, name), m_type(type), m_bPerAnimSet(bPerAnimSet) {};
		~Attribute() {};

	public:
		std::string getName() const { return m_name; };
		std::string getType() const { return m_type; };
		bool isPerAnimSet() const { return m_bPerAnimSet; };
	};

	class BoolAttribute : public Attribute
	{
		bool m_value;

	public:
		BoolAttribute(Node* parent, std::string name, bool value) : Attribute(parent, name, "bool", false), m_value(value) {};
		~BoolAttribute() {};
		bool getValue() const { return m_value; };
		void setValue(bool value) { m_value = value; };
	};

	class FloatAttribute : public Attribute
	{
		float m_value;
	public:
		FloatAttribute(Node* parent, std::string name, float value) : Attribute(parent, name, "float", false), m_value(value) {};
		~FloatAttribute() {};

		float getValue() const { return m_value; };
		void setValue(float value) { m_value = value; };
	};
}