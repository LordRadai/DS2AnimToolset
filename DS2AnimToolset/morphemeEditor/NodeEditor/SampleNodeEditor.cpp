#include "SampleNodeEditor.h"
#include "extern.h"
#include "RLog/RLog.h"

namespace NodeEditor
{
	bool SampleNodeEditor::initialise()
	{
		bool bInit = NodeEditor::initialise();

		if (!bInit)
			return false;

		BlendTree* rootGraph = createRootBlendTree();

		ControlParameter* cp = createControlParameterFloat(0, "Float");
		createControlParameterBool(1, "Bool");
		createControlParameterInt(2, "Int");
		createControlParameterUInt(3, "UInt");
		createControlParameterVector3(4, "Vector3");
		createControlParameterVector4(5, "Vector4");
		createControlParameterQuaternion(6, "Quaternion");

		createRequest(0, "Default");

		Node* animationNode = rootGraph->createNode(6, "AnimWithEvents", "IdleAnim");
		animationNode->setAttribute("Loop", true);

		Node* node1 = rootGraph->createNode(7, "Blend2");
		Node* node2 = rootGraph->createNode(8, "PassThrough");

		Node* stateMachine = rootGraph->createStateMachine(9, "SM_Main");
		StateMachine* sm = stateMachine->getSubGraph()->asType<StateMachine>();

		Node* src = sm->createBlendTree(10);
		Node* dst = sm->createBlendTree(11);
		sm->createStateNode(8);

		Transition* transit0 = sm->createTransition(6, "Transit", src, dst);
		Transition* transit1 = sm->createTransition(7, "Transit", dst, src);

		transit0->setAttribute("ReverseControlParameter", getControlParameter("Bool"));

		transit0->createCondition("MessageCondition");

		Node* blend2 = src->getSubGraph()->asType<BlendTree>()->createNode(12, "Blend2");

		cp->getOutputPin()->connectTo(node1->getInputPin("Weight"));
		node1->getOutputPin("Result")->connectTo(node2->getInputPin("Source"));

		cp->getOutputPin()->connectTo(blend2->getInputPin("Weight"));
		rootGraph->connectToOutput(node2->getOutputPin("Result"));

		return true;
	}
}
