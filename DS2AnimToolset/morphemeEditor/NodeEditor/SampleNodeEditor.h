#pragma once
#include "NodeEditorBase.h"

namespace NodeEditor
{
	class SampleNodeEditor : public NodeEditorBase
	{
	public:
		SampleNodeEditor() : NodeEditorBase() {}
		~SampleNodeEditor() override {}
		
		virtual bool initialise() override;
	};
}
