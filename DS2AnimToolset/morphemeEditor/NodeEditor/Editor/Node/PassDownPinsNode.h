#pragma once
#include "Node.h"

namespace NodeEditor
{
	class PassDownPinsNode : public Node
	{
	public:
		PassDownPinsNode(Editor* editor, Graph* parent, int id, const std::string& name);
		
		virtual ~PassDownPinsNode() override {}
		virtual void draw() override;
		virtual bool editorGUI() override { return false; }

		void updatePins();
	};
}
