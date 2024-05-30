#pragma once

#include <stdexcept>
#include <map>

#include "../stdafx.h"

namespace cfr
{
	class MTD : public File
	{
		public:
		class Marker;
		class Block;
		class MarkedString;
		class Texture;
		class Value;
		class Param;
		class Lists;
		class Data;
		class Header;
		
		class Marker
		{
			public:
			uint8_t value = 0;

			Marker(UMEM* src);
			Marker();
		};

		class Block
		{
			public:
			int32_t zero = 0;
			int32_t length = 0;
			int32_t type = 0;
			int32_t version = 0;
			Marker marker;

			Block(UMEM* src);
			Block();
		};

		class MarkedString
		{
			public:
			int32_t length = 0;
			char* str = nullptr;
			Marker marker;

			MarkedString(UMEM* src);
			MarkedString();

			void print();
			char* toUtf8(std::map<int,int>* charMap, int* length);
		};

		class Texture
		{
			public:
			Block block;
			MarkedString type;
			int32_t uvNumer = 0;
			Marker marker;
			int32_t shaderDataIndex = 0;

			//only if version == 5
			int32_t unkB0C = 0;
			MarkedString path;
			int32_t floatCount = 0;
			float* floats = nullptr;

			Texture(UMEM* src);
		};

		class Value
		{
			public:
			Block block;
			int32_t valueCount = 0;
			int8_t* byteValues = nullptr;
			int32_t* intValues = nullptr;
			float* floatValues = nullptr;

			Value(UMEM* src);
			Value();
		};

		class Param
		{
			public:
			Block block;
			MarkedString name;
			MarkedString type;
			int32_t unkB00 = 0;
			Value value;
			Marker end;
			int32_t unkC00 = 0;

			Param(UMEM* src);

			void print();
		};

		class Lists
		{
			public:
			Block block;
			int32_t unkA04 = 0;
			Marker marker;
			int32_t paramCount = 0;
			Param* params = nullptr;
			Marker paramsEnd;
			int32_t textureCount = 0;
			Texture* textures = nullptr;
			Marker end;
			int32_t unkC00 = 0;

			Lists(UMEM* src);
			Lists();
		};

		class Data
		{
			public:
			Block block;
			MarkedString shader;
			MarkedString description;
			int32_t unkB00 = 0;
			Lists lists;
			Marker end;
			int32_t unkC00 = 0;

			Data(UMEM* src);
			Data();
		};

		class Header
		{
			public:
			Block block;
			MarkedString magic;
			int32_t unkB00 = 0;

			Header(UMEM* src);
			Header();
		};

		Block block;
		Header header;
		Marker headerEnd;
		Data mtdData;
		Marker end;
		int32_t unkC00 = 0;

		MTD(const char* path);
		MTD(UMEM* src);

		void print();
	};
};