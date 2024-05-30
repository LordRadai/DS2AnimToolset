#include "open.h"

//TODO: don't currently have a way record compression info
// nor is there a way to compress stuff. For now, compressed
// data cannot be written...

namespace cfr
{
	File* OpenFile(const char* path)
	{
		UMEM* src = uopenFile(path,"rb");
		return OpenFile(src);
	};

	File* OpenFile(UMEM* src)
	{
		FILE_FORMAT format = getFormat(src);

		UMEM* tmp = src;

		//decompress the DCX and substitue pointers
		if(format == FROM_DCX)
			tmp = openDCX(src);

		useek(src,0,SEEK_SET);

		switch(format)
		{
			case FROM_DCX:
				throw std::runtime_error("Found double compression?\n");
			//case FROM_BDF3:
			//	return new BDF3(tmp);
			//case FROM_BDF4:
			//	return new BDF4(tmp);
			//case FROM_BHD5_0:
			//	return new BHD5_0(tmp);
			//case FROM_BHD5_1:
			//	return new BHD5_1(tmp);
			case FROM_BND3:
				return new BND3(tmp);
			case FROM_FLVER2:
				return new FLVER2(tmp);
			//case FROM_BND4:
			//	return new BND4(tmp);
			case UNKOWN_FORMAT:
				throw std::runtime_error("Unkown format!\n");
		}

		return nullptr;
	};

	// Note: data is COPIED to child. This will duplicate memory.
	File* Binder::loadEntry(int index)
	{
		long totalSize = std::max(
			this->entries[index]->compressedSize,
			this->entries[index]->uncompressedSize
		);

		useek(this->data,this->entries[index]->offset,SEEK_SET);

		char* fileData = (char*)malloc(totalSize);
		uread(fileData,totalSize,1,this->data);

		//return to start to clean up
		useek(this->data,0,SEEK_SET);

		UMEM* umem = uopenMem(fileData,totalSize);
		useek(umem,0,SEEK_SET);

		return OpenFile(umem);

		//create child and bind to parent
		//File* file = new File(uopenMem(fileData,totalSize));
		//this->newChild(file);

		//return file;
	};
}