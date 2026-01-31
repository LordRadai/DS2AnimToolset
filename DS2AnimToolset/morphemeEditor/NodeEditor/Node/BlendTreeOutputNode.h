#pragma once
#include "NodeEditor/Node/Node.h"

namespace NodeEditor
{
	class BlendTreeOutputNode : public Node
	{
	public:
		BlendTreeOutputNode(NodeEditor* editor, Graph* parent);
		//BlendTreeOutputNode(NodeEditor* editor, Graph* parent, Project::ProjectNode* projectNode, Graph* subGraph);
		virtual ~BlendTreeOutputNode() override {}
		virtual void draw() override;
		virtual void editorGUI() override {}

		Pin* getOutputPin() const { return getInputPin(0); }
	};
}
