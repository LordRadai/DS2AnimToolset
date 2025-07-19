#pragma once
#include "RCore.h"

namespace db
{
	class Attribute
	{
	protected:
		enum class AttributeType
		{
			kInt,
			kFloat,
			kBool,
			kVector3,
			kVector4,
			kQuaternion,
			kAnimationTake,
			kAnimationSet
		};

		std::string m_name;
		AttributeType m_type;

		Attribute(std::string name, AttributeType type)
			: m_name(name), m_type(type) {
		};

		~Attribute() {};

	public:
		std::string getName() const { return m_name; };
		void setName(std::string name) { m_name = name; };
		AttributeType getType() const { return m_type; };
		void setType(AttributeType type) { m_type = type; };
	};

	class AnimationSetAttribute : public Attribute
	{
		Attribute* m_attribute;

	public:
		AnimationSetAttribute(std::string name, Attribute* attribute)
			: Attribute(name, AttributeType::kAnimationSet), m_attribute(attribute) {
		};

		~AnimationSetAttribute() {};

		Attribute* getAttribute() const { return m_attribute; };
		void setAttribute(Attribute* attribute) { m_attribute = attribute; };
	};

	class BoolAttribute : public Attribute
	{
		bool m_value;

	public:
		BoolAttribute(std::string name, bool value)
			: Attribute(name, AttributeType::kBool), m_value(value) {
		};
		~BoolAttribute() {};
		bool getValue() const { return m_value; };
		void setValue(bool value) { m_value = value; };
	};

	class FloatAttribute : public Attribute
	{
		float m_value;
	public:
		FloatAttribute(std::string name, float value)
			: Attribute(name, AttributeType::kFloat), m_value(value) {
		};
		~FloatAttribute() {};
		float getValue() const { return m_value; };
		void setValue(float value) { m_value = value; };
	};
}