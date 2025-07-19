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

		ControlParameter() : Node("ControlParameter"), m_type("undefined") {};
		ControlParameter(std::string name, std::string type) : Node(name), m_type(type) {};

		~ControlParameter() {};

	public:
		std::string getType() const { return m_type; };
	};

	class ControlParameterBool : public ControlParameter
	{
		bool m_value;

	public:
		ControlParameterBool(std::string name, bool value) 
			: ControlParameter(name, "bool"), m_value(value) 
		{
			m_dataPin = DataPin("Result", "bool");
		};

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
		ControlParameterInt(std::string name, int value, int min, int max) 
			: ControlParameter(name, "int"), m_value(value), m_min(min), m_max(max) 
		{
			m_dataPin = DataPin("Result", "int");
		};

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
		ControlParameterUInt(std::string name, uint32_t value, uint32_t min, uint32_t max) 
			: ControlParameter(name, "uint"), m_value(value), m_min(min), m_max(max) 
		{
			m_dataPin = DataPin("Result", "uint");
		};

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
		ControlParameterFloat(std::string name, float value, float min, float max) 
			: ControlParameter(name, "float"), m_value(value), m_min(min), m_max(max) 
		{
			m_dataPin = DataPin("Result", "float");
		};

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
		ControlParameterVector3(std::string name, NMP::Vector3 value,
			NMP::Vector3 min, NMP::Vector3 max) 
			: ControlParameter(name, "vector3"), m_value(value), m_min(min), m_max(max) 
		{
			m_dataPin = DataPin("Result", "vector3");
		};

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
		ControlParameterVector4(std::string name, NMP::Quat value, 
			NMP::Quat min, NMP::Quat max) 
			: ControlParameter(name, "vector4"), m_value(value), m_min(min), m_max(max) 
		{
			m_dataPin = DataPin("Result", "vector4");
		};

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
		ControlParameterQuaternion(std::string name, NMP::Quat value, 
			NMP::Quat min, NMP::Quat max) 
			: ControlParameter(name, "quaternion"), m_value(value), m_min(min), m_max(max) 
		{
			m_dataPin = DataPin("Result", "quaternion");
		};

		~ControlParameterQuaternion() {};

		NMP::Quat getValue() const { return m_value; };
		void setValue(NMP::Quat value) { m_value = value; };

		NMP::Quat getMin() const { return m_min; };
		void setMin(NMP::Quat min) { m_min = min; };

		NMP::Quat getMax() const { return m_max; };
		void setMax(NMP::Quat max) { m_max = max; };
	};
}