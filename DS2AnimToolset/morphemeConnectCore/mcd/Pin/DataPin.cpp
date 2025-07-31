#include "DataPin.h"

namespace mcd
{
	DataPin::DataPin(db::Node* parent, const std::string pinName, DataTypes dataType)
		: Pin(parent, "DataPin", pinName),
		m_dataType(std::make_unique<db::EnumAttribute>(this, "DataType", "")),
		m_passThroughEnabled(std::make_unique<db::BoolAttribute>(this, "PassThroughEnabled", false)),
		m_input(std::make_unique<db::BoolAttribute>(this, "Input", false))
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

	void DataPin::setPassThroughEnabled(bool enabled)
	{
		removeAttribute(m_passThroughEnabled.get());

		// Only add to the list if it is enabled.
		if (enabled)
			addAttribute(m_passThroughEnabled.get());

		m_passThroughEnabled->setValue(enabled);
	}

	void DataPin::setInput(bool input)
	{
		removeAttribute(m_input.get());

		// Only add to the list if it is an input.
		if (input)
			addAttribute(m_input.get());

		m_input->setValue(input);
	}
}
