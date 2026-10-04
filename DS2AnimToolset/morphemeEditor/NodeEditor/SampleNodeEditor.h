#pragma once
#include "NodeEditor.h"

namespace NodeEditor
{
	class SampleNodeEditor : public Editor
	{
	public:
		SampleNodeEditor(int flags = 0) : Editor(flags) {}
		~SampleNodeEditor() override {}
		
		virtual bool initialise() override;
	};
}
