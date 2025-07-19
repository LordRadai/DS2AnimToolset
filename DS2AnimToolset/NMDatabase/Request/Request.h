#pragma once
#include "RCore.h"

namespace db
{
	class Request
	{
		std::string m_name;

	public:
		Request(std::string name)
			: m_name(name) {}

		~Request() {};

		std::string getName() const { return m_name; }
	};
}