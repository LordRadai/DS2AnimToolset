#include <fstream>
#include <filesystem>
#include "RCore.h"
#include "MCN/MCN.h"
#include "MCN/MCNUtils.h"
#include "MCN/NodeMap.h"

RLog* g_appLog;

int main(int argc, char* argv[])
{
	g_appLog = new RLog(MsgLevel_Debug, "mcnMerger.log", "mcnMerger");

    if (argc < 4)
    {
        g_appLog->alertMessage(MsgLevel_Error, "This program needs at least %d arguments (usage mcnMerger <mcnPath> <netXmlPath> <animLibrary>)", 4);
        return 0;
    }

	std::filesystem::path mcnPath = argv[1];
	std::filesystem::path netXmlPath = argv[2];
	std::filesystem::path animLibraryPath = argv[3];

	g_appLog->debugMessage(MsgLevel_Info, "Adding entries from %ws to %ws\n", netXmlPath.c_str(), mcnPath.c_str());

	NMP::Memory::init();

	ME::ExportFactoryXML factory;

	ME::NetworkDefExportXML* netDef = static_cast<ME::NetworkDefExportXML*>(factory.loadAsset(RString::toNarrow(netXmlPath).c_str()));
	ME::AnimationLibraryXML* animLibrary = static_cast<ME::AnimationLibraryXML*>(factory.loadAsset(RString::toNarrow(animLibraryPath).c_str()));

	MCN::MCNFile* mcn = MCN::MCNFile::loadFile(RString::toNarrow(mcnPath));

	mcn->buildNodeMap(netDef, animLibrary);

	try
	{
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
				g_appLog->debugMessage(MsgLevel_Info, "Adding ControlParameter node %d (name=%s)\n", i, MCNUtils::getNodeNameWithoutParent(node).c_str());
				mcn->addControlParameter(node);
				break;
			default:
				MCN::NodeMap* nodeMap = mcn->getNodeMap(node->getNodeID());

				std::string nodeName = "";
				if (nodeMap != nullptr)
					nodeName = nodeMap->getName();

				mcn->addNode(i, animLibrary);

				break;
			}
		}

		//Once we built the DB, set all the default states
		mcn->setStateMachinesDefaultStates();
	}
	catch (const std::exception& e)
	{
		g_appLog->panicMessage(e.what());
	}

	for (size_t i = 0; i < netDef->getNumMessages(); i++)
	{
		ME::MessageExportXML* message = static_cast<ME::MessageExportXML*>(netDef->getMessage(i));

		g_appLog->debugMessage(MsgLevel_Info, "Adding request %d (name=%s)\n", i, message->getName());

		mcn->addRequest(message);
	}

	std::string outMcn = mcnPath.replace_extension("").string() + std::string("_merged.mcn");

	g_appLog->debugMessage(MsgLevel_Info, "Saving mcn file to %s\n", outMcn.c_str());

	mcn->setDestFileName(outMcn);
	mcn->save();
	mcn->destroy();

	g_appLog->debugMessage(MsgLevel_Info, "Exit\n");

	NMP::Memory::shutdown();
	delete g_appLog;
}