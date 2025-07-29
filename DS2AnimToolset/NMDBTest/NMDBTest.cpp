#include <iostream>
#include "morphemeConnectCore/morphemeConnectCore.h"

int main()
{
	printf_s("NMDBTest - NMDatabase Test Application\n");
	
	try
	{
		mcc::MorphemeDocument doc;

		printf_s("--------Creating control parameters--------\n");

		doc.getMorphemeDB()->createNetwork("Network");
		mcd::Network* network = doc.getMorphemeDB()->getNetwork();

		network->addControlParameter(doc.createFloatControlParameter("FloatParam", 0.0f, 1.0f, 0.5f));
		network->addControlParameter(doc.createVector3ControlParameter("Vector3Param", 0.0f, 10.0f, NMP::Vector3(1.0f, 2.0f, 3.0f)));
		network->addControlParameter(doc.createVector4ControlParameter("Vector4Param", 0.0f, 10.0f, NMP::Quat(0.707f, 0.0f, 0.707f, 0.0f)));
		network->addControlParameter(doc.createBoolControlParameter("BoolParam", true));
		network->addControlParameter(doc.createIntControlParameter("IntParam", -10, 10, 0));
		network->addControlParameter(doc.createUIntControlParameter("UIntParam", 0, 100, 50));
		network->addControlParameter(doc.createQuaternionControlParameter("QuaternionParam", 0.f, 1.f, NMP::Quat(0.707f, 0.0f, 0.707f, 0.0f)));

		printf_s("--------Control parameters created successfully.--------\n");

		doc.saveAs("testMorphemeDoc.xml");

		printf_s("--------Loading manifest test--------\n");

		mcc::MorphemeManifest manifest;
		manifest.registerNode("Data\\manifest\\nodes\\animation\\AnimWithEvents.json");

		printf_s("--------Manifest loaded successfully.--------\n");
	}
	catch (const std::exception& e)
	{
		printf_s("Exception: %s\n", e.what());
	}
}
