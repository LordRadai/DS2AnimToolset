#pragma once
#include "RCore.h"
#include "../Node/Node.h"

namespace db
{
	class PreviewScript : public Node
	{
		std::string m_filepath;

	public:
		PreviewScript(Node* parent, const std::string name, const std::string filepath) : Node(parent, name), m_filepath(filepath) {};
		~PreviewScript() {};

		std::string getFilepath() const { return m_filepath; };
	};
}