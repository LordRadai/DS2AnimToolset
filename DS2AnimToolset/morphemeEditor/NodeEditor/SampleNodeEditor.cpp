#include "SampleNodeEditor.h"
#include "extern.h"
#include "RLog/RLog.h"

namespace NodeEditor
{
	bool SampleNodeEditor::initialise()
	{
		bool bInit = NodeEditorBase::initialise();

		if (!bInit)
			return false;

		Graph* rootGraph = new Graph();
		pushGraph(rootGraph);

		Node* node1 = rootGraph->addNode(1, "Node1");
		node1->addInputPin("Source0");
		node1->addInputPin("Source1");

		node1->addOutputPin("Result");

		Node* node2 = rootGraph->addNode(2, "Node2");
		node2->addInputPin("Source");
		node2->addOutputPin("Result");

		Node* stateMachine = rootGraph->addContainerNode(3, "StateMachine");
		Graph* subGraph = stateMachine->getSubGraph();

		subGraph->addNode(4, "SubNode1");

		if (!node1->getOutputPin("Result")->connectTo(node2->getInputPin("Source")))
			g_appLog->debugMessage(MsgLevel_Error, "Failed to connect pins\n");

		return true;
	}
}
