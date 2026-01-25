#pragma once
#include "Pin.h"

namespace mcd
{
	class DataPin : public Pin
	{
	public:
		enum class DataType
		{
			kFloat,
			kVector3,
			kVector4,
			kBool,
			kQuaternion,
			kInt,
			kUInt,
			kString,

			kNumDataType
		};

	private:
		std::unique_ptr<db::EnumAttribute> m_dataType;
	public:
		DataPin(db::Node* parent, const std::string& pinName, DataType dataType);

		virtual ~DataPin() override {};
		virtual bool isCompatibleConnectionTarget(Pin* to) override;
		virtual bool canStartConnection() override;
		virtual bool canReceiveConnection(Pin* from) override;

		const DataType getDataType() const;

		static DataPin::DataType getDataTypeFromString(const std::string& typeName);
		static std::string getStringFromDataType(DataType type);
	protected:
		void setDataType(DataType dataType);
	};
}
