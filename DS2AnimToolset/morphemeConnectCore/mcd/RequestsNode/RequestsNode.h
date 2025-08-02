#pragma once
#include "NMDatabase/NMDatabase.h"
#include "NMDBExtensions/TypedNodeContainer.inl"
#include "mcd/Request/Request.h"

namespace mcd
{
	class RequestsNode : public db::Node
	{
		std::unique_ptr<db::TypedNodeContainer<mcd::Request>> m_requests;

	public:
		RequestsNode(db::Attribute* parent, std::string name);

		virtual ~RequestsNode() override {}

		mcd::Request* getRequest(const uint32_t index) const { return m_requests->getNode(index); }
		mcd::Request* findRequest(const uint32_t id) const;
		mcd::Request* findRequest(const std::string& name) const { return m_requests->find(name); }

		void addControlParameter(mcd::Request* request) { m_requests->add(request); }
	};
}
