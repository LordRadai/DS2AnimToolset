#include "RequestAttribute.h"

namespace mcd
{
	RequestAttribute::RequestAttribute(db::CompositeAttribute* parent, const std::string& name) :
		mcd::Attribute(parent, "RequestAttribute", name),
		m_value(std::make_unique<db::Pointer<mcd::Request>>(this, "RequestEntry", nullptr))
	{
	}

	RequestAttribute::~RequestAttribute()
	{
	}

	bool RequestAttribute::isValueEqualTo(Attribute* attr)
	{
		if (!attr->isOfType<RequestAttribute>())
			return false;

		RequestAttribute* otherReqAttr = dynamic_cast<RequestAttribute*>(attr);

		return otherReqAttr->getValue() == m_value->getValue();
	}

	void RequestAttribute::setValue(mcd::Request* request)
	{
		removeAttribute(m_value.get());

		m_value->setValue(request);

		if (request != nullptr)
			addAttribute(m_value.get());
	}
}