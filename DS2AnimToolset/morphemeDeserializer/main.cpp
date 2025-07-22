#include "Database\Database.h"
#include "RCore.h"

RLog* g_appLog = nullptr;

void testDb()
{
	db::Database database;
	
	db::Network* network = database.addNetwork();

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* root = database.serialize(doc.NewElement("PlaceholderRoot"));

	doc.InsertFirstChild(root);

	std::filesystem::create_directories("MorphemeDeserializer");
	doc.SaveFile("MorphemeDeserializer//testDatabase.xml");
}

int main(int argc, char* argv[]) 
{
	g_appLog = new RLog(MsgLevel::MsgLevel_Debug, "morphemeDecompiler.log", "Morpheme Decompiler");

	testDb();

	if (argc < 4)
	{
		g_appLog->alertMessage(MsgLevel::MsgLevel_Error, "Usage: morphemeDeserializer <input file> <output file> <anim_library_path>");
		
		g_appLog->shutdown();
		return 1;
	}

	std::string inputFile = argv[1];
	std::string outputFile = argv[2];
	std::string animLibraryPath = argv[3];

	g_appLog->shutdown();

	return 0;
}
