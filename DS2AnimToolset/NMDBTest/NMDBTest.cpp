#include <iostream>
#include "morphemeConnectCore/morphemeConnectCore.h"
#include "Morpheme4/morpheme/SDK/sharedDefines/mSharedDefines.h"

mcc::MorphemeDocument* g_doc;

int main()
{
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

	printf_s("--------Creating nodes--------\n");

	mcd::BlendTree* rootBt = network->createBlendTreeAsParent();

	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_ANIMWITHEVENTS), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_APPLYBINDPOSE), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_APPLYGLOBALTIME), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_BASICUNEVENTERRAIN), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_BLEND2), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_BLEND2X2), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_BLENDALL), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_BLENDN), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_BLENDNXM), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_CLOSESTANIM), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_EMITREQUESTONDISCRETEEVENT), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_EXTRACTJOINTINFO), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_FEATHERBLEND2), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_FILTERTRANSFORMS), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_FREEZE), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_GUNAIMIK), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_HEADLOOK), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_HIPSIK), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_LASTFRAMESTRANSFORMS), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_LOCKFOOT), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_MIRRORTRANSFORMS), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_PASSTHROUGH), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_PLAYSPEEDMODIFIER), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_PREDICTIVEUNEVENTERRAIN), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_RETARGET), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_SCALECHARACTER), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_SCALETODURATION), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_SEQUENCE), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_SINGLEFRAME), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_SMOOTHTRANSFORMS), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_SUBTRACTIVEBLEND), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_SWITCH), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_TWOBONEIK), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORARITHMETIC), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORFLOATSTOVECTOR3), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORFUNCTION), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORNOISEGEN), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORONEINPUTARITHMETIC), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORRAMPFLOAT), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORRANDOMFLOAT), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORRATEOFCHANGE), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORRAYCAST), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORRERANGE), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORSMOOTHFLOAT), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORSMOOTHVECTOR3), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORVECTOR3ANGLE), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORVECTOR3ARITHMETIC), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORVECTOR3CROSSPRODUCT), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORVECTOR3DISTANCE), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORVECTOR3DOT), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORVECTOR3LENGTH), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORVECTOR3NORMALISE), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORVECTOR3ONEINPUTARITHMETIC), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORVECTOR3RATEOFCHANGE), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_OPERATORVECTOR3TOFLOATS), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_ACTIVEANIMATION), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_HARDKEYFRAME), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_LOCALSOFTKEYFRAME), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_RAGDOLL), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_SOFTKEYFRAME), rootBt, "");
	g_doc->createBlendTreeNode(manifest->findNodeManifest(MANIFEST_NODE_SOFTKEYFRAMEANDACTIVEANIMATION), rootBt, "");

	printf_s("--------Nodes created successfully.--------\n");

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

	g_doc->saveAs("testMorphemeDoc.xml");

	delete g_doc;
}
