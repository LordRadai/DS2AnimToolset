#pragma once
#include "../Attribute.h"
#include "NMDBExtensions/Pointer.inl"
#include "mcd/Request/Request.h"

namespace mcd
{
	class RequestAttribute : public mcd::Attribute
	{
		std::unique_ptr<db::Pointer<mcd::Request>> m_value;

	public:
		RequestAttribute(db::Attribute* parent, const std::string& name);

		virtual ~RequestAttribute() override;

		mcd::Request* getValue() const { return m_value->getValue(); }
		void setValue(mcd::Request* request) { m_value->setValue(request); }
	};
}
