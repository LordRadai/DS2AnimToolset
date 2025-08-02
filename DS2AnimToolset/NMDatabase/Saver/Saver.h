#pragma once
#include <string>
#include "RCore.h"
#include "Attribute/Attribute.h"

namespace db
{
	class Saver
	{
	protected:
		FILE* m_outputStream;
		std::string m_typeString;

	public:
		Saver(FILE* outputStream, const char* typeString) :
			m_typeString(typeString),
			m_outputStream(outputStream)
		{}

		virtual ~Saver();
		virtual bool doSave(db::Attribute* attribute) = 0;

		bool save(db::Attribute* attribute);
	};
}
