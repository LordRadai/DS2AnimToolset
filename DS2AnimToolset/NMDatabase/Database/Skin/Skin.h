#pragma once
#include "RCore.h"
#include "../Node/Node.h"

namespace db
{
	class Skin : public Node
	{
		std::string m_path;

	public:
		Skin(Node* parent, const std::string& name, const std::string& path) : Node(parent, "Skin", name), m_path(path) {}
		
		virtual ~Skin() {};
		virtual bool isValid() const;
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		std::string getPath() const { return m_path; }
	};
}