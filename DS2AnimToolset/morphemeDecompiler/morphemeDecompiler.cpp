#include <iostream>
#include <RCore.h>
#include "extern.h"

RLog* g_appLog;

/*
* Usage: morphemeDecompiler.exe --in <path> --out <path> --animFmt <xmd/fbx/gltf> --fps <fps>
*/
int main(int argc, char* argv[])
{
	g_appLog = new RLog(MsgLevel_Debug, "morphemeDecompiler.log", "morphemeDecompiler");

	if (argc < 12)
	{
		g_appLog->alertMessage(MsgLevel_Error, "This program needs at least %d arguments (usage morphemeDecompiler --in <path> --out <path> --animFmt <xmd/fbx/gltf> --fps <fps>)", 6);
		return 0;
	}

	std::string inputPath = "";
	std::string outputPath = "";
	std::string animFormat = "";
	int fps = 30;

	g_appLog->debugMessage(MsgLevel_Info, "Invoked morphemeDecompiler with arguments:\n", argc);

	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--in") == 0 && i + 1 < argc)
		{
			inputPath = argv[++i];
			g_appLog->debugMessage(MsgLevel_Info, "\tInput path: %s\n", inputPath.c_str());
		}
		else if (strcmp(argv[i], "--out") == 0 && i + 1 < argc)
		{
			outputPath = argv[++i];
			g_appLog->debugMessage(MsgLevel_Info, "\tOutput path: %s\n", outputPath.c_str());
		}
		else if (strcmp(argv[i], "--animFmt") == 0 && i + 1 < argc)
		{
			animFormat = argv[++i];
			g_appLog->debugMessage(MsgLevel_Info, "\tAnimation format: %s\n", animFormat.c_str());
		}
		else if (strcmp(argv[i], "--fps") == 0 && i + 1 < argc)
		{
			fps = std::stoi(argv[++i]);
			g_appLog->debugMessage(MsgLevel_Info, "\tFPS: %d\n", fps);
		}
	}
}