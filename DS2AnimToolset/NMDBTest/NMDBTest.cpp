#include <iostream>
#include "Database/Database.h"

int main()
{
	printf_s("NMDBTest - NMDatabase Test Application\n");
	db::Database db("testDB", "Test Database");

	db.add(new db::Node(&db, "node1", "Node 1"));
	db::Node* node = db.find("Node 1");
}
