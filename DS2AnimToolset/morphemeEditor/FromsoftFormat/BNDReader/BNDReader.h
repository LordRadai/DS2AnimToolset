#include "../Utils/MemReader/MemReader.h"

class BndFile
{
public:
	UINT64 m_compressedSize;
	UINT64 m_uncompressedSize;
	int m_dataOffset;
	int m_id;
	int m_nameOffset;

	std::string m_name;
	char* m_data;

	BndFile();
	BndFile(ifstream* dcx);
	~BndFile();
};

class BNDReader 
{
public:
	PWSTR m_filePath;
	UINT64 m_fileSize;
	PWSTR m_outFilePath;
	UINT64 m_outFileSize;
	bool m_init = false;

	int m_fileCount;
	std::vector<BndFile> m_files;

	BNDReader();
	BNDReader(PWSTR pszFilePath);
	~BNDReader();
};