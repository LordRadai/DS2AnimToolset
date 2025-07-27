#include "DataPin.h"

namespace mcd
{
	DataPin::DataPin(db::Node* parent, const std::string pinName, DataTypes dataType)
		: Pin(parent, "DataPin", pinName),
		m_dataType(std::make_unique<db::EnumAttribute>(this, "DataType", ""))
	{
		this->addAttribute(m_dataType.get());
		setDataType(dataType);
	}

	const DataTypes DataPin::getDataType() const
	{
		const std::string& typeStr = m_dataType->getValue();

		if (typeStr == "float")
			return DataTypes::kFloat;
		else if (typeStr == "vector3")
			return DataTypes::kVector3;
		else if (typeStr == "vector4")
			return DataTypes::kVector4;
		else if (typeStr == "bool")
			return DataTypes::kBool;
		else if (typeStr == "quaternion")
			return DataTypes::kQuaternion;
		else if (typeStr == "int")
			return DataTypes::kInt;
		else if (typeStr == "uint")
			return DataTypes::kUInt;
		else
			throw std::invalid_argument("Unsupported data type for DataPin.");
	}

	void DataPin::setDataType(DataTypes dataType)
	{
		switch (dataType)
		{
		case DataTypes::kFloat:
			m_dataType->setValue("float");
			break;
		case DataTypes::kVector3:
			m_dataType->setValue("vector3");
			break;
		case DataTypes::kVector4:
			m_dataType->setValue("vector4");
			break;
		case DataTypes::kBool:
			m_dataType->setValue("bool");
			break;
		case DataTypes::kQuaternion:
			m_dataType->setValue("quaternion");
			break;
		case DataTypes::kInt:
			m_dataType->setValue("int");
			break;
		case DataTypes::kUInt:
			m_dataType->setValue("uint");
			break;
		default:
			throw std::invalid_argument("Unsupported data type for DataPin.");
		}
	}
}
