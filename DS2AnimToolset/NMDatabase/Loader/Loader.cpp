#include "Loader.h"

namespace db
{
	Loader::~Loader()
	{
		if (m_referenceResolver)
		{
			delete m_referenceResolver;
			m_referenceResolver = nullptr;
		}

		if (m_inputStream)
		{
			fclose(m_inputStream);
			m_inputStream = nullptr;
		}
	}
}
