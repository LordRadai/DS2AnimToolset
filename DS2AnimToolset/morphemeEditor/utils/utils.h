#include <string>
#include <vector>
#include <filesystem>
#include "NMDX/NMDX.h"

namespace utils
{
	std::wstring findGamePath(std::wstring current_path);
	std::vector<std::wstring> getTaeFileListFromChrId(std::wstring tae_path, int chrId);
	std::string extractTimeActFilePrefix(std::string filename);
    std::string extractTimeActFileSuffix(std::string filename);
}