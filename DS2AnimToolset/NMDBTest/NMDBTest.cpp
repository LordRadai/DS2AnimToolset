#include <iostream>
#include "morphemeConnectCore/morphemeConnectCore.h"

int main()
{
	printf_s("NMDBTest - NMDatabase Test Application\n");
	
	try
	{
		mcd::MorphemeDB db;

		db.makeNode(new mcd::BoolAttribute(&db, "testBoolAttribute", true));
		db.makeNode(new mcd::IntAttribute(&db, "testIntAttribute", 42));
		db.makeNode(new mcd::FloatAttribute(&db, "testFloatAttribute", 3.14f));
		db.makeNode(new mcd::StringAttribute(&db, "testStringAttribute", "Hello World"));

		mcd::AttributePlaceholder* placeholder = new mcd::AttributePlaceholder(&db, "testAttributePlaceholder");
		placeholder->setMcdAttribute(new mcd::BoolAttribute(placeholder, "boolInPlaceholder", false));
		db.makeNode(placeholder);

		db.exportXML("test.xml");
	}
	catch (const std::exception& e)
	{
		printf_s("Exception: %s\n", e.what());
	}
}
