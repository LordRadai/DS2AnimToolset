#pragma once
#include <string>

namespace NodeEditor
{
	class Message
	{
		int m_messageID;
		std::string m_name;
		std::string m_type;

	public:
		Message(int requestID, const std::string& name, const std::string& typeName) : m_messageID(requestID), m_name(name), m_type(typeName) {}

		virtual ~Message() {}

		int getRequestID() const { return m_messageID; }
		const std::string& getName() const { return m_name; }
		const std::string& getType() const { return m_type; }
	};
}
