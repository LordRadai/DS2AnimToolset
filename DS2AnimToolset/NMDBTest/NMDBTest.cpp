#include <iostream>
#include "morphemeConnectCore/morphemeConnectCore.h"

int main()
{
	printf_s("NMDBTest - NMDatabase Test Application\n");
	
	try
	{
		mcc::MorphemeDocument doc;
		printf_s("Creating control parameters...\n");
		auto floatParam = doc.createFloatControlParameter("FloatParam", 0.0f, 1.0f, 0.5f);
		auto vector3Param = doc.createVector3ControlParameter("Vector3Param", 0.0f, 10.0f, NMP::Vector3(1.0f, 2.0f, 3.0f));
		auto vector4Param = doc.createVector4ControlParameter("Vector4Param", 0.0f, 10.0f, NMP::Quat(0.707f, 0.0f, 0.707f, 0.0f));
		auto boolParam = doc.createBoolControlParameter("BoolParam", true);
		auto intParam = doc.createIntControlParameter("IntParam", -10, 10, 0);
		auto uintParam = doc.createUIntControlParameter("UIntParam", 0, 100, 50);
		auto quaternionParam = doc.createQuaternionControlParameter("QuaternionParam", 0.f, 1.f, NMP::Quat(0.707f, 0.0f, 0.707f, 0.0f));
		printf_s("Control parameters created successfully.\n");

		doc.saveAs("testMorphemeDoc.xml");
	}
	catch (const std::exception& e)
	{
		printf_s("Exception: %s\n", e.what());
	}
}
