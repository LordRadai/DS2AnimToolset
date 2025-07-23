#pragma once
#include "Attribute/Attribute.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMQuat.h"

namespace db
{
	class SingleValueAttribute : public Attribute
	{
	public:
		SingleValueAttribute(Attribute* parent, std::string name, std::string label, std::string type)
			: Attribute(parent, name, label, type) {};

		virtual ~SingleValueAttribute() override {};
		virtual bool getValueAsBool() const = 0;
		virtual int getValueAsEnum() const = 0;
		virtual int getValueAsInt() const = 0;
		virtual float getValueAsFloat() const = 0;
		virtual double getValueAsDouble() const = 0;
		virtual std::string getValueAsString() const = 0;
		virtual NMP::Vector3 getValueAsVector3() const = 0;
		virtual NMP::Matrix34 getValueAsMatrix34() const = 0;
		virtual NMP::Quat getValueAsQuaternion() const = 0;
	};
}
