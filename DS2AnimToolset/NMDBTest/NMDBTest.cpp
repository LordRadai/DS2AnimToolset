#include <iostream>
#include "Database/Database.h"

int main()
{
	printf_s("NMDBTest - NMDatabase Test Application\n");
	
	try
	{
		db::Database db("testDB", "Test Database");

		db.makeNode(new db::Node(&db, "node1", "Node 1"));
		db::Node* node = db.find("Node 1");
		node->addBoolAttribute("boolAttr", true);
		node->addFloatAttribute("floatAttr", 3.14f);
		node->addIntAttribute("intAttr", 42);
		node->addStringAttribute("stringAttr", "Hello World");
		node->addEnumAttribute("enumAttr", "enumValue0");
		node->addMatrix34Attribute("matrix34Attr", NMP::Matrix34Identity());
		node->addQuaternionAttribute("quaternionAttr", NMP::QuatIdentity());
		node->addVector3Attribute("vector3Attr", NMP::Vector3(1.0f, 2.0f, 3.0f));

		db::Attribute* boolAttr = node->findAttribute("boolAttr");

		if (boolAttr->getParentAttribute() != node)
			printf_s("Error: boolAttr parent is not node\n");

		db.exportXML("test.xml");
	}
	catch (const std::exception& e)
	{
		printf_s("Exception: %s\n", e.what());
	}
}
