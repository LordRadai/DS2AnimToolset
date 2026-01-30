#pragma once
#include "NodeEditor.h"

namespace NodeEditor
{
	class SampleNodeEditor : public NodeEditor
	{
	public:
		SampleNodeEditor() : NodeEditor() {}
		~SampleNodeEditor() override {}
		
		virtual bool initialise() override;
	};
}
