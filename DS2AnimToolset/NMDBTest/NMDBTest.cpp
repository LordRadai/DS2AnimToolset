#include <iostream>
#include "morphemeConnectCore/morphemeConnectCore.h"

mcc::MorphemeDocument* g_doc = nullptr;
db::Registry* g_registry = nullptr;

int main()
{
	try
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

		g_doc->getMorphemeDB()->createNetwork("Network");
		mcd::Network* network = g_doc->getMorphemeDB()->getNetwork();

		printf_s("--------Creating control parameters--------\n");

		network->addControlParameter(g_doc->createFloatControlParameter("FloatParam", 0.0f, 1.0f, 0.5f));
		network->addControlParameter(g_doc->createVector3ControlParameter("Vector3Param", 0.0f, 10.0f, NMP::Vector3(1.0f, 2.0f, 3.0f)));
		network->addControlParameter(g_doc->createVector4ControlParameter("Vector4Param", 0.0f, 10.0f, NMP::Quat(0.707f, 0.0f, 0.707f, 0.0f)));
		network->addControlParameter(g_doc->createBoolControlParameter("BoolParam", true));
		network->addControlParameter(g_doc->createIntControlParameter("IntParam", -10, 10, 0));
		network->addControlParameter(g_doc->createUIntControlParameter("UIntParam", 0, 100, 50));
		network->addControlParameter(g_doc->createQuaternionControlParameter("QuaternionParam", 0.f, 1.f, NMP::Quat(0.707f, 0.0f, 0.707f, 0.0f)));

		printf_s("--------Control parameters created successfully.--------\n");

		printf_s("--------Creating requests--------\n");

		network->addRequest(g_doc->createRequest("Request0"));
		network->addRequest(g_doc->createRequest("Request1"));

		printf_s("--------Requests created successfully.--------\n");

		printf_s("--------Creating nodes--------\n");

		mcd::BlendTree* rootBt = network->createBlendTreeAsParent();

		mcd::PassDownPin* result = rootBt->getResultPin();

		mcd::BlendTreeNode* pSm = dynamic_cast<mcd::BlendTreeNode*>(g_doc->createNewStateMachine("StateMachine1", manifest->findStateMachineManifest(MANIFEST_NODE_STATE_MACHINE), rootBt));
		mcd::Pin* pin = pSm->getPin("Result");

		pin->connectTo(result);

		printf_s("--------Nodes created successfully.--------\n");

		g_doc->saveAs("testMorphemeDoc.xml");

		//manifest->shutdown();

		delete g_doc;
	}
	catch (const std::exception& e)
	{
		MessageBoxA(NULL, e.what(), "Exception Caught", MB_OK | MB_ICONERROR);
	}
}
