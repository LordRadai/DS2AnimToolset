#ifndef CFR_BINDER
#define CFR_BINDER

#pragma once
#include "../../util/file/file.h"
//#include "../../open.h"

namespace cfr
{
	//listing for a file in a bnd, not all members exist in each binder
	class Entry
	{
		public:
		std::string name = ""; //e.g. 'c5370.flver'
		std::string path = ""; //eg 'FRPG:/DATA/CHR/C5370/'
		long hash = 0;
		long offset = 0; //relative to start of binder data
		long uncompressedSize = 0; //in bytes
		long compressedSize   = 0; //in bytes
		long id = 0;

		Entry();

		Entry(
			std::string name, std::string path, long offset, 
			long compressedSize, long uncompressedSize, long id
		);
	};

	class Binder : public File
	{
		public:
		std::vector<Entry*> entries = {};

		File* loadEntry(int index);

		File* loadFileId(int entryID);

		File* loadFileHash(int hash);

		File* loadFileName(std::string name);

		//writing will require all open children to be written as well...

		//virtual int write(const char* path);

		//virtual int write(FILE* file);

		protected:
		//will need mem to create the file i suppose? 
		Binder(UMEM* src);
		Binder(const char* path);
		//~Binder();

		//Binder(File file);
	};
};

#endif