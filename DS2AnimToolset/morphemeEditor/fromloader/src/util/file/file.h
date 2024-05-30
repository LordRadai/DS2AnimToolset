#ifndef CFR_FILE
#define CFR_FILE

#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>
//#include <random>
#include <stdexcept>

#include "../types/types.h"
#include "../../../umem.h"

namespace cfr
{
	//an in memory blob of bytes
	class File
	{
		public:
		File* parent = nullptr;
		std::unordered_map<int,File*> children = {};
		int uid; //unique id to aid in tracking
		FILE_FORMAT format = UNKOWN_FORMAT;

		bool bigEndian = false; //figure out later
		UMEM* data = nullptr;

		public:

		//returns type of file if possible
		FILE_FORMAT getFormat();

		//write to 
		//virtual UMEM* toBytes();

		//closes file unless there's children, returns how many
		int close();

		//force closes file and all of its children, returns dead child count
		int forceClose();

		protected:

		int newChild(File* file);

		File(const char* path);
		File(UMEM* src);
		~File();
	};
};

#endif