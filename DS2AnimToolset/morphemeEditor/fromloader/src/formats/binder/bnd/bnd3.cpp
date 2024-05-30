#include "bnd3.h"

namespace cfr
{
	BND3::BND3_Entry::BND3_Entry(Header* header, UMEM* src, long startPos)
	{
		uread(&this->rawFlags,sizeof(char),4,src);

		uread(&this->compressedSize,sizeof(int32_t),1,src);

		uread(&this->dataOffset,sizeof(uint32_t),1,src);

		//file id
		if(header->rawFormat & 0b01000000)
			uread(&this->id,sizeof(int32_t),1,src);

		//file name offset
		if((header->rawFormat & 0b00100000) | (header->rawFormat & 0b00010000))
			uread(&this->nameOffset,4,1,src);

		//file uncompressed size
		if(header->rawFormat & 0b00000100)
			uread(&this->uncompressedSize,sizeof(int32_t),1,src);
	};

	/*Entry* BND3::BND3_Entry::newEntry(UMEM* src, int startPos)
	{
		long position = utell(src);
			
		useek(src,this->nameOffset+startPos,SEEK_CUR);

		std::string fullpath = freadString(src);

		std::pair<std::string, std::string> pathName = splitFullPath(fullpath);

		useek(src,position,SEEK_SET);

		return new Entry(
			pathName.first, pathName.second,
			this->dataOffset+startPos,
			this->compressedSize,
			this->uncompressedSize,
			this->id				
		);
	};*/

	BND3::Header::Header()
	{
		//do nothing
	};

	BND3::Header::Header(UMEM* src)
	{
		uread(this,sizeof(Header),1,src);

		if(strncmp(this->magic,"BND3",4) != 0)
			throw std::runtime_error("Failed BND3 magic!\n");

		if(this->unk1C != 0)
			throw std::runtime_error("Failed BND3 unk1C!\n");
	};

	BND3::BND3(const char* path) : Binder(path)
	{
		UMEM* src = uopenFile(path,"rb");
		BND3init(this,src);
	};

	BND3::BND3(UMEM* src) : Binder(src)
	{
		BND3init(this,src);
	};

	void BND3::BND3init(BND3* b, UMEM* src)
	{
		b->format = FROM_BND3;

		//confirm its not a DCX
		char magic[4];
		uread(magic,4,1,src);
		useek(src,0,SEEK_SET);

		if(strncmp(magic,"DCX",3) == 0)
		{
			b->data = openDCX(src);
			uclose(src);
		}
		else
			b->data = src;

		useek(b->data,0,SEEK_SET);

		b->header = new BND3::Header(b->data);

		for(int32_t i = 0; i < b->header->fileCount; i++)
		{
			BND3::BND3_Entry* bndEntry = new BND3::BND3_Entry(b->header,b->data,0);
			b->internalEntries.push_back(bndEntry);

			long position = utell(b->data);
			
			useek(b->data,bndEntry->nameOffset,SEEK_SET);

			std::string fullpath = freadString(b->data);

			//std::pair<std::string, std::string> pathName = splitFullPath(fullpath);

			useek(b->data,position,SEEK_SET);

			long edo = bndEntry->dataOffset;
			long ecs = bndEntry->compressedSize;
			long eus = bndEntry->uncompressedSize;
			long eid = bndEntry->id;

			Entry* entry = new Entry("FIXME","LATER",edo,ecs,eus,eid);

			b->entries.push_back(entry);
		}

		//return to start, cleaned up
		useek(b->data,0,SEEK_SET);
	};
};