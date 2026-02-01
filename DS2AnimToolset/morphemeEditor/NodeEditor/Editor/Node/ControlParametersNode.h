#pragma once
#include "Node.h"

namespace NodeEditor
{
	class ControlParametersNode : public Node
	{
	public:
		ControlParametersNode(NodeEditor* editor, const std::string& name);

		virtual ~ControlParametersNode() override {}
		virtual void draw() override;
		virtual void editorGUI() override {}
	};
}
