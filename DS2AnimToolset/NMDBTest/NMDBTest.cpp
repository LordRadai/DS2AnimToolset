#include <iostream>
#include "morphemeConnectCore/morphemeConnectCore.h"
#include "Morpheme4/morpheme/SDK/sharedDefines/mSharedDefines.h"

mcc::MorphemeDocument* g_doc;

int main()
{
	g_doc = new mcc::MorphemeDocument();

	printf_s("--------Loading manifest--------\n");

	if (g_doc->initializeManifest())
		printf_s("--------Manifest loaded successfully.--------\n");
	else
		printf_s("--------Failed to load manifest.--------\n");

	mcc::MorphemeManifest* manifest = g_doc->getManifest();

	printf_s("--------Creating control parameters--------\n");

	g_doc->getMorphemeDB()->createNetwork("Network");
	mcd::Network* network = g_doc->getMorphemeDB()->getNetwork();

	mcd::BlendTree* rootBt = network->createBlendTreeAsParent();
	g_doc->createBlendTreeNode(manifest->findNodeManifest(NODE_TYPE_ANIM_EVENTS), rootBt, "AnimWithEvents");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(NODE_TYPE_SWITCH), rootBt, "Switch");

	network->addControlParameter(g_doc->createFloatControlParameter("FloatParam", 0.0f, 1.0f, 0.5f));
	network->addControlParameter(g_doc->createVector3ControlParameter("Vector3Param", 0.0f, 10.0f, NMP::Vector3(1.0f, 2.0f, 3.0f)));
	network->addControlParameter(g_doc->createVector4ControlParameter("Vector4Param", 0.0f, 10.0f, NMP::Quat(0.707f, 0.0f, 0.707f, 0.0f)));
	network->addControlParameter(g_doc->createBoolControlParameter("BoolParam", true));
	network->addControlParameter(g_doc->createIntControlParameter("IntParam", -10, 10, 0));
	network->addControlParameter(g_doc->createUIntControlParameter("UIntParam", 0, 100, 50));
	network->addControlParameter(g_doc->createQuaternionControlParameter("QuaternionParam", 0.f, 1.f, NMP::Quat(0.707f, 0.0f, 0.707f, 0.0f)));

	printf_s("--------Control parameters created successfully.--------\n");

	g_doc->saveAs("testMorphemeDoc.xml");

	delete g_doc;
}
