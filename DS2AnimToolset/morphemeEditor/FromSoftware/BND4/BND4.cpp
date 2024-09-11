#include <assert.h>
#include "BND4.h"
#include "RCore.h"
#include <filesystem>

BndFile::BndFile()
{
}

BndFile::BndFile(RFile* dcx)
{
	dcx->read(&this->rawFlags);
	dcx->alignStream(4);
	dcx->read(&this->iVar4);

	dcx->read(&this->compressedSize);
	dcx->read(&this->uncompressedSize);

	int dataOffset;
	dcx->read(&dataOffset);
	dcx->read(&this->id);

	int nameOffset;
	dcx->read(&nameOffset);

	dcx->seek(nameOffset);

	char name[50];
	dcx->readArray(name, 50);

	this->name = name;

	dcx->seek(dataOffset);
	this->data = new BYTE[this->compressedSize];

	dcx->readArray(this->data, this->compressedSize);
}

BndFile::~BndFile()
{
}

BndFile* BndFile::create(void* data, int fileSize, std::string name, int id)
{
	BndFile* file = new BndFile;

	file->data = (BYTE*)data;
	file->compressedSize = fileSize;
	file->uncompressedSize = fileSize;
	file->id = id;
	file->name = name;

	return file;
}

void BndFile::writeBinary(RFile* file, int nameOffset, int dataOffset)
{
	file->write(this->rawFlags);
	file->alignStream(4);
	file->write(this->iVar4);
	file->write(this->compressedSize);
	file->write(this->uncompressedSize);

	file->write(dataOffset);
	file->write(this->id);
	file->write(nameOffset);

	UINT64 bak = file->tell();
	file->seek(dataOffset);

	file->writeArray(this->data, this->uncompressedSize);

	file->seek(nameOffset);
	file->writeString(this->name);

	file->seek(bak);
}

Bnd4::Bnd4()
{
}

Bnd4::~Bnd4()
{
}

Bnd4* Bnd4::create(bool bigEndian, bool unicode, bool extended)
{
	Bnd4* bnd = new Bnd4;

	bnd->m_bigEndian = bigEndian;
	bnd->m_unicode = unicode;
	bnd->m_extended = extended;

	return bnd;
}

Bnd4* Bnd4::loadFromFile(std::wstring filepath)
{
	if (!std::filesystem::exists(filepath))
		return nullptr;

	Bnd4* bndFile = new Bnd4;

	RFile* file = RFile::open(filepath.c_str(), kFileModeRead);

	file->readArray(bndFile->m_magic, 4); assert(strcmp(bndFile->m_magic, "BND4") == 0);
	file->read(&bndFile->m_sVar4);
	file->read(&bndFile->m_sVar5);
	file->read(&bndFile->m_bVar6);
	file->read(&bndFile->m_bigEndian);
	file->read(&bndFile->m_bVarA);
	file->alignStream(4);
	file->read(&bndFile->m_fileCount);

	UINT64 fileOffset;
	file->read(&fileOffset);

	file->readArray(bndFile->m_version, 8);
	file->read(&bndFile->m_fileHeaderSize);
	file->read(&bndFile->m_fileHeadersEnd);
	file->read(&bndFile->m_unicode);
	file->read(&bndFile->m_rawFormat);
	file->read(&bndFile->m_extended);
	file->alignStream(8);

	UINT64 bucketsOffset;
	file->read(&bucketsOffset);

	bndFile->m_files.reserve(bndFile->m_fileCount);
	for (size_t i = 0; i < bndFile->m_fileCount; i++)
	{
		int offset = fileOffset + i * bndFile->m_fileHeaderSize;
		file->seek(offset);

		bndFile->m_files.push_back(new BndFile(file));
	}

	file->close();

	delete file;

	return bndFile;
}

void Bnd4::destroy()
{
	for (size_t i = 0; i < this->m_fileCount; i++)
		delete this->m_files[i];
	
	this->m_files.clear();
}

BndFile* Bnd4::getFile(int idx)
{
	if (idx < this->m_fileCount)
		return this->m_files[idx];

	return nullptr;
}

void Bnd4::addFile(void* data, int fileSize, std::string name)
{
	BndFile* newFile = BndFile::create(data, fileSize, name, this->m_fileCount);

	this->m_fileCount++;

	this->m_files.push_back(newFile);
}

void Bnd4::writeBinary(RFile* file)
{
	file->writeArray(this->m_magic, 4);
	file->write(this->m_sVar4);
	file->write(this->m_sVar5);
	file->write(this->m_bVar6);
	file->write(this->m_bigEndian);
	file->write(this->m_bVarA);
	file->alignStream(4);
	file->write(this->m_fileCount);
	file->writeLongPtr(64);
	file->writeArray(this->m_version, 8);
	file->writeLongPtr(36);

	UINT64 fileHeadersEnd = this->m_fileCount * 36 + 64;

	int nameTotalLen = 0;

	for (size_t i = 0; i < this->m_fileCount; i++)
		nameTotalLen += this->m_files[i]->name.length() + 1;

	fileHeadersEnd += nameTotalLen;

	fileHeadersEnd = RMemory::align(fileHeadersEnd, 4);

	file->write(fileHeadersEnd);
	file->write(this->m_unicode);
	file->write(this->m_rawFormat);
	file->write(this->m_extended);
	file->alignStream(8);
	file->writeLongPtr(NULL);

	int nameStrLen = 0;
	int dataLen = 0;

	for (size_t i = 0; i < this->m_fileCount; i++)
	{
		int nameOffset = 64 + 36 * this->m_fileCount + nameStrLen;
		int dataOffset = RMemory::align(fileHeadersEnd, 16) + dataLen;

		dataOffset = RMemory::align(dataOffset, 16);

		this->m_files[i]->writeBinary(file, nameOffset, dataOffset);

		nameStrLen += this->m_files[i]->name.length() + 1;
		dataLen += this->m_files[i]->uncompressedSize;
	}
}

void Bnd4::saveToFile(std::wstring filename)
{
	RFile* bndFile = RFile::create(filename);
	
	this->writeBinary(bndFile);

	delete bndFile;
}