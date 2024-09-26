#include <fstream>
#include <filesystem>
#include "RCore.h"

int main(int argc, char* argv[])
{
    RLog* log = new RLog(MsgLevel_Debug, "bndBuilder.log", "bndBuilder");

    if (argc < 2)
    {
        log->alertMessage(MsgLevel_Error, "This program needs at least two arguments (usage bndBuilder <folderpath>)");
        return 0;
    }

    std::filesystem::path folderPath(argv[1]);

    log->debugMessage(MsgLevel_Info, "Processing file %s\n", folderPath.string().c_str());

    BND4::Bnd4* bnd = BND4::Bnd4::create(false, false, false);
    
    for (const auto& fileEntry : std::filesystem::directory_iterator(folderPath))
    {
        std::string filename = fileEntry.path().filename().string();
        int fileSize = fileEntry.file_size();
        char* fileBuf = new char[fileSize];

        std::ifstream file(filename.c_str(), std::ios::binary | std::ios::in);
        file.read(fileBuf, fileSize);

        bnd->addFile(fileBuf, fileSize, filename);

        delete fileBuf;
        file.close();
    }

    std::wstring bndFilePath = folderPath.filename().replace_extension("").c_str() + std::wstring(L".bnd");

    RFile* bndOut = RFile::create(bndFilePath);
    bnd->writeBinary(bndOut);

    bndOut->close();
    delete bndOut;
    bnd->destroy();
}