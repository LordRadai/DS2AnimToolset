#include "Saver.h"

namespace db
{
	Saver::~Saver()
	{
		if (m_outputStream)
		{
			fclose(m_outputStream);

			m_outputStream = nullptr;
		}
	}

	bool Saver::save(db::Attribute* attribute)
	{
		if (m_outputStream == nullptr)
			throw std::runtime_error("File is not OK to write.");

		return doSave(attribute);
	}
}
