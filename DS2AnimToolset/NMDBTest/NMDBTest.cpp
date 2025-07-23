#include <iostream>
#include "Database/Database.h"

int main()
{
	printf_s("NMDBTest - NMDatabase Test Application\n");
	db::Database db("testDB", "Test Database");
}
