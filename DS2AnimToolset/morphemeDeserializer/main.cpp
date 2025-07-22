#include "RCore.h"

RLog* g_appLog = nullptr;

int main(int argc, char* argv[]) 
{
	g_appLog = new RLog(MsgLevel::MsgLevel_Debug, "morphemeDeserializer.log", "Morpheme Deserializer");

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
