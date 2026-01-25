#include <iostream>
#include "morphemeConnectCore/morphemeConnectCore.h"

mcc::MorphemeDocument* g_doc = nullptr;
db::Registry* g_registry = nullptr;

void btRootWithBlend2NodeExample(mcc::MorphemeManifest* manifest)
{
	mcd::MorphemeDB* morphemeDB = g_doc->getMorphemeDB();
	morphemeDB->removeNetwork();

	morphemeDB->createNetwork("Network");
	mcd::Network* network = morphemeDB->getNetwork();

	network->addControlParameter(g_doc->createFloatControlParameter("FloatParam", 0.0f, 1.0f, 0.5f));

	mcd::BlendTree* rootBt = network->createBlendTreeAsParent();

	mcd::PassDownPin* networkResult = rootBt->getResultPin();

	mcd::BlendTreeNode* pBlend2 = g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_BLEND2), rootBt, "Blend2_1");
	mcd::FunctionalPin* source0 = pBlend2->getPin("Source0")->asFunctionalPin();
	mcd::FunctionalPin* source1 = pBlend2->getPin("Source1")->asFunctionalPin();
	mcd::FunctionalPin* result = pBlend2->getPin("Result")->asFunctionalPin();
	mcd::DataPin* weight = pBlend2->getPin("Weight")->asDataPin();

	network->findControlParameter("FloatParam")->getResultDataPin()->connectTo(weight);
	result->connectTo(networkResult);

	mcd::BlendTreeNode* bt1 = g_doc->createNewBlendTree("BlendTree1", rootBt);
	mcd::BlendTreeNode* bt2 = g_doc->createNewBlendTree("BlendTree2", rootBt);

	bt1->getResultPin()->connectTo(source0);
	bt2->getResultPin()->connectTo(source1);

	g_doc->saveAs("btRootWithBlend2Node.xml");
}

void btRootWithNestedSMExample(mcc::MorphemeManifest* manifest)
{
	mcd::MorphemeDB* morphemeDB = g_doc->getMorphemeDB();
	morphemeDB->removeNetwork();

	morphemeDB->createNetwork("Network");
	mcd::Network* network = morphemeDB->getNetwork();

	mcd::BlendTree* rootBt = network->createBlendTreeAsParent();

	mcd::BlendTreeNode* sm = dynamic_cast<mcd::BlendTreeNode*>(g_doc->createNewStateMachine("StateMachine1", manifest->findStateMachineManifest(MANIFEST_NODE_STATE_MACHINE), rootBt));

	mcd::StateMachine* smGraph = dynamic_cast<mcd::StateMachine*>(sm->getGraphEntryNode());

	g_doc->createNewStateMachine("SM_Main", manifest->findStateMachineManifest(MANIFEST_NODE_STATE_MACHINE), smGraph);
	g_doc->createNewBlendTree("SM_BlendTree1", smGraph);
	g_doc->createStateMachineNode(manifest->findStateMachineNodeManifest(MANIFEST_STATE_MACHINE_NODE_ACTIVESTATE), smGraph, "ActiveState1");

	g_doc->saveAs("btRootWithNestedSM.xml");
}

void emittedControlParametersExample(mcc::MorphemeManifest* manifest)
{
	mcd::MorphemeDB* morphemeDB = g_doc->getMorphemeDB();

	morphemeDB->removeNetwork();
	morphemeDB->createNetwork("Network");

	mcd::Network* network = morphemeDB->getNetwork();
	mcd::BlendTree* rootBt = network->createBlendTreeAsParent();

	mcd::EmittedControlParameter* emittedParam = g_doc->createFloatEmittedControlParameter("EmittedFloatParam", 0.f, 1.f, 0.0f);
	network->addEmittedControlParameter(emittedParam);

	mcd::BlendTreeNode* noiseGen = g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORNOISEGEN), rootBt, "OperatorNoiseGen1");
	mcd::DataPin* input = noiseGen->getPin("Input")->asDataPin();
	mcd::DataPin* output = noiseGen->getPin("Result")->asDataPin();

	mcd::ControlParameter* cp = emittedParam->getControlParameter();

	cp->getResultDataPin()->connectTo(input);
	output->connectTo(emittedParam->getOutputPin());

	g_doc->saveAs("emittedControlParametersExample.xml");
}

int main()
{
	g_registry = db::Registry::getInstance();
	g_registry->enableRuntimeIDGeneration(true);

	g_doc = new mcc::MorphemeDocument();

	printf_s("--------Loading manifest--------\n");

	mcc::MorphemeManifest* manifest = g_doc->getManifest();

	if (!manifest->init())
		throw std::runtime_error("Failed to initialize Morpheme manifest.");

	manifest->printRegisteredManifests();

	printf_s("--------Manifest loaded successfully.--------\n");

	btRootWithBlend2NodeExample(manifest);
	btRootWithNestedSMExample(manifest);
	emittedControlParametersExample(manifest);

	//manifest->shutdown();

	delete g_doc;
}
