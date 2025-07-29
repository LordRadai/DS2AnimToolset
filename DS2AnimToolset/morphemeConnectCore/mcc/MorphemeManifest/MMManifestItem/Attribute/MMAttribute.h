#pragma once
#include "../MMManifestItemBase.h"

namespace mcc
{
	enum class Type
	{
		kBool,
		kInt,
		kFloat,
		kString,
		kVector3,
		kVector4,
		kQuaternion,
		kAnimationTake,
		kNumDataTypes
	};

	class MMAttribute : public MMManifestItemBase
	{
		Type m_type;
		std::string m_name;
		nlohmann::json m_value;
		std::string m_units;
		float m_min;
		float m_max;
		std::string m_displayName;
		std::string m_helpText;
		bool m_bPerAnimSet;

	public:
		Type getAttributeTypeFromString(const std::string& typeStr) const
		{
			if (typeStr == "int") return Type::kInt;
			else if (typeStr == "float") return Type::kFloat;
			else if (typeStr == "string") return Type::kString;
			else if (typeStr == "bool") return Type::kBool;
			else if (typeStr == "vector3") return Type::kVector3;
			else if (typeStr == "vector4") return Type::kVector4;
			else if (typeStr == "quaternion") return Type::kQuaternion;
			else if (typeStr == "animationTake") return Type::kAnimationTake;
			return Type::kNumDataTypes;
		}

		MMAttribute() : m_value(nullptr), m_min(0.0f), m_max(1.0f), m_bPerAnimSet(false) {}

		virtual ~MMAttribute() override {}
		virtual void fromJson(const nlohmann::json& json) override;

		Type getType() const { return m_type; }
		std::string getName() const { return m_name; }

		bool getValueAsBool() const
		{
			if (m_type != Type::kBool) throw std::runtime_error("MMAttribute type is not bool");
			return m_value.get<bool>();
		}

		int getValueAsInt() const
		{
			if (m_type != Type::kInt) throw std::runtime_error("MMAttribute type is not int");
			return m_value.get<int>();
		}

		float getValueAsFloat() const
		{
			if (m_type != Type::kFloat) throw std::runtime_error("MMAttribute type is not float");
			return m_value.get<float>();
		}

		std::string getValueAsString() const
		{
			if (m_type != Type::kString) throw std::runtime_error("MMAttribute type is not string");
			return m_value.get<std::string>();
		}

		nlohmann::json getValue() const
		{
			return m_value;
		}
	};
}
