#pragma once
#include "RCore.h"

namespace db
{
	class Node
	{
	protected:
		std::string m_name;

	public:
		Node() {};

		Node(std::string name)
			: m_name(name) {
		};

		~Node() {};

		std::string getName() const { return m_name; };
	};
}