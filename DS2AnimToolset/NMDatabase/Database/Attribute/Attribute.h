#pragma once
#include "RCore.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMMatrix.h"
#include "NMPlatform/NMMatrix34.h"
#include "../Node/Node.h"
#include "../NodeContainer/NodeContainer.h"

namespace db
{
	class Request;
	class ControlParameter;

	class Attribute : public Node
	{
	protected:
		std::string m_type;

		Attribute(Node* parent, std::string name, std::string type) : Node(parent, type, name), m_type(type) {};
		virtual ~Attribute() {};

	public:
		virtual bool isValid() const;

		std::string getType() const { return m_type; };
	};

	class AttributePlaceholder : public Attribute
	{
		Attribute* m_attribute;

	public:
		AttributePlaceholder(Node* parent, std::string name, Attribute* attribute) : Attribute(parent, name, "AttributePlaceholder"), m_attribute(attribute) {};
		
		virtual ~AttributePlaceholder() {};
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		Attribute* getAttribute() const { return m_attribute; };
		void setAttribute(Attribute* attribute) { m_attribute = attribute; };
	};

	class AnimationSetAttribute : public Attribute
	{
		NodeContainer m_attributePlaceholders;

	public:
		AnimationSetAttribute(Node* parent, std::string name) : Attribute(parent, name, "AnimationSetAttribute"), m_attributePlaceholders(this, "AttributePlaceholders") {};
		
		virtual ~AnimationSetAttribute() {};
		virtual bool isValid() const { return Attribute::isValid() && getNumAttributePlaceholders() == 1; } // AnimationSetAttribute should always have exactly one AttributePlaceholder, containing one Attribute.
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		void addAttributePlaceholder(AttributePlaceholder* placeholder) { m_attributePlaceholders.addNode(placeholder); }
		AttributePlaceholder* getAttributePlaceholder(int index) const { return dynamic_cast<AttributePlaceholder*>(m_attributePlaceholders.getNode(index)); }
		size_t getNumAttributePlaceholders() const { return m_attributePlaceholders.getNumNodes(); }

		Attribute* getAttribute() const { return getAttributePlaceholder(0)->getAttribute(); }
	};

	class BoolAttribute : public Attribute
	{
		bool m_value;

	public:
		BoolAttribute(Node* parent, std::string name, bool value) : Attribute(parent, name, "BoolAttribute"), m_value(value) {};
		
		virtual ~BoolAttribute() {};
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* element);

		bool getValue() const { return m_value; };
		void setValue(bool value) { m_value = value; };
	};

	class FloatAttribute : public Attribute
	{
		float m_value;
	public:
		FloatAttribute(Node* parent, std::string name, float value) : Attribute(parent, name, "FloatAttribute"), m_value(value) {};
		
		virtual ~FloatAttribute() {};
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		float getValue() const { return m_value; };
		void setValue(float value) { m_value = value; };
	};

	class IntAttribute : public Attribute
	{
		int m_value;

	public:
		IntAttribute(Node* parent, std::string name, int value) : Attribute(parent, name, "IntAttribute"), m_value(value) {};

		virtual ~IntAttribute() {};
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		int getValue() const { return m_value; };
		void setValue(int value) { m_value = value; }
	};

	class RequestAttribute : public Attribute
	{
		Request* m_request;

	public:
		RequestAttribute(Node* parent, std::string name, Request* request) : Attribute(parent, name, "RequestAttribute"), m_request(request) {};

		virtual ~RequestAttribute() {};
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		Request* getRequest() const { return m_request; };
	};

	class ControlParameterAttribute : public Attribute
	{
		ControlParameter* m_controlParameter;

	public:
		ControlParameterAttribute(Node* parent, std::string name, ControlParameter* controlParameter) : Attribute(parent, name, "ControlParameterAttribute"), m_controlParameter(controlParameter) {}

		virtual ~ControlParameterAttribute() {};
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		ControlParameter* getControlParameter() const { return m_controlParameter; };
	};

	class RefAttribute : public Attribute
	{
		int m_refKind;

	public:
		RefAttribute(Node* parent, std::string name, int refKind) : Attribute(parent, name, "RefAttribute"), m_refKind(refKind) {};

		virtual ~RefAttribute() {};
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		int getRefKind() const { return m_refKind; };
		void setRefKind(int refKind) { m_refKind = refKind; }
	};
}