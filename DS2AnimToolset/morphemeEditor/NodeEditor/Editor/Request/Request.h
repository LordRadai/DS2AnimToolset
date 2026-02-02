#pragma once
#include <string>

namespace NodeEditor
{
	class Request
	{
		int m_requestID;
		std::string m_name;

	public:
		Request(int requestID, const std::string& name) : m_requestID(requestID), m_name(name) {}

		virtual ~Request() {}

		int getRequestID() const { return m_requestID; }
		const std::string& getName() const { return m_name; }
	};
}
