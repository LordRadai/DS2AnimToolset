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
		blend2Node->createOutputPin("Result");
		blend2Node->createInputPin("Source0");
		blend2Node->createInputPin("Source1");
		blend2Node->createInputDataPin("Weight", DataPin::kDataTypeFloat);

		rootBlendTree->connectToOutput(blend2Node->getOutputPin(0));

		Node* idleAnim = createNode("", "AnimWithEvents", 8, "Idle");
		idleAnim->createOutputPin("Result");

		Node* moveStateMachine = createStateMachine("", 9, "SM_Move");

		idleAnim->getOutputPin("Result")->connectTo(blend2Node->getInputPin("Source0"));
		moveStateMachine->getOutputPin("Result")->connectTo(blend2Node->getInputPin("Source1"));

		getControlParameter("Float")->getOutputPin()->connectTo(blend2Node->getInputPin("Weight"));

		return true;
	}
}
