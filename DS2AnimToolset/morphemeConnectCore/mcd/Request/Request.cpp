#include "Request.h"

namespace mcd
{
	Request::Request(db::Node* parent, const std::string& name, const int id) :
		db::Node(parent, "Request", name),
		m_requestID(id)
	{};
}