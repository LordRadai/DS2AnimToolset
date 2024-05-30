#include "binder.h"

namespace cfr
{
	Entry::Entry(
		std::string name, std::string path, long offset, 
		long compressedSize, long uncompressedSize, long id
	): name(name), path(path), offset(offset), compressedSize(compressedSize), 
	uncompressedSize(uncompressedSize), id(id)
	{
		//do nothing ._.
	};

	// Note: data is COPIED to child. This will duplicate memory.
	/*File* Binder::loadEntry(int index)
	{
		long totalSize = std::max(
			this->entries[index].compressedSize,
			this->entries[index].uncompressedSize
		);

		useek(this->data,this->entries[index].offset,SEEK_SET);

		char* fileData = (char*)malloc(totalSize);
		uread(fileData,totalSize,1,this->data);

		//return to start to clean up
		useek(this->data,0,SEEK_SET);

		return OpenFile(fileData);

		//create child and bind to parent
		//File* file = new File(uopenMem(fileData,totalSize));
		//this->newChild(file);

		//return file;
	};*/

	File* Binder::loadFileName(std::string name)
	{
		//hash file name, then grab hash
		/*for(int i = 0; i < this->entries.size(); i++)
		{
			if(this->entries[i].name.compare(name) == 0)
			{
				return loadFile(i);
			}
		}*/

		return nullptr;
	};

	Binder::Binder(const char* path) : File(path)
	{
		//nothing should happen
	};

	Binder::Binder(UMEM* src) : File(src)
	{
		//nothing should happen
	};
};