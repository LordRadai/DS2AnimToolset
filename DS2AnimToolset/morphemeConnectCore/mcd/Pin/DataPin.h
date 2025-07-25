#pragma once
#include "Pin.h"

namespace mcd
{
	class DataPin : public Pin
	{
		db::EnumAttribute m_dataType;
	public:
		DataPin(db::Node* parent, const std::string pinName, const std::string& dataType);

		virtual ~DataPin() override {};

		const std::string& getDataType() const { return m_dataType.getValue(); }
		void setDataType(const std::string& dataType) { m_dataType.setValue(dataType); }
	};
}
