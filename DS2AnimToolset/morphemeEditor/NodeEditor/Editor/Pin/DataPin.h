#pragma once
#include "Pin.h"

namespace NodeEditor
{
	class DataPin : public Pin
	{
	public:
		enum DataType
		{
			kDataTypeFloat,
			kDataTypeVector3,
			kDataTypeVector4,
			kDataTypeBool,
			kDataTypeQuaternion,
			kDataTypeInt,
			kDataTypeUInt,

			kNumDataTypes
		};

	private:
		DataType m_dataType;

	public:
		DataPin(NodeEditor* editor, Node* parent, const std::string& name, bool isInput, DataType dataType)
			: Pin(editor, parent, name, isInput), m_dataType(dataType) {}

		virtual ~DataPin() override {}
		virtual void draw() override;

		DataType getDataType() const { return m_dataType; }

		static const char* dataTypeToString(DataType dataType);
		static DataType stringToDataType(const std::string& typeStr);
	};
}
