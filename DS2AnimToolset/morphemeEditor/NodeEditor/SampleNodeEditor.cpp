#include "SampleNodeEditor.h"

namespace NodeEditor
{
	bool SampleNodeEditor::initialise()
	{
		bool bInit = NodeEditorBase::initialise();

		if (!bInit)
			return false;

		m_currentGraph = new Graph();

		Node* node1 = m_currentGraph->addNode(1, "Sample Node 1");
		node1->addInputPin("Source0");
		node1->addInputPin("Source1");

		node1->addOutputPin("Result");

		Node* node2 = m_currentGraph->addNode(2, "Sample Node 2");
		node2->addInputPin("Source");
		node2->addOutputPin("Result");

		node1->getOutputPin("Result")->connectTo(node2->getInputPin("Source"));
	}
}
