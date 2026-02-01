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

		BlendTree* rootBlendTree = createRootBlendTree();

		createControlParameterFloat(0, "Float");
		createControlParameterBool(1, "Bool");
		createControlParameterInt(2, "Int");
		createControlParameterUInt(3, "UInt");
		createControlParameterVector3(4, "Vector3");
		createControlParameterVector4(5, "Vector4");
		createControlParameterQuaternion(6, "Quaternion");

		Node* blend2Node = createNode("", "Blend2", 7);

		rootBlendTree->connectToOutput(blend2Node->getOutputPin(0));

		Node* idleAnim = createNode("", "AnimWithEvents", 8, "Idle");
		Node* moveStateMachine = createStateMachine("", 9, "SM_Move");

		createBlendTree("SM_Move", 10, "BT_Jump");
		createBlendTree("SM_Move", 11, "BT_Landing");

		connect("Idle.Result", "Blend2_1.Source0");
		connect("SM_Move.Output", "Blend2_1.Source1");
		connect("ControlParameters|Float.Output", "Blend2.Weight");

		return true;
	}
}
