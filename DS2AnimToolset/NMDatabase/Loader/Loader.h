#pragma once
#include "RCore.h"

namespace db
{
	class Loader
	{
	protected:
		FILE* m_outputStream;
		std::string m_typeString;
		std::string m_productVersion;
		int m_dataVersion;
		int m_formatVersion;

	public:
		Loader(FILE* inputStream);
	};
}
