#include "stringio.h"

namespace cfr
{
	std::string readString(UMEM* src)
	{
		std::string ret;
		bool nullFound = false;
		char c = 'a';

		while(nullFound == false)
		{
			uread(&c,sizeof(char),1,src);

			if(c == '\0')
				nullFound = true;
			else
				ret.push_back(c);
		}

		return ret;
	};

	std::wstring readWString(UMEM* src, bool endian)
	{
		std::wstring ret;
		bool nullFound = false;
		wchar_t c = 'a';

		while(nullFound == false)
		{
			ureade(&c,sizeof(wchar_t),1,src,endian);

			if(c == '\0')
				nullFound = true;
			else
				ret.push_back(c);
		}

		return ret;
	};

	std::string freadString(UMEM* src, bool nullTerm, int length)
	{
		std::string ret = "";

		if(nullTerm)
		{
			bool found = false;

			while(!found)
			{
				char c = ugetc(src);

				if(ueod(src) != 0)
					throw std::runtime_error("Reached end of file while reading string...\n");
				if(c == 0)
					found = true;
				else
					ret.push_back(c);
			}

			return ret;
		}
		else
		{
			//this could be better optimized
			for(int i = 0; i < length; i++)
			{
				if(ueod(src) != 0)
					throw std::runtime_error("Reached end of file while reading string...\n");
				ret.push_back(ugetc(src));
			}

			return ret;
		}
	};

	std::pair<std::string,std::string> splitFullPath(std::string fullpath)
	{
		int len = fullpath.size();
		std::pair<std::string, std::string> combo;
		combo.first = "";
		combo.second = "";
		bool found = false;

		while(!found)
		{
			if(len < 1)
				throw std::runtime_error("Reached end of string while getting name...\n");

			char c = fullpath.at(len);

			if((c == '\\')||(c == '/'))	
				found = true;
			else
			{
				combo.first.insert(0,1,c);
				len--;
			}
		}
		
		while(len > 0)
		{
			char c = fullpath.at(len);
			combo.second.insert(0,1,c);
			len--;
		}
		
		return combo;
	};

	wchar_t* getUniversalString(long offset, bool unicode, UMEM* src, int* len)
	{
		long pos = utell(src);
		useek(src,offset,SEEK_SET);

		int length = 0;
		wchar_t* ret;

		if(unicode)
		{
			wchar_t c;
			
			while(true)
			{
				uread(&c,sizeof(wchar_t),1,src);
				length++;
				if(c == L'\0')
					break;
			}

			useek(src,-1 * sizeof(wchar_t) * length,SEEK_CUR);
			ret = (wchar_t*)malloc(sizeof(wchar_t) * length);
			uread(&ret[0],sizeof(wchar_t),length,src);
		}
		else
		{			
			char c;
			
			while(true)
			{
				uread(&c,sizeof(char),1,src);
				length++;
				if(c == '\0')
					break;
			}

			useek(src,-length,SEEK_CUR);
			char* buffer = (char*)malloc(length);
			uread(&buffer[0],1,length,src);
			
			ret = (wchar_t*)malloc(length * sizeof(wchar_t));

			mbstowcs(ret,buffer,length);
		}

		useek(src,pos,SEEK_SET);

		memcpy(len,&length,sizeof(int));
		return ret;
	};

	int getWideStringLength(wchar_t* str)
	{
		int offset = 0;
		wchar_t c;

		while(true)
		{
			c = str[offset];
			offset++;
			if(c == L'\0')
				break;
		}

		return offset;
	};

	char* jisToUtf8(char* str, int lengthIn, std::map<int,int>* charMap, int* lengthOut)
	{
		int newLength = 0;
		int ci = 0;
		while(ci < lengthIn)
		{
			char section = (uint8_t)str[ci] >> 4;
			if(section == 0x8 || section == 0x9 || section == 0xE)
			{
				newLength++;
				ci+=2;
			}
			else
			{
				newLength++;
				ci+=1;
			}
			
		}

		char* output = (char*)malloc(newLength);
		for(int i = 0; i < newLength; i++)
		{
			output[i] = '\0';
		}
		ci = 0;
		int newIndex = 0;
		while(ci < lengthIn)
		{
			char section = (uint8_t)str[ci] >> 4;
			if(section == 0x8 || section == 0x9 || section == 0xE)
			{
				wchar_t w = L'\0';
				memcpy(&w,&str[ci],2);
				char utf = (*charMap)[(int)w];
				memcpy(&output[newIndex],&utf,1);
				ci+=2;
			}
			else
			{
				char c = '\0';
				memcpy(&c,&str[ci],1);
				char utf = (*charMap)[(int)c];
				memcpy(&output[newIndex],&utf,1);
				ci+=1;
			}
			newIndex++;
		}

		//memcpy(&newLength,lengthOut,sizeof(int));
		return output;
	};	
};