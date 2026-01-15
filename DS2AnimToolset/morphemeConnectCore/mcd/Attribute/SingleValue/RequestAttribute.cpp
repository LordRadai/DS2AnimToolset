#include "RequestAttribute.h"

namespace mcd
{
	RequestAttribute::RequestAttribute(db::Attribute* parent, const std::string& name) :
		mcd::Attribute(parent, name, "RequestAttribute"),
		m_value(std::make_unique<db::Pointer<mcd::Request>>(this, "RequestEntry", nullptr))
	{
		addAttribute(m_value.get());
	}

	RequestAttribute::~RequestAttribute()
	{
	}


}