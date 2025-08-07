#pragma once
#include "RCore.h"
#include "ReferenceResolver/ReferenceResolver.h"

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
		ReferenceResolver* m_referenceResolver;

	public:
		Loader(FILE* inputStream) : 
			m_inputStream(inputStream),
			m_typeString(""),
			m_productVersion(""),
			m_dataVersion(0),
			m_formatVersion(0),
			m_referenceResolver(new ReferenceResolver())
		{}

		virtual ~Loader();

		std::string getTypeString() const { return m_typeString; }
		std::string getProductVersion() const { return m_productVersion; }
		int getDataVersion() const { return m_dataVersion; }
		int getFormatVersion() const { return m_formatVersion; }
		ReferenceResolver* getReferenceResolver() const { return m_referenceResolver; }
	};
}
