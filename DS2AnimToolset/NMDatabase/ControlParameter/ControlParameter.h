#pragma once
#include "RCore.h"

namespace db
{
	class ControlParameter
	{
	protected:
		enum class CpType
		{
			kBool,
			kInt,
			kUInt,
			kFloat,
			kVector3,
			kVector4,
			kQuaternion
		};

		CpType m_type;
		std::string m_name;

		ControlParameter(std::string name, CpType type)
			: m_name(name), m_type(type) {
		};
		~ControlParameter() {};

	public:
		std::string getName() const { return m_name; };
		void setName(std::string name) { m_name = name; };

		CpType getType() const { return m_type; };
	};

	class ControlParameterBool : public ControlParameter
	{
		bool m_value;

	public:
		ControlParameterBool(std::string name, bool value) 
			: ControlParameter(name, CpType::kBool), m_value(value) {
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
			: ControlParameter(name, CpType::kInt), m_value(value), m_min(min), m_max(max) {
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
			: ControlParameter(name, CpType::kUInt), m_value(value), m_min(min), m_max(max) {
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
			: ControlParameter(name, CpType::kFloat), m_value(value), m_min(min), m_max(max) {
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
		DirectX::SimpleMath::Vector3 m_value;
		DirectX::SimpleMath::Vector3 m_min;
		DirectX::SimpleMath::Vector3 m_max;

	public:
		ControlParameterVector3(std::string name, DirectX::SimpleMath::Vector3 value, 
			DirectX::SimpleMath::Vector3 min, DirectX::SimpleMath::Vector3 max) 
			: ControlParameter(name, CpType::kVector3), m_value(value), m_min(min), m_max(max) {
		};
		~ControlParameterVector3() {};

		DirectX::SimpleMath::Vector3 getValue() const { return m_value; };
		void setValue(DirectX::SimpleMath::Vector3 value) { m_value = value; };

		DirectX::SimpleMath::Vector3 getMin() const { return m_min; };
		void setMin(DirectX::SimpleMath::Vector3 min) { m_min = min; };

		DirectX::SimpleMath::Vector3 getMax() const { return m_max; };
		void setMax(DirectX::SimpleMath::Vector3 max) { m_max = max; };
	};

	class ControlParameterVector4 : public ControlParameter
	{
		DirectX::SimpleMath::Vector4 m_value;
		DirectX::SimpleMath::Vector4 m_min;
		DirectX::SimpleMath::Vector4 m_max;

	public:
		ControlParameterVector4(std::string name, DirectX::SimpleMath::Vector4 value, 
			DirectX::SimpleMath::Vector4 min, DirectX::SimpleMath::Vector4 max) 
			: ControlParameter(name, CpType::kVector4), m_value(value), m_min(min), m_max(max) {
		};
		~ControlParameterVector4() {};

		DirectX::SimpleMath::Vector4 getValue() const { return m_value; };
		void setValue(DirectX::SimpleMath::Vector4 value) { m_value = value; };

		DirectX::SimpleMath::Vector4 getMin() const { return m_min; };
		void setMin(DirectX::SimpleMath::Vector4 min) { m_min = min; };

		DirectX::SimpleMath::Vector4 getMax() const { return m_max; };
		void setMax(DirectX::SimpleMath::Vector4 max) { m_max = max; };
	};

	class ControlParameterQuaternion : public ControlParameter
	{
		DirectX::SimpleMath::Quaternion m_value;
		DirectX::SimpleMath::Quaternion m_min;
		DirectX::SimpleMath::Quaternion m_max;

	public:
		ControlParameterQuaternion(std::string name, DirectX::SimpleMath::Quaternion value, 
			DirectX::SimpleMath::Quaternion min, DirectX::SimpleMath::Quaternion max) 
			: ControlParameter(name, CpType::kQuaternion), m_value(value), m_min(min), m_max(max) {
		};
		~ControlParameterQuaternion() {};

		DirectX::SimpleMath::Quaternion getValue() const { return m_value; };
		void setValue(DirectX::SimpleMath::Quaternion value) { m_value = value; };

		DirectX::SimpleMath::Quaternion getMin() const { return m_min; };
		void setMin(DirectX::SimpleMath::Quaternion min) { m_min = min; };

		DirectX::SimpleMath::Quaternion getMax() const { return m_max; };
		void setMax(DirectX::SimpleMath::Quaternion max) { m_max = max; };
	};
}