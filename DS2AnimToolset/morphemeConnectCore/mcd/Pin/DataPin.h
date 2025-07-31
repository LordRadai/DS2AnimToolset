#pragma once
#include "Pin.h"
#include "DataTypes.h"

namespace mcd
{
	class DataPin : public Pin
	{
		std::unique_ptr<db::EnumAttribute> m_dataType;
		std::unique_ptr<db::BoolAttribute> m_passThroughEnabled;
		std::unique_ptr<db::BoolAttribute> m_input;
	public:
		DataPin(db::Node* parent, const std::string pinName, DataTypes dataType);

		virtual ~DataPin() override {};

		const DataTypes getDataType() const;
		void setDataType(DataTypes dataType);

		void setPassThroughEnabled(bool enabled);
		void setInput(bool input);
		bool isPassThroughEnabled() const { return m_passThroughEnabled->getValue(); }
		bool isInput() const { return m_input->getValue(); }
	};
}
