#pragma once
#include "Pin.inl"
#include "DataTypes.h"

namespace mcd
{
	class DataPin : public Pin
	{
		std::unique_ptr<db::EnumAttribute> m_dataType;
	public:
		DataPin(db::Node* parent, const std::string pinName, DataTypes dataType);

		virtual ~DataPin() override {};

		const DataTypes getDataType() const;
		void setDataType(DataTypes dataType);
	};
}
