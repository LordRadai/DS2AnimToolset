#include "SaverXML.h"
#include "extern.h"

namespace db
{
	SaverXML::SaverXML(FILE* outputStream, const char* typeString) :
		Saver(outputStream, typeString),
		m_xmlDocument(new tinyxml2::XMLDocument())
	{}

	SaverXML::~SaverXML()
	{
		if (m_xmlDocument)
		{
			delete m_xmlDocument;
			m_xmlDocument = nullptr;
		}
	}

	bool SaverXML::doSave(db::Attribute* attribute)
	{
		tinyxml2::XMLDeclaration* decl = m_xmlDocument->NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
		tinyxml2::XMLElement* rootElement = m_xmlDocument->NewElement("NaturalMotion");
		rootElement->SetAttribute("typeString", m_typeString.c_str());
		rootElement->SetAttribute("productVersion", g_productVersion);
		rootElement->SetAttribute("dataVersion", g_dataVersion);
		rootElement->SetAttribute("formatVersion", g_formatVersion);

		m_xmlDocument->InsertEndChild(decl);
		m_xmlDocument->InsertEndChild(rootElement);

		bool bWriteStatus = attribute->writeXML(3, this);

		if (!bWriteStatus)
			throw std::runtime_error("Failed to save XML.");

		rootElement->InsertEndChild(attribute->getXMLElement());
		m_xmlDocument->SaveFile(m_outputStream);

		return bWriteStatus;
	}
}
