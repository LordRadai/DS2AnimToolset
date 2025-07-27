#pragma once
#include "NMDatabase/NMDatabase.h"
#include "NMDBExtensions/TypedNodeContainer.inl"
#include "mcd/Pin/DataPin.h"

namespace mcc
{
	class MorphemeDocument;
}

namespace mcd
{
	class ControlParameter : public db::Node
	{
		friend class mcc::MorphemeDocument;

		std::unique_ptr<db::TypedNodeContainer<mcd::DataPin>> m_dataPins;
		std::unique_ptr<db::FloatAttribute> m_min;
		std::unique_ptr<db::FloatAttribute> m_max;
		std::unique_ptr<db::FloatAttribute> m_defaultFloat;
		std::unique_ptr<db::StringAttribute> m_defaultString;
		std::unique_ptr<db::Vector3Attribute> m_defaultVector3;
		std::unique_ptr<db::BoolAttribute> m_defaultBool;
		std::unique_ptr<db::IntAttribute> m_defaultInt;
		std::unique_ptr<db::IntAttribute> m_minInt;
		std::unique_ptr<db::IntAttribute> m_maxInt;
		std::unique_ptr<db::QuaternionAttribute> m_defaultQuaternion;

	public:
		ControlParameter(db::Node* parent, const std::string& name);

		virtual ~ControlParameter() override {};

		uint32_t getPinCount() const { return m_dataPins->size(); }
		mcd::DataPin* getDataPin(uint32_t index) const { return m_dataPins->getNode(index); }
		mcd::DataPin* getResultDataPin() const;
	};
}