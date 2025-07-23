#include <iostream>
#include "Database/Database.h"

int main()
{
	printf_s("NMDBTest - NMDatabase Test Application\n");
	db::Database db("testDB", "Test Database");

	db.add(new db::Node(&db, "node1", "Node 1"));
	db::Node* node = db.find("Node 1");
	node->addBoolAttribute("boolAttr", true);
	node->addFloatAttribute("floatAttr", 3.14f);
	node->addIntAttribute("intAttr", 42);
	node->addStringAttribute("stringAttr", "Hello World");
	node->addEnumAttribute("enumAttr", "enumValue0");
}
