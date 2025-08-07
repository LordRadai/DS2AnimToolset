#include "LoaderXML.h"
#include "extern.h"

namespace db
{
	LoaderXML::LoaderXML(FILE* inputStream) : Loader(inputStream)
	{
		m_xmlDocument = new tinyxml2::XMLDocument();
	}

	LoaderXML::~LoaderXML()
	{
		if (m_xmlDocument)
		{
			delete m_xmlDocument;
			m_xmlDocument = nullptr;
		}

		Loader::~Loader();
	}

	bool LoaderXML::load(db::Attribute* attribute)
	{
		if (!attribute)
			return false;

		if (!m_xmlDocument->LoadFile(m_inputStream))
			return false;

		return parse(attribute);
	}

	bool LoaderXML::startElementHandler(tinyxml2::XMLElement* rootElement)
	{
		m_typeString = rootElement->Attribute("typeString");
		m_productVersion = rootElement->Attribute("productVersion");
		m_dataVersion = rootElement->IntAttribute("dataVersion", -1);
		m_formatVersion = rootElement->IntAttribute("formatVersion", -1);

		if (m_typeString.empty())
		{
			throw std::runtime_error("LoaderXML::startElementHandler: Missing 'typeString' attribute in root element. File is invalid.\n");
			return false;
		}

		if (m_productVersion != g_productVersion)
		{
			throw std::runtime_error("LoaderXML::startElementHandler: Loaded a file created with a different version than expected (3.6.2).\n");
			return false;
		}

		if (m_dataVersion != g_dataVersion)
		{
			throw std::runtime_error("LoaderXML::startElementHandler: Loaded a file with a different data version than the one expected (3506).\n");
			return false;
		}

		if (m_formatVersion != g_formatVersion)
		{
			throw std::runtime_error("LoaderXML::startElementHandler: Loaded file with a different format version than the one expected (5).\n");
			return false;
		}

		return true;
	}

	bool LoaderXML::parse(db::Attribute* attribute)
	{
		tinyxml2::XMLElement* rootElement = m_xmlDocument->FirstChildElement("NaturalMotion");

		if (!rootElement)
			return onParseFailed();

		startElementHandler(rootElement);

		tinyxml2::XMLElement* firstElement = rootElement->FirstChildElement();

		if (!firstElement)
			return onParseFailed();

		return onParseSuccess(attribute, firstElement);	
	}

	bool LoaderXML::onParseSuccess(db::Attribute* attribute, tinyxml2::XMLElement* element)
	{
		XMLElement* firstDbElement = new XMLElement(element, element->Name(), attribute, nullptr);

		bool bStatus = attribute->readValueXML(3, firstDbElement, this);

		m_referenceResolver->resolvePathReferences(attribute);
		m_referenceResolver->resolveRuntimeIDReferences(attribute);

		return bStatus;
	}

	bool LoaderXML::onParseFailed()
	{
		return false;
	}
}