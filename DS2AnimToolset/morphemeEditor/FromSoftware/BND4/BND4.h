#include <fstream>
#include <vector>
#include "RCore.h"

class BndFile
{
public:
	BYTE rawFlags = 64;
	int iVar4 = -1;
	UINT64 compressedSize = 0;
	UINT64 uncompressedSize = 0;
	BYTE* data = nullptr;
	int id = 0;
	std::string name;

	BndFile();
	BndFile(RFile* dcx);
	~BndFile();

	static BndFile* create(void* data, int fileSize, std::string name, int id);

	void writeBinary(RFile* file, int nameOffset, int dataOffset);
};

class Bnd4 
{
public:
	Bnd4();
	~Bnd4();

	static Bnd4* create(bool bigEndian, bool unicode, bool extended);
	static Bnd4* loadFromFile(std::wstring filepath);
	void destroy();

	bool isBigEndian() const { return this->m_bigEndian; }
	bool isExtended() const { return this->m_extended; }
	bool isUnicode() const { return this->m_unicode; }
	int getNumFiles() const { return this->m_fileCount; }
	BndFile* getFile(int idx);

	void addFile(void* data, int fileSize, std::string name);

	void writeBinary(RFile* file);
	void saveToFile(std::wstring filename);

private:

	char m_magic[4] = { 'B', 'N', 'D', '4' };
	short m_sVar4 = 0;
	short m_sVar5 = 0;
	BYTE m_bVar6 = 0;
	bool m_bigEndian = false;
	BYTE m_bVarA = 1;
	int m_fileCount = 0;
	std::vector<BndFile*> m_files;
	char m_version[8] = { '1', '4', 'M', '2', '6', 'R', '5', '1' };
	UINT64 m_fileHeaderSize = 0;
	UINT64 m_fileHeadersEnd = 0;
	bool m_unicode = false;
	BYTE m_rawFormat = 84;
	bool m_extended = false;
	BYTE* m_buckets = nullptr;
};