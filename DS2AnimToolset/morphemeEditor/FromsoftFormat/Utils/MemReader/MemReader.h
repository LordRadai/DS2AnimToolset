#pragma once
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>
#include <commdlg.h>
#include <shobjidl.h> 
#include <iostream>
#include <fstream>

using namespace std;

namespace MemReader
{
	template <typename T> static void Read(ifstream* pStream, T* pBuf)
	{
		pStream->read((char*)pBuf, sizeof(T));
	}

	template <typename T> static void ReadArray(ifstream* pStream, T* pBuf, size_t iCount)
	{
		pStream->read((char*)pBuf, iCount * sizeof(T));
	}

	template <typename T> static void Write(ofstream* pStream, T v)
	{
		pStream->write((const char*)&v, sizeof(T));
	}

	template <typename T> static void WriteArray(ofstream* pStream, T* pVal, size_t iCount)
	{
		pStream->write((const char*)pVal, iCount * sizeof(T));
	}

	static void Pad(ofstream* pStream, BYTE padVal, int iCount)
	{
		BYTE* pPaddingVal = new BYTE[iCount];

		for (size_t i = 0; i < iCount; i++)
			pPaddingVal[i] = padVal;

		WriteArray(pStream, pPaddingVal, iCount);

		delete[] pPaddingVal;
	}

	static void AlignStream(ifstream* pStream, UINT64 alignment)
	{
		if (alignment == 0)
			return;

		streampos current_pos = pStream->tellg();
		UINT64 remainder = current_pos % alignment;

		if (remainder > 0)
		{
			UINT64 pad_amt = alignment - remainder;

			pStream->seekg(current_pos + (streampos)pad_amt);
		}
	}

	static void AlignStream(ofstream* pStream, UINT64 alignment)
	{
		if (alignment == 0)
			return;

		streampos current_pos = pStream->tellp();
		UINT64 remainder = current_pos % alignment;

		if (remainder > 0)
		{
			UINT64 pad_amt = alignment - remainder;

			Pad(pStream, 0xCD, pad_amt);
		}
	}
}