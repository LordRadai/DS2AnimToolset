#include <iostream>
#include "mcd/mcd.h"

int main()
{
	printf_s("NMDBTest - NMDatabase Test Application\n");
	
	try
	{
		mcd::MorphemeDB db;

		db.makeNode(new mcd::BoolAttribute(&db, "testBoolAttribute", true));
		db.exportXML("test.xml");
	}
	catch (const std::exception& e)
	{
		printf_s("Exception: %s\n", e.what());
	}
}
