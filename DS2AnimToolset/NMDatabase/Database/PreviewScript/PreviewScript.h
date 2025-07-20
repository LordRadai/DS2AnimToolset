#pragma once
#include "RCore.h"
#include "../Node/Node.h"

namespace db
{
	class PreviewScript : public Node
	{
		std::string m_filepath;

	public:
		PreviewScript(Node* parent, const std::string name, const std::string filepath) : Node(parent, "PreviewScriptNode", name), m_filepath(filepath) {};
		
		virtual ~PreviewScript() {};
		virtual bool isValid() const { return Node::isValid(); }
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		std::string getFilepath() const { return m_filepath; };
	};
}