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

		Graph* rootGraph = new Graph(nullptr, "Root");
		pushGraph(rootGraph);

		rootGraph->createControlParameterFloat("Speed");

		Node* node1 = rootGraph->createNode(1, "Node1");
		node1->createInputPin("Source0");
		node1->createInputPin("Source1");

		node1->createOutputPin("Result");

		Node* node2 = rootGraph->createNode(2, "Node2");
		node2->createInputPin("Source");
		node2->createOutputPin("Result");

		Node* node5 = rootGraph->createNode(5, "NodeWithAVeryLongNameItsSoLongItShouldExtendTheNormalNodeWidth");
		node5->createInputPin("Source");
		node5->createOutputPin("Result");

		Node* stateMachine = rootGraph->createContainerNode(3, "NodeContainer");
		Graph* subGraph = stateMachine->getSubGraph();

		Node* src = subGraph->createNode(4, "Node1");
		Node* dst = subGraph->createContainerNode(5, "NodeContainer");
		subGraph->createTransition(6, src, dst);
		subGraph->createTransition(7, dst, src);

		if (!node1->getOutputPin("Result")->connectTo(node2->getInputPin("Source")))
			g_appLog->debugMessage(MsgLevel_Error, "Failed to connect pins\n");

		return true;
	}
}
