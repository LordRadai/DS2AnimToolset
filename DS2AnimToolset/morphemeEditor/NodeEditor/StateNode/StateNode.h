#pragma once
#include "NodeEditor/Node/Node.h"

namespace NodeEditor
{
	class StateNode : public Node
	{
	public:
		StateNode(NodeEditor* editor, Graph* parent, int id, const std::string& name, Graph* subGraph);
		virtual ~StateNode() override {}
		virtual void draw() override;
	};
}
