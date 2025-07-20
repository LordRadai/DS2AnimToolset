#pragma once
#include "RCore.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMMatrix.h"
#include "NMPlatform/NMMatrix34.h"
#include "../Pin/Pin.h"
#include "../Node/Node.h"

namespace db
{
	class ControlParameter : public Node
	{
		friend class Network;

	protected:
		std::string m_type;
		DataPin m_dataPin;

		ControlParameter(Node* parent, std::string name, std::string type) : Node(parent, name), m_type(type), m_dataPin(this, "Result", type) {};
		~ControlParameter() {};

	public:
		std::string getType() const { return m_type; };
	};

	class ControlParameterBool : public ControlParameter
	{
		bool m_value;

	public:
		ControlParameterBool(Node* parent, std::string name, bool value) : ControlParameter(parent, name, "bool"), m_value(value) {};
		~ControlParameterBool() {};

		bool getValue() const { return m_value; };
		void setValue(bool value) { m_value = value; };
	};

	class ControlParameterInt : public ControlParameter
	{
		int m_value;
		int m_min;
		int m_max;

	public:
		ControlParameterInt(Node* parent, std::string name, int value, int min, int max) : ControlParameter(parent, name, "int"), m_value(value), m_min(min), m_max(max) {};
		~ControlParameterInt() {};

		int getValue() const { return m_value; };
		void setValue(int value) { m_value = value; };

		int getMin() const { return m_min; };
		void setMin(int min) { m_min = min; };

		int getMax() const { return m_max; };
		void setMax(int max) { m_max = max; };
	};

	class ControlParameterUInt : public ControlParameter
	{
		uint32_t m_value;
		uint32_t m_min;
		uint32_t m_max;

	public:
		ControlParameterUInt(Node* parent, std::string name, uint32_t value, uint32_t min, uint32_t max) : ControlParameter(parent, name, "uint"), m_value(value), m_min(min), m_max(max) {};
		~ControlParameterUInt() {};

		uint32_t getValue() const { return m_value; };
		void setValue(uint32_t value) { m_value = value; };

		uint32_t getMin() const { return m_min; };
		void setMin(uint32_t min) { m_min = min; };

		uint32_t getMax() const { return m_max; };
		void setMax(uint32_t max) { m_max = max; };
	};

	class ControlParameterFloat : public ControlParameter
	{
		float m_value;
		float m_min;
		float m_max;

	public:
		ControlParameterFloat(Node* parent, std::string name, float value, float min, float max) : ControlParameter(parent, name, "float"), m_value(value), m_min(min), m_max(max) {};
		~ControlParameterFloat() {};

		float getValue() const { return m_value; };
		void setValue(float value) { m_value = value; };

		float getMin() const { return m_min; };
		void setMin(float min) { m_min = min; };

		float getMax() const { return m_max; };
		void setMax(float max) { m_max = max; };
	};

	class ControlParameterVector3 : public ControlParameter
	{
		NMP::Vector3 m_value;
		NMP::Vector3 m_min;
		NMP::Vector3 m_max;

	public:
		ControlParameterVector3(Node* parent, std::string name, NMP::Vector3 value, NMP::Vector3 min, NMP::Vector3 max) : ControlParameter(parent, name, "vector3"), m_value(value), m_min(min), m_max(max) {};
		~ControlParameterVector3() {};

		NMP::Vector3 getValue() const { return m_value; };
		void setValue(NMP::Vector3 value) { m_value = value; };

		NMP::Vector3 getMin() const { return m_min; };
		void setMin(NMP::Vector3 min) { m_min = min; };

		NMP::Vector3 getMax() const { return m_max; };
		void setMax(NMP::Vector3 max) { m_max = max; };
	};

	class ControlParameterVector4 : public ControlParameter
	{
		NMP::Quat m_value;
		NMP::Quat m_min;
		NMP::Quat m_max;

	public:
		ControlParameterVector4(Node* parent, std::string name, NMP::Quat value, NMP::Quat min, NMP::Quat max) : ControlParameter(parent, name, "vector4"), m_value(value), m_min(min), m_max(max) {};
		~ControlParameterVector4() {};

		NMP::Quat getValue() const { return m_value; };
		void setValue(NMP::Quat value) { m_value = value; };

		NMP::Quat getMin() const { return m_min; };
		void setMin(NMP::Quat min) { m_min = min; };

		NMP::Quat getMax() const { return m_max; };
		void setMax(NMP::Quat max) { m_max = max; };
	};

	class ControlParameterQuaternion : public ControlParameter
	{
		NMP::Quat m_value;
		NMP::Quat m_min;
		NMP::Quat m_max;

	public:
		ControlParameterQuaternion(Node* parent, std::string name, NMP::Quat value, NMP::Quat min, NMP::Quat max) : ControlParameter(parent, name, "quaternion"), m_value(value), m_min(min), m_max(max) {};
		~ControlParameterQuaternion() {};

		NMP::Quat getValue() const { return m_value; };
		void setValue(NMP::Quat value) { m_value = value; };

		NMP::Quat getMin() const { return m_min; };
		void setMin(NMP::Quat min) { m_min = min; };

		NMP::Quat getMax() const { return m_max; };
		void setMax(NMP::Quat max) { m_max = max; };
	};
}