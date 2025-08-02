#include "RequestsNode.h"

namespace mcd
{
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
