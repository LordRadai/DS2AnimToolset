#include <iostream>
#include "morphemeConnectCore/morphemeConnectCore.h"
#include "Morpheme4/morpheme/SDK/sharedDefines/mSharedDefines.h"

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

		if (!g_doc->initializeManifest())
			throw std::runtime_error("Failed to initialize Morpheme manifest.");

		mcc::MorphemeManifest* manifest = g_doc->getManifest();

		for (size_t i = 0; i < manifest->getNumRegisteredStateMachines(); i++)
		{
			const int id = manifest->getStateMachineManifest(static_cast<uint32_t>(i))->getID();
			const std::string name = manifest->getStateMachineManifest(static_cast<uint32_t>(i))->getName();

			printf_s("Registered State Machine: ID=%d, Name=%s\n", id, name.c_str());
		}

		for (size_t i = 0; i < manifest->getNumRegisteredNodes(); i++)
		{
			const int id = manifest->getNodeManifest(static_cast<uint32_t>(i))->getID();
			const std::string name = manifest->getNodeManifest(static_cast<uint32_t>(i))->getName();

			printf_s("Registered Node: ID=%d, Name=%s\n", id, name.c_str());
		}

		for (size_t i = 0; i < manifest->getNumRegisteredConditions(); i++)
		{
			const int id = manifest->getConditionManifest(static_cast<uint32_t>(i))->getID();
			const std::string name = manifest->getConditionManifest(static_cast<uint32_t>(i))->getName();

			printf_s("Registered Condition: ID=%d, Name=%s\n", id, name.c_str());
		}

		for (size_t i = 0; i < manifest->getNumRegisteredTransitions(); i++)
		{
			const int id = manifest->getTransitionManifest(static_cast<uint32_t>(i))->getAnimID();
			const std::string name = manifest->getTransitionManifest(static_cast<uint32_t>(i))->getName();

			printf_s("Registered Transition: ID=%d, Name=%s\n", id, name.c_str());
		}

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

		mcd::BlendTreeNode* pEmitRequest = g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_EMITREQUESTONDISCRETEEVENT), rootBt, "");
		mcd::BlendTreeNode* pStateMachine = g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_STATE_MACHINE), rootBt, "");

		mcd::StateMachine* pSM = dynamic_cast<mcd::StateMachine*>(pStateMachine->getGraphEntryNode());
		g_doc->createNewStateMachineNode("BlendTree", pSM, "BlendTree1");
		g_doc->createNewStateMachineNode("StateMachine", pSM, "StateMachine1");

		pSM->setDefaultState(pSM->findStateMachineNode("BlendTree1"));

		dynamic_cast<mcd::RequestAttribute*>(pEmitRequest->findAttribute("EmittedRequest0"))->setValue(network->findRequest("Request0"));
		dynamic_cast<mcd::RequestAttribute*>(pEmitRequest->findAttribute("EmittedRequest1"))->setValue(network->findRequest("Request1"));

		printf_s("--------Nodes created successfully.--------\n");

		g_doc->saveAs("testMorphemeDoc.xml");

		delete g_doc;
	}
	catch (const std::exception& e)
	{
		MessageBoxA(NULL, e.what(), "Exception Caught", MB_OK | MB_ICONERROR);
	}
}
