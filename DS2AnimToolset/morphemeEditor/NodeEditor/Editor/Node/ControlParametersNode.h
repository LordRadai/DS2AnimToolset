#pragma once
#include "Node.h"

namespace NodeEditor
{
	class ControlParametersNode : public Node
	{
	public:
		ControlParametersNode(Editor* editor, const std::string& name);

		virtual ~ControlParametersNode() override {}
		virtual void draw() override;
		virtual bool editorGUI() override { return false; }

		void reset();
	};
}
