#pragma once
#include "NodeEditor/Node/Node.h"

namespace NodeEditor
{
	class ControlParametersNode : public Node
	{
	public:
		ControlParametersNode(NodeEditorBase* editor, const std::string& name);
		virtual ~ControlParametersNode() override {}
		virtual void draw() override;
	};
}
