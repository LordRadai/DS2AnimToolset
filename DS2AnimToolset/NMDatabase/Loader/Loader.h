#pragma once
#include "RCore.h"

namespace db
{
	class Loader
	{
	protected:
		FILE* m_inputStream;
		std::string m_typeString;
		std::string m_productVersion;
		int m_dataVersion;
		int m_formatVersion;

	public:
		Loader(FILE* inputStream) : 
			m_inputStream(inputStream),
			m_typeString(""),
			m_productVersion(""),
			m_dataVersion(0),
			m_formatVersion(0)
		{}

		virtual ~Loader() = default;

		std::string getTypeString() const { return m_typeString; }
		std::string getProductVersion() const { return m_productVersion; }
		int getDataVersion() const { return m_dataVersion; }
		int getFormatVersion() const { return m_formatVersion; }
	};
}
