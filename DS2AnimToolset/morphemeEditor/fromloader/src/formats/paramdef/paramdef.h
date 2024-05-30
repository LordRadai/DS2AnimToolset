#pragma once
#include <string>
#include <wchar.h>

#include "../stdafx.h"

#ifndef __PARAMDEF__
#define __PARAMDEF__

namespace cfr {
	enum EditEnum
	{
		EDIT_WRAP = 1,
		EDIT_LOCK = 4,
	};

	class PARAMDEF : public File
	{
		private:
		//requires a fully manual read :(
		class i_Header //internal header
		{
			public:
			int32_t fileSize = 0;
			uint16_t headerSize = 0;
			uint16_t dataVersion = 0;
			uint16_t fieldCount = 0;
			uint16_t fieldSize = 0;

			int32_t typePad1 = 0;
			int64_t paramTypeOffset = 0;
			int64_t typePad2 = 0;
			int64_t typePad3 = 0;
			int32_t typePad4 = 0;

			char paramID[32];

			int8_t m_bigEndian = 0;
			int8_t unicode = 0;
			int16_t formatVersion = 0;

			int64_t fieldsOffset = 0;

			i_Header() {}
			i_Header(UMEM* src);
		};

		class i_Field //internal field
		{
			public:
			int64_t displayNameOffset = 0;
			wchar_t displayNameW[0x20] = {};
			char displayName[0x40] = {};
			char displayType[8];
			char displayFormat[8];

			float defaultValue = 0;
			float minValue = 0;
			float maxValue = 0;
			float increment = 0;
			
			int32_t editFlags = 0;
			int32_t byteCount = 0;
			int32_t descriptionOffset32 = 0;
			int64_t descriptionOffset64 = 0;
			int64_t internalTypeOffset  = 0;
			char internalType[32] = {};
			int64_t internalNameOffset  = 0;
			char internalName[32] = {};
			int32_t sortId = 0;
			int32_t unkB4 = 0;
			int64_t offsetB8 = 0;
			int64_t offsetC0 = 0;
			int64_t offsetC8 = 0;

			std::wstring descriptionW;
			std::string description;

			std::string  unkB8;
			std::string  unkC0;
			std::wstring unkC8;

			std::wstring displayNameWStr;
			std::string internalTypeStr;
			std::string internalNameStr;

			i_Field();
			i_Field(UMEM* src, PARAMDEF* parent);
		};

		private:
		i_Header i_header;
		std::vector<i_Field> i_fields;

		public:
		class Field
		{
			public:
			std::wstring name;
			std::wstring description;
			std::wstring type;
			std::wstring format;
			int id;
			float defaultValue;
			float minValue;
			float maxValue;
			float increment;
		};

		std::vector<Field> fields;

		PARAMDEF();
		PARAMDEF(UMEM* src);
	};
};

#endif