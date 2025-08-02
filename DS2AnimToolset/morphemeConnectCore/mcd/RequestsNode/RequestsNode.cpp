#include "RequestsNode.h"

namespace mcd
{
	RequestsNode::RequestsNode(db::Attribute* parent, std::string name)
		: db::Node(parent, "RequestsNode", name),
		  m_requests(std::make_unique<db::TypedNodeContainer<mcd::Request>>(this, "RequestArray"))
	{
		addAttribute(m_requests.get());
	}

	mcd::Request* RequestsNode::findRequest(const uint32_t id) const
	{
		for (size_t i = 0; i < m_requests->size(); i++)
		{
			mcd::Request* request = m_requests->getNode(i);

			if (request->getRequestID() == id)
				return request;
		}
	}
}
