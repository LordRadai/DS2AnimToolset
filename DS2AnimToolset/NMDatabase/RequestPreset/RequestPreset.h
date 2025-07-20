#pragma once
#include "RCore.h"
#include "../Request/Request.h"
#include "../Node/Node.h"
#include "../NodeContainer/NodeContainer.h"

namespace db
{
	class RequestPreset : public Node
	{
		Request m_request;

	public:
		RequestPreset(Node* parent, Request request) : Node(parent, "Message"), m_request(request) {};
		
		virtual ~RequestPreset() {};
		virtual bool isValid() const { return Node::isValid() && m_request.isValid(); };
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		Request getRequest() const { return m_request; };
	};
}