#include "DataPin.h"
#include "mcu/Log.h"
#include "PassDownPin.h"
#include "mcd/ControlParameter/ControlParameter.h"
#include "mcd/Graph/GraphNode.h"

namespace mcd
{
	DataPin::DataPin(db::Node* parent, const std::string& pinName, DataType dataType)
		: Pin(parent, "DataPin", pinName),
		m_dataType(std::make_unique<db::EnumAttribute>(this, "Type", ""))
	{
		this->addAttribute(m_dataType.get());
		setDataType(dataType);
	}

	bool DataPin::isCompatibleConnectionTarget(Pin* to)
	{
		if (isInput())
			return false;

		if (to->isOfType<DataPin>())
		{
			mcd::DataPin* targetDataPin = dynamic_cast<mcd::DataPin*>(to);

			if (getType() != targetDataPin->getType())
				return false;

			if (!targetDataPin->isInput())
				return false;

			GraphNode* myParentGraphNode = dynamic_cast<GraphNode*>(getParentNode());

			if (!myParentGraphNode)
			{
				db::Attribute* parentAttribute = getParentAttribute();

				LOG_TODO("Disallow ControlParameter -> EmittedControlParameter connection");

				return true;
			}

			const int pinCount = myParentGraphNode->getPinCount();

			if (pinCount > 0)
			{
				for (int i = 0; i < pinCount; i++)
				{
					DataPin* pin = myParentGraphNode->getPin(i)->asDataPin();

					if (pin == this)
						continue;

					if (pin->isInput())
					{
						if (myParentGraphNode->isOperatorNode())
							return true;

						if (targetDataPin->hasParentNode<GraphNode>())
							return false;

						return true;
					}
				}
			}

			return true;
		}

		if (to->isOfType<PassDownPin>())
		{
			PassDownPin* targetPassDownPin = dynamic_cast<PassDownPin*>(to);

			if (!targetPassDownPin->isInput())
				return false;

			Graph* childGraph = targetPassDownPin->getChildGraph();
			Graph* thisGraph = getParentOrGrandParentGraph();

			if (childGraph != thisGraph)
			{
				if (targetPassDownPin->hasDownstreamConnection())
				{
					DataPin* downstreamData = targetPassDownPin->recurseDownstreamToFirstDataPin();

					if (downstreamData)
						return isCompatibleConnectionTarget(downstreamData);

					if (targetPassDownPin->getFirstDownstreamFunctionalPin())
						return false;
				}

				return true;
			}
		}

		return false;
	}

	bool DataPin::canStartConnection()
	{
		return !isInput();
	}

	bool DataPin::canReceiveConnection(Pin* from)
	{
		if (!isInput())
			return false;

		LOG_TODO("Handle emitted control parameters");

		if (hasIncidentEdge())
			return false;

		return true;
	}

	const DataPin::DataType DataPin::getDataType() const
	{
		const std::string& typeStr = m_dataType->getValue();

		if (typeStr == "float")
			return DataType::kFloat;
		else if (typeStr == "vector3")
			return DataType::kVector3;
		else if (typeStr == "vector4")
			return DataType::kVector4;
		else if (typeStr == "bool")
			return DataType::kBool;
		else if (typeStr == "quaternion")
			return DataType::kQuaternion;
		else if (typeStr == "int")
			return DataType::kInt;
		else if (typeStr == "uint")
			return DataType::kUInt;
		else
			throw std::invalid_argument("Unsupported data type for DataPin.");
	}

	void DataPin::setDataType(DataType dataType)
	{
		switch (dataType)
		{
		case DataType::kFloat:
			m_dataType->setValue("float");
			break;
		case DataType::kVector3:
			m_dataType->setValue("vector3");
			break;
		case DataType::kVector4:
			m_dataType->setValue("vector4");
			break;
		case DataType::kBool:
			m_dataType->setValue("bool");
			break;
		case DataType::kQuaternion:
			m_dataType->setValue("quaternion");
			break;
		case DataType::kInt:
			m_dataType->setValue("int");
			break;
		case DataType::kUInt:
			m_dataType->setValue("uint");
			break;
		default:
			throw std::invalid_argument("Unsupported data type for DataPin.");
		}
	}

	DataPin::DataType DataPin::getDataTypeFromString(const std::string& typeName)
	{
		if (typeName == "float") return DataType::kFloat;
		if (typeName == "vector3") return DataType::kVector3;
		if (typeName == "vector4") return DataType::kVector4;
		if (typeName == "bool") return DataType::kBool;
		if (typeName == "quaternion") return DataType::kQuaternion;
		if (typeName == "int") return DataType::kInt;
		if (typeName == "uint") return DataType::kUInt;
		if (typeName == "string") return DataType::kString;

		throw std::invalid_argument("Invalid data type string: " + typeName);
	}

	std::string DataPin::getStringFromDataType(DataType dataType)
	{
		switch (dataType)
		{
		case DataType::kFloat: return "float";
		case DataType::kVector3: return "vector3";
		case DataType::kVector4: return "vector4";
		case DataType::kBool: return "bool";
		case DataType::kQuaternion: return "quaternion";
		case DataType::kInt: return "int";
		case DataType::kUInt: return "uint";
		case DataType::kString: return "string";
		}

		throw std::invalid_argument("Invalid data type: " + std::to_string(static_cast<int>(dataType)));
	}
}
