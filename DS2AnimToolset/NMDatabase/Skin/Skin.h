#pragma once
#include "RCore.h"
#include "../Node/Node.h"

namespace db
{
	class Skin : public Node
	{
		std::string m_path;

	public:
		Skin(Node* parent, const std::string& name, const std::string& path) : Node(parent, name), m_path(path) {}
		~Skin() {};

		std::string getPath() const { return m_path; }
	};
}