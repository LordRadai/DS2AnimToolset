#pragma once
#include "Saver.h"

namespace db
{
	class SaverXML : public Saver
	{
		tinyxml2::XMLDocument* m_xmlDocument;

	public:
		SaverXML(FILE* outputStream, const char* typeString);

		virtual ~SaverXML() override;
		bool doSave(db::Attribute* attribute) override;

		/**
		 * \brief Get the XML document.
		 * 
		 * \return The XML document used for saving.
		 */
		tinyxml2::XMLDocument* getXMLDocument() const { return m_xmlDocument; }
	};
}
