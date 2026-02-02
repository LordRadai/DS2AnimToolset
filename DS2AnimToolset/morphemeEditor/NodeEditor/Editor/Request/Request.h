#pragma once
#include <string>

namespace NodeEditor
{
	class Request
	{
		int m_requestID;
		std::string m_name;
		std::string m_type;

	public:
		Request(int requestID, const std::string& name, const std::string& typeName) : m_requestID(requestID), m_name(name), m_type(typeName) {}

		virtual ~Request() {}

		int getRequestID() const { return m_requestID; }
		const std::string& getName() const { return m_name; }
		const std::string& getType() const { return m_type; }
	};
}
