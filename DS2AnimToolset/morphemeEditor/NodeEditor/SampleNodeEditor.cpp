#include "SampleNodeEditor.h"
#include "extern.h"
#include "RLog/RLog.h"
#include "Graph/BlendTree.h"
#include "Graph/StateMachine.h"

namespace NodeEditor
{
	bool SampleNodeEditor::initialise()
	{
		bool bInit = NodeEditor::initialise();

		if (!bInit)
			return false;

		BlendTree* rootGraph = new BlendTree(this, nullptr, "Root");
		pushGraph(rootGraph);

		ControlParameter* cp = createControlParameterFloat("Float");
		createControlParameterBool("Bool");
		createControlParameterInt("Int");
		createControlParameterUInt("UInt");
		createControlParameterVector3("Vector3");
		createControlParameterVector4("Vector4");
		createControlParameterQuaternion("Quaternion");

		Node* node1 = rootGraph->createNode(1, "Blend2");
		node1->createInputPin("Source0");
		node1->createInputPin("Source1");
		node1->createInputDataPin("Weight", DataPin::kDataTypeFloat);

		node1->createOutputPin("Result");

		Node* node2 = rootGraph->createNode(2, "PassThrough");
		node2->createInputPin("Source");
		node2->createOutputPin("Result");

		Node* stateMachine = rootGraph->createStateMachine(3, "SM_Main");
		StateMachine* sm = stateMachine->getSubGraph()->asType<StateMachine>();

		Node* src = sm->createBlendTree(4);
		Node* dst = sm->createBlendTree(5);
		sm->createStateNode(8);

		sm->createTransition(6, "Transit", src, dst);
		sm->createTransition(7, "Transit", dst, src);

		Node* blend2 = src->getSubGraph()->asType<BlendTree>()->createNode(1, "Blend2");
		blend2->createInputPin("Source0");
		blend2->createInputPin("Source1");
		blend2->createInputDataPin("Weight", DataPin::kDataTypeFloat);

		cp->getOutputPin()->connectTo(node1->getInputPin("Weight"));
		node1->getOutputPin("Result")->connectTo(node2->getInputPin("Source"));

		cp->getOutputPin()->connectTo(blend2->getInputPin("Weight"));
		rootGraph->connectToOutput(node2->getOutputPin("Result"));

		return true;
	}
}
