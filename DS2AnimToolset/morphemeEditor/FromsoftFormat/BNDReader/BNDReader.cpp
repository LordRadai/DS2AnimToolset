#include <assert.h>
#include "BNDReader.h"

BndFile::BndFile()
{
}

BndFile::BndFile(ifstream* dcx)
{
	UINT64 start = dcx->tellg();

	dcx->seekg(start + 0x8);

	MemReader::Read(dcx, &this->m_compressedSize);
	MemReader::Read(dcx, &this->m_uncompressedSize);
	MemReader::Read(dcx, &this->m_dataOffset);
	MemReader::Read(dcx, &this->m_id);
	MemReader::Read(dcx, &this->m_nameOffset);

	dcx->seekg(this->m_nameOffset);
	char name[50];

	MemReader::ReadArray(dcx, name, 50);
	this->m_name = std::string(name);

	dcx->seekg(this->m_dataOffset);

	this->m_data = new char[this->m_compressedSize];

	MemReader::ReadArray(dcx, this->m_data, this->m_compressedSize);
}

BndFile::~BndFile()
{
}

BNDReader::BNDReader()
{
}

BNDReader::BNDReader(PWSTR pszFilePath)
{
	this->m_filePath = pszFilePath;

	ifstream dcx;

	dcx.open(this->m_filePath, ios::binary | ios::ate);
	this->m_fileSize = dcx.tellg();
	dcx.close();

	dcx.open(this->m_filePath, ios::binary);

	char magic[4];
	MemReader::ReadArray(&dcx, magic, 4);

	UINT64 header_size;

	dcx.seekg(0xC);
	MemReader::Read(&dcx, &this->m_fileCount);
	MemReader::Read(&dcx, &header_size);

	this->m_files.reserve(this->m_fileCount);

	for (size_t i = 0; i < this->m_fileCount; i++)
	{	
		dcx.seekg(header_size + (BYTE)i * 0x24);
		this->m_files.push_back(BndFile(&dcx));
	}
}

BNDReader::~BNDReader()
{
}