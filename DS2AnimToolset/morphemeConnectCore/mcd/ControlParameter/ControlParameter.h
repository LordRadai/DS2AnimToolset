#pragma once
#include "NMDatabase/NMDatabase.h"
#include "NMDBExtensions/TypedNodeContainer.inl"
#include "NMDBExtensions/Pointer.inl"
#include "mcd/Pin/DataPin.h"
#include "mcd/AttributePinNodeBase/AttributePinNodeBase.h"

namespace mcc
{
	class MorphemeDocument
}

namespace mcd
{
	class EmittedControlParameter;

	class ControlParameter : public mcd::AttributePinNodeBase
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
		std::unique_ptr<db::Pointer<EmittedControlParameter>> m_emittedControlParameter;

	public:
		ControlParameter(db::Node* parent, const std::string& name, DataPin::DataType dataType);

		virtual ~ControlParameter() override {};
		virtual int getPinCount() const override { return m_dataPins->size(); }
		virtual mcd::Pin* getPin(int idx) override { return m_dataPins->getNode(idx); }
		virtual mcd::Pin* getPin(const std::string& name) override { return m_dataPins->find(name); }

		mcd::DataPin* getResultDataPin() const;

		DataPin::DataType getDataType() const;
	};
}