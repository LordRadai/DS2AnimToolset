#pragma once
#include "NMDatabase/NMDatabase.h"

namespace mcd
{
	class Request : public db::Node
	{
		int m_requestID;
	public:
		Request(db::Node* parent, const std::string& name, const int id);

		virtual ~Request() override {};

		int getRequestID() const { return m_requestID; }
	};
}
