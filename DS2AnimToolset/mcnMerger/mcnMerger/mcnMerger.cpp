#include <fstream>
#include <filesystem>
#include "RCore.h"
#include "MCN/MCN.h"

int main(int argc, char* argv[])
{
    RLog* log = new RLog(MsgLevel_Debug, "mcnMerger.log", "mcnMerger");

    if (argc < 3)
    {
        log->alertMessage(MsgLevel_Error, "This program needs at least %d arguments (usage mcnMerger <mcnPath> <netXmlPath>)", 3);
        return 0;
    }

	std::filesystem::path mcnPath = argv[1];
	std::filesystem::path netXmlPath = argv[2];

	log->debugMessage(MsgLevel_Info, "Adding entries from %s to %s\n", netXmlPath.c_str(), mcnPath.c_str());

	NMP::Memory::init();

	ME::ExportFactoryXML factory;

	ME::NetworkDefExportXML* netDef = static_cast<ME::NetworkDefExportXML*>(factory.loadAsset(RString::toNarrow(netXmlPath).c_str()));

	MCN::MCNFile* mcn = MCN::MCNFile::loadFile(RString::toNarrow(mcnPath));

	for (size_t i = 0; i < netDef->getNumNodes(); i++)
	{
		ME::NodeExportXML* node = static_cast<ME::NodeExportXML*>(netDef->getNode(i));
		int typeID = node->getTypeID();

		switch (typeID)
		{
		case NODE_TYPE_CP_BOOL:
		case NODE_TYPE_CP_INT:
		case NODE_TYPE_CP_UINT:
		case NODE_TYPE_CP_FLOAT:
		case NODE_TYPE_CP_VECTOR3:
		case NODE_TYPE_CP_VECTOR4:
			log->debugMessage(MsgLevel_Info, "Adding ControlParameter node %d\n", i, typeID);
			mcn->addControlParameter(node);
			break;
		default:
			log->debugMessage(MsgLevel_Info, "Adding node %d (typeId=%d)\n", i, typeID);
			break;
		}
	}

	for (size_t i = 0; i < netDef->getNumMessages(); i++)
	{
		ME::MessageExportXML* message = static_cast<ME::MessageExportXML*>(netDef->getMessage(i));

		log->debugMessage(MsgLevel_Info, "Adding request %d (name=%s)\n", i, message->getName());

		mcn->addRequest(message);
	}

	std::string outMcn = mcnPath.replace_extension("").string() + std::string("_merged.mcn");

	log->debugMessage(MsgLevel_Info, "Saving mcn file to %s\n", outMcn.c_str());

	mcn->setDestFileName(outMcn);
	mcn->save();
	mcn->destroy();

	log->debugMessage(MsgLevel_Info, "Exit\n");

	NMP::Memory::shutdown();
	delete log;
}