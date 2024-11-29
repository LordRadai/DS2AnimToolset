#include "extern.h"
#include "RCore.h"

namespace utils
{
	std::wstring findGamePath(std::wstring current_path)
	{
		std::filesystem::path gamepath = current_path;

		do
		{
			std::wstring parent_path = gamepath.parent_path();
			gamepath = parent_path;

			int lastDirPos = parent_path.find_last_of(L"\\");

			std::wstring folder = parent_path.substr(lastDirPos, parent_path.length());

			if (folder.compare(L"\\") == 0)
				return L"";

			if (folder.compare(L"\\Game") == 0)
				return gamepath;

		} while (true);

		return L"";
	}

	std::vector<std::wstring> getTaeFileListFromChrId(std::wstring tae_path, int chrId)
	{
		wchar_t chrIdStr[256];
		swprintf_s(chrIdStr, L"%04d", chrId);

		std::vector<std::wstring> files;

		g_appLog->debugMessage(MsgLevel_Debug, "TimeAct belonging to c%04d:\n", chrId);

		for (const auto& entry : std::filesystem::directory_iterator(tae_path))
		{
			if (entry.path().extension().compare(".tae") == 0)
			{
				std::wstring filename = entry.path().filename();
				std::wstring filenameChrId = filename.substr(1, 4);

				if (filenameChrId.compare(chrIdStr) == 0)
				{
					g_appLog->debugMessage(MsgLevel_Debug, "\t%ws\n", filename.c_str());
					files.push_back(entry.path());
				}
			}
		}

		if (files.size() == 0)
			g_appLog->alertMessage(MsgLevel_Debug, "Could not find any TimeAct files belonging to c%d in %ws\n", chrId, tae_path);

		return files;
	}

	std::string extractTimeActFilePrefix(std::string filename)
	{
		std::string name = std::filesystem::path(filename).filename().string();

		if (name.at(0) == 'o')
			return filename;

		size_t underscorePos = filename.find_last_of('_');

		if (underscorePos != std::string::npos)
			return filename.substr(0, underscorePos);

		return "";
	}

	std::string extractTimeActFileSuffix(std::string filename)
	{
		std::string name = std::filesystem::path(filename).filename().string();

		if (name.at(0) == 'o')
			return "";

		size_t underscorePos = filename.find('_');
		size_t dotPos = filename.find('.');

		if ((underscorePos != std::string::npos) && (dotPos != std::string::npos) && (dotPos > underscorePos))
			return filename.substr(underscorePos + 1, dotPos - underscorePos - 1);

		return "";
	}
}