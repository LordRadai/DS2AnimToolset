#pragma once
#include "RCore.h"
#include "Attribute/CompoundAttribute.h"

namespace db
{
	class Node : public CompoundAttribute
	{
	public:
		Node(Node* parent, std::string name, std::string label) : CompoundAttribute(parent, name, label, "node") {}
		
		virtual ~Node() override {};

		virtual bool writeValueXML(int format) override;
	};
}