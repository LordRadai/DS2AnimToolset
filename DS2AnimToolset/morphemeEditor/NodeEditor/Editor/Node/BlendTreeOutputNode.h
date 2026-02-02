#pragma once
#include "Node.h"

namespace NodeEditor
{
	class BlendTreeOutputNode : public Node
	{
	public:
		BlendTreeOutputNode(NodeEditor* editor, Graph* parent);

		virtual ~BlendTreeOutputNode() override {}
		virtual void draw() override;
		virtual bool editorGUI() override { return false; }

		Pin* getOutputPin() const { return getInputPin(0); }
	};
}
