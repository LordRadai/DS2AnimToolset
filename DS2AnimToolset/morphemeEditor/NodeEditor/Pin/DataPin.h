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
		DataPin(Node* parent, const std::string& name, bool isInput, DataType dataType)
			: Pin(parent, name, isInput), m_dataType(dataType) {}

		virtual ~DataPin() override {}
		virtual void draw() override;

		DataType getDataType() const { return m_dataType; }
	};
}
