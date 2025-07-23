#pragma once
#include "RCore.h"
#include "Attribute/CompoundAttribute.h"

namespace db
{
	class Node : public CompoundAttribute
	{
		std::string m_nodeName;
	public:
		Node(Node* parent, std::string name, std::string nodeName) : CompoundAttribute(parent, name, "node"), m_nodeName(nodeName) {}
		
		virtual ~Node() override {};

		virtual bool writeValueXML(int format) override;
		virtual bool writeStartElementXML(int format) override;
	};
}