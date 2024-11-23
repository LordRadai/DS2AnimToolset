#include <fstream>
#include <filesystem>
#include "RCore.h"

RLog* g_appLog;

int getAnimIDByFilename(const std::wstring& filename) 
{
	std::wstring numericString = L"";
	std::wstring animIdStr = filename.substr(1, 12);

	for (char ch : animIdStr)
	{
		if (std::isdigit(ch))
			numericString += ch;
	}

	int animId = !numericString.empty() ? std::stoi(numericString) : 0;

	g_appLog->debugMessage(MsgLevel_Debug, "animId=%d, filename=%ws\n", animId, filename.c_str());

	return animId;
}

bool getExtAnimList(std::wstring path, std::unordered_map<int, int>& dataMap)
{
	g_appLog->debugMessage(MsgLevel_Info, "Loading extanibnd file list at %ws\n", path.c_str());

	std::ifstream inputFile(path);

	if (!inputFile) 
	{
		g_appLog->debugMessage(MsgLevel_Error, "Input file %ws does not exist\n", path.c_str());

		return false;
	}

	std::string line;

	int numLines = 0;
	while (std::getline(inputFile, line)) 
	{
		std::istringstream lineStream(line);
		std::string animIdStr, chrIdStr;

		if (std::getline(lineStream, animIdStr, ',') && std::getline(lineStream, chrIdStr))
		{
			int animID = std::stoi(animIdStr);
			int chrID = std::stoi(chrIdStr);

			dataMap.insert(std::pair<int, int>(animID, chrID));
		}

		numLines++;
	}

	g_appLog->debugMessage(MsgLevel_Info, "Read %d entries from %ws\n", numLines, path.c_str());

	inputFile.close();

	for (const auto& [key, value] : dataMap)
		g_appLog->debugMessage(MsgLevel_Debug, "\t(animId=%d, chrId=%d)\n", key, value);

	return true;
}

void createWitchyBndXml(std::string filename, std::string binderFileName)
{
	tinyxml2::XMLDocument doc;
	doc.NewDeclaration("version = \"1.0\" encoding=\"utf-8\"");

	tinyxml2::XMLElement* root = doc.NewElement("anibnd4");

	root->InsertNewChildElement("filename")->SetText(binderFileName.c_str());
	root->InsertNewChildElement("compression")->SetText("DCX_DFLT_10000_24_9");
	root->InsertNewChildElement("version")->SetText("10G29R1");
	root->InsertNewChildElement("format")->SetText("IDs, Names1, Names2, Compression");
	root->InsertNewChildElement("bigendian")->SetText("False");
	root->InsertNewChildElement("bitbigendian")->SetText("False");
	root->InsertNewChildElement("unicode")->SetText("True");
	root->InsertNewChildElement("extended")->SetText("0x01");
	root->InsertNewChildElement("unk04")->SetText("False");
	root->InsertNewChildElement("unk05")->SetText("False");

	doc.InsertEndChild(root);

	doc.SaveFile(filename.c_str());
}

int main(int argc, char* argv[])
{
	g_appLog = new RLog(MsgLevel_Debug, "morphemeBinderPacker.log", "morphemeBinderPacker");

	if (argc < 2)
	{
		g_appLog->alertMessage(MsgLevel_Error, "This program needs at least %d arguments (usage morphemeBinderPacker <filepath>)", 2);
		return 0;
	}

	std::filesystem::path filepath = argv[1];

	std::filesystem::path runtimeBinary = filepath.wstring() + L"\\runtimeBinary";

	if (!std::filesystem::exists(runtimeBinary))
	{
		g_appLog->alertMessage(MsgLevel_Error, "Could not find folder %ws", runtimeBinary.c_str());
		return 1;
	}

	std::wstring chrIdStr = filepath.filename().wstring();
	int chrId = std::stoi(chrIdStr.substr(1, 4));
	std::unordered_map<int, int> animIdChrIdMap;

	if (chrId == 1)
		getExtAnimList(L"extanimlist.txt", animIdChrIdMap);

	std::filesystem::path outputFolder = filepath.wstring() + L"\\binders";

	for (const auto& entry : std::filesystem::directory_iterator(runtimeBinary)) 
	{
		if (entry.is_regular_file() && (entry.path().extension() == ".nsa" || entry.path().extension() == ".nmb")) 
		{
			std::wstring filename = entry.path().filename().wstring();
			std::filesystem::path destinationFile = std::filesystem::path(outputFolder).wstring() + L"\\" + chrIdStr + L"\\runtimeBinary\\" + filename;
			
			CHAR binderFileName[256];
			sprintf_s(binderFileName, "%ws.anibnd.dcx", chrIdStr.c_str());

			if (entry.path().extension() == ".nsa")
			{
				int animId = getAnimIDByFilename(filename);

				if (chrId == 1)
				{
					int targetChrId = -1;

					if (animIdChrIdMap.find(animId) != animIdChrIdMap.end())
						targetChrId = animIdChrIdMap[animId];

					if (targetChrId != -1)
					{
						wchar_t extAnibndFolderName[256];
						wsprintf(extAnibndFolderName, L"c0001_%d", targetChrId);

						destinationFile = std::filesystem::path(outputFolder).wstring() + L"\\" + chrIdStr + L"\\" + extAnibndFolderName + L"\\" + filename;
					
						sprintf_s(binderFileName, "%ws.extanibnd.dcx", extAnibndFolderName);
					}
				}
			}

			std::filesystem::create_directories(destinationFile.parent_path());
			createWitchyBndXml(destinationFile.parent_path().string() + "\\_witchy-anibnd4.xml", binderFileName);

			std::filesystem::copy(entry.path(), destinationFile, std::filesystem::copy_options::overwrite_existing);

			g_appLog->debugMessage(MsgLevel_Info, "Copied \"%ws\" to \"%ws\"\n", entry.path().c_str(), destinationFile.c_str());
		}
	}
}