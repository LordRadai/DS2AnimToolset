#pragma once
#include "RCore.h"
#include "../Node/Node.h"

namespace db
{
	class BodyGroup : public Node
	{
		std::string m_name;

	public:
		BodyGroup() {};
		BodyGroup(const std::string& name) : m_name(name) {}
		~BodyGroup() {};

		std::string getName() const { return m_name; }
	};
}