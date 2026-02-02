#pragma once
#include "NodeEditor.h"

namespace NodeEditor
{
	class SampleNodeEditor : public NodeEditor
	{
	public:
		SampleNodeEditor(int flags = 0) : NodeEditor(flags) {}
		~SampleNodeEditor() override {}
		
		virtual bool initialise() override;
	};
}
