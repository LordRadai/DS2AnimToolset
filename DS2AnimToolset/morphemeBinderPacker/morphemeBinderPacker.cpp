#include <fstream>
#include <filesystem>
#include "RCore.h"

RLog* g_appLog;

int getAnimIDByFilename(const std::wstring& filename) 
{
	std::wstring numericString;

	for (char ch : filename) 
	{
		if (std::isdigit(ch))
			numericString += ch;
	}

	return !numericString.empty() ? std::stoi(numericString) : 0;
}

int main(int argc, char* argv[])
{
	g_appLog = new RLog(MsgLevel_Debug, "morphemeBinderPacker.log", "mcnMerger");

	if (argc < 2)
	{
		g_appLog->alertMessage(MsgLevel_Error, "This program needs at least %d arguments (usage morphemeBinderPacker <filepath>)", 2);
		return 0;
	}

	std::filesystem::path filepath = argv[1];

	std::filesystem::path runtimeBinary = filepath.wstring() + L"\\runtimeBinary";

	if (!std::filesystem::exists(runtimeBinary))
	{
		g_appLog->alertMessage(MsgLevel_Error, "Could not find folder %s", runtimeBinary.c_str());
		return 1;
	}

	std::wstring chrIdStr = filepath.filename().wstring();
	int chrId = std::stoi(chrIdStr.substr(1, 4));

	std::filesystem::path outputFolder = filepath.wstring() + L"\\morphemeBinders";

	for (const auto& entry : std::filesystem::directory_iterator(runtimeBinary)) 
	{
		if (entry.is_regular_file() && (entry.path().extension() == ".nsa" || entry.path().extension() == ".nmb")) 
		{
			std::wstring filename = entry.path().filename().wstring();

			if (entry.path().extension() == ".nsa")
			{
				int animId = getAnimIDByFilename(filename);

				if (chrId == 1)
				{

				}
			}


			std::filesystem::path destinationFile = std::filesystem::path(outputFolder).wstring() + L"\\" + chrIdStr + L"\\" + filename;
			std::filesystem::create_directories(destinationFile.parent_path());

			std::filesystem::copy(entry.path(), destinationFile, std::filesystem::copy_options::overwrite_existing);

			g_appLog->debugMessage(MsgLevel_Info, "Copied \"%ws\" to \"%ws\"\n", entry.path().c_str(), destinationFile.c_str());
		}
	}
}