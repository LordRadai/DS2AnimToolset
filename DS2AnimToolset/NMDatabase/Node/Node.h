#pragma once
#include "RCore.h"

namespace db
{
	class Node
	{
	protected:
		std::string m_name = "";
		Node* m_parent = nullptr;

	public:
		Node(Node* parent, std::string name) : m_parent(parent), m_name(name) {};
		~Node() {};

		std::string getName() const { return m_name; };
		Node* getParent() const { return m_parent; };

		std::string getQualifiedName() const;
	};
}