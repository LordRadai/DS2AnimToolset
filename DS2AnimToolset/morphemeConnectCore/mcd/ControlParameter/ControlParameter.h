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

		db::TypedNodeContainer<mcd::DataPin> m_dataPins;
		db::FloatAttribute m_min;
		db::FloatAttribute m_max;
		db::FloatAttribute m_defaultFloat;
		db::StringAttribute m_defaultString;
		db::Vector3Attribute m_defaultVector3;
		db::BoolAttribute m_defaultBool;
		db::IntAttribute m_defaultInt;
		db::IntAttribute m_minInt;
		db::IntAttribute m_maxInt;
		db::QuaternionAttribute m_defaultQuaternion;

	public:
		ControlParameter(db::Node* parent, const std::string& name);

		virtual ~ControlParameter() override {};

		uint32_t getPinCount() const { return m_dataPins.size(); }
		mcd::DataPin* getDataPin(uint32_t index) const { return m_dataPins.getNode(index); }
		mcd::DataPin* getResultDataPin() const;
	};
}