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
		blend2Node->createInputPin("Source2");
		blend2Node->createInputDataPin("Weight", DataPin::kDataTypeFloat);

		rootBlendTree->connectToOutput(blend2Node->getOutputPin(0));

		Node* walkAnim = createNode("", "AnimWithEvents", 8, "WalkAnim");
		walkAnim->createOutputPin("Result");

		Node* runAnim = createNode("", "AnimWithEvents", 9, "RunAnim");
		runAnim->createOutputPin("Result");

		walkAnim->getOutputPin("Result")->connectTo(blend2Node->getInputPin("Source0"));
		runAnim->getOutputPin("Result")->connectTo(blend2Node->getInputPin("Source2"));
		getControlParameter("Float")->getOutputPin()->connectTo(blend2Node->getInputPin("Weight"));

		return true;
	}
}
