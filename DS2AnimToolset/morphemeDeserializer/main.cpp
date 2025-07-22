#include "Database\Database.h"
#include "RCore.h"

RLog* g_appLog = nullptr;

void testDb()
{
	std::filesystem::create_directories("MorphemeDeserializer");

	db::Database database;
	
	db::Network* network = database.addNetwork();
	network->addAnimationLocation("$(RootDir)\Characters\MaleCharacter\Animation\XMD\"", "$(RootDir)\Characters\MaleCharacter\morphemeMarkup\"", true);
	
	db::AnimationSet* animSet = network->addAnimationSet("MaleCharacter");

	animSet->setRig("$(RootDir)/Characters/MaleCharacter/morphemeRigs/maleCharacterAnimationRig.mcarig");
	animSet->addSkin("maleCharacterAnimationRig", "$(RootDir)/Characters/MaleCharacter/morphemeRigs/maleCharacterAnimationRig.mcskin");
	animSet->setAssetManagerSkin(animSet->getSkin(0));
	animSet->addChannelName("CharacterWorldSpaceTM");

	network->setAnimLibraryGUID("d805d223-e4c0-4b27-883b-943d54f9ceb1");
	network->setAssetManagerSelectedSet("MaleCharacter");
	network->addPreviewScript("MorphemeDeserializer//DefaultPreviewScript.lua");
	
	database.exportXML("MorphemeDeserializer//testDatabase.mcn");
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
