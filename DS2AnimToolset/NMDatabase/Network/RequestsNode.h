#pragma once
#include "../Request/Request.h"
#include "../NodeContainer/NodeContainer.h"

namespace db
	{
	class RequestsNode : public Node
	{
		NodeContainer m_requests;

	public:
		RequestsNode(Node* parent) : Node(parent, "RequestsNode", "Requests"), m_requests(this, "RequestArray") {};
		virtual ~RequestsNode() {};
		virtual bool isValid() const { return Node::isValid() && m_requests.isValid(); }
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		void addRequest(Request* request) { m_requests.addNode(request); }
		void removeRequest(int index) { m_requests.removeNode(index); }
		Request* getRequest(int index) const { return dynamic_cast<Request*>(m_requests.getNode(index)); }
		size_t getNumRequests() const { return m_requests.getNumNodes(); }
	};
}
