#include "SampleNodeEditor.h"
#include "extern.h"
#include "RLog/RLog.h"
#include "Graph/BlendTree.h"
#include "Graph/StateMachine.h"

namespace NodeEditor
{
	bool SampleNodeEditor::initialise()
	{
		bool bInit = NodeEditorBase::initialise();

		if (!bInit)
			return false;

		BlendTree* rootGraph = new BlendTree(this, nullptr, "Root");
		pushGraph(rootGraph);

		ControlParameter* cp = createControlParameterFloat("Speed");

		Node* node1 = rootGraph->createNode(1, "Blend2");
		node1->createInputPin("Source0");
		node1->createInputPin("Source1");
		node1->createInputDataPin("Weight", DataPin::kDataTypeFloat);

		node1->createOutputPin("Result");

		Node* node2 = rootGraph->createNode(2, "Node2");
		node2->createInputPin("Source");
		node2->createOutputPin("Result");

		Node* stateMachine = rootGraph->createStateMachine(3, "StateMachine");
		StateMachine* subGraph = stateMachine->getSubGraph()->asType<StateMachine>();

		Node* src = subGraph->createBlendTree(4, "BlendTree1");
		Node* dst = subGraph->createBlendTree(5, "BlendTree2");
		subGraph->createTransition(6, src, dst);
		subGraph->createTransition(7, dst, src);

		Node* blend2 = src->getSubGraph()->asType<BlendTree>()->createNode(1, "Blend2");
		blend2->createInputPin("Source0");
		blend2->createInputPin("Source1");
		blend2->createInputDataPin("Weight", DataPin::kDataTypeFloat);

		cp->getOutputPin()->connectTo(node1->getInputPin("Weight"));
		node1->getOutputPin("Result")->connectTo(node2->getInputPin("Source"));

		cp->getOutputPin()->connectTo(blend2->getInputPin("Weight"));

		return true;
	}
}
