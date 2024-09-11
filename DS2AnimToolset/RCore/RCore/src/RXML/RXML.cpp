#include "RXML.h"

namespace RXML
{
	XMLElemObj* XMLElemObj::create(tinyxml2::XMLElement* elem)
	{
		XMLElemObj* xmlElemObj = new XMLElemObj;
		xmlElemObj->m_xmlElement = elem;

		for (tinyxml2::XMLElement* child = elem->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
			xmlElemObj->m_childElements.push_back(XMLElemObj::create(child));

		return xmlElemObj;
	}

	XMLElemObj* XMLElemObj::getChildElement(std::string name)
	{
		for (size_t i = 0; i < this->m_childElements.size(); i++)
		{
			if (name.compare(this->m_childElements[i]->m_xmlElement->Name()) == 0)
				return this->m_childElements[i];
		}

		return nullptr;
	}

	void XMLElemObj::addChildElement(std::string name)
	{
		tinyxml2::XMLElement* element = this->m_xmlElement->InsertNewChildElement(name.c_str());

		this->m_childElements.push_back(XMLElemObj::create(element));
	}

	void XMLElemObj::destroy()
	{
		for (size_t i = 0; i < this->m_childElements.size(); i++)
			this->m_childElements[i]->destroy();

		this->m_childElements.clear();

		delete this;
	}

	XMLFileObj* XMLFileObj::create()
	{
		XMLFileObj* xmlFileObj = new XMLFileObj;

		xmlFileObj->m_xmlDoc = new tinyxml2::XMLDocument;

		return xmlFileObj;
	}

	XMLFileObj* XMLFileObj::create(const char* filename)
	{
		XMLFileObj* xmlFileObj = new XMLFileObj;

		tinyxml2::XMLDocument* xml = new tinyxml2::XMLDocument;
		if (xml->LoadFile(filename) != tinyxml2::XML_SUCCESS)
		{
			delete xmlFileObj;
			return nullptr;
		}

		xmlFileObj->m_xmlDoc = xml;
		xmlFileObj->m_rootElement = XMLElemObj::create(xml->FirstChildElement());

		return xmlFileObj;
	}

	void XMLFileObj::addChildElement(std::string name)
	{
		tinyxml2::XMLElement* element = this->m_xmlDoc->NewElement(name.c_str());

		this->m_xmlDoc->InsertEndChild(element);
		this->m_rootElement->addChildElement(name);
	}

	bool XMLFileObj::save()
	{
		if (this->m_xmlDoc->SaveFile(this->m_dstFileName.c_str()) == tinyxml2::XML_SUCCESS)
			return true;

		return false;
	}

	void XMLFileObj::destroy()
	{
		this->m_rootElement->destroy();

		this->m_xmlDoc->Clear();

		delete this->m_xmlDoc;
		delete this;
	}

	tinyxml2::XMLElement* createBoolElement(tinyxml2::XMLElement* parent, std::string name, bool value)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name.c_str());

		element->SetAttribute("type", "bool");
		element->SetText(value);

		return element;
	}

	tinyxml2::XMLElement* createByteElement(tinyxml2::XMLElement* parent, std::string name, INT8 value)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name.c_str());

		element->SetAttribute("type", "byte");
		element->SetText(value);

		return element;
	}

	tinyxml2::XMLElement* createUByteElement(tinyxml2::XMLElement* parent, std::string name, UINT8 value)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name.c_str());

		element->SetAttribute("type", "ubyte");
		element->SetText(value);

		return element;
	}

	tinyxml2::XMLElement* createShortElement(tinyxml2::XMLElement* parent, std::string name, INT16 value)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name.c_str());

		element->SetAttribute("type", "short");
		element->SetText(value);

		return element;
	}

	tinyxml2::XMLElement* createUShortElement(tinyxml2::XMLElement* parent, std::string name, UINT16 value)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name.c_str());

		element->SetAttribute("type", "ushort");
		element->SetText(value);

		return element;
	}

	tinyxml2::XMLElement* createIntElement(tinyxml2::XMLElement* parent, std::string name, int value)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name.c_str());

		element->SetAttribute("type", "int");
		element->SetText(value);

		return element;
	}

	tinyxml2::XMLElement* createUIntElement(tinyxml2::XMLElement* parent, std::string name, UINT value)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name.c_str());

		element->SetAttribute("type", "uint");
		element->SetText(value);

		return element;
	}

	tinyxml2::XMLElement* createInt64Element(tinyxml2::XMLElement* parent, std::string name, INT64 value)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name.c_str());

		element->SetAttribute("type", "int64");
		element->SetText(value);

		return element;
	}

	tinyxml2::XMLElement* createUInt64Element(tinyxml2::XMLElement* parent, std::string name, UINT64 value)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name.c_str());

		element->SetAttribute("type", "uint64");
		element->SetText(value);

		return element;
	}

	tinyxml2::XMLElement* createFloatElement(tinyxml2::XMLElement* parent, std::string name, float value)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name.c_str());

		element->SetAttribute("type", "float");
		element->SetText(value);

		return element;
	}

	tinyxml2::XMLElement* createVector3Element(tinyxml2::XMLElement* parent, std::string name, float x, float y, float z)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name.c_str());

		element->SetAttribute("type", "vector3");
		RXML::createFloatElement(element, "x", x);
		RXML::createFloatElement(element, "y", y);
		RXML::createFloatElement(element, "z", z);

		return element;
	}

	tinyxml2::XMLElement* createVector4Element(tinyxml2::XMLElement* parent, std::string name, float x, float y, float z, float w)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name.c_str());

		element->SetAttribute("type", "vector4");
		RXML::createFloatElement(element, "x", x);
		RXML::createFloatElement(element, "y", y);
		RXML::createFloatElement(element, "z", z);
		RXML::createFloatElement(element, "w", w);

		return element;
	}

	bool getBoolElement(tinyxml2::XMLElement* parent, std::string name)
	{
		tinyxml2::XMLElement* element = parent->FirstChildElement(name.c_str());

		if (strcmp(element->FindAttribute("type")->Value(), "bool") != 0)
			throw("Element type mismath");

		std::string value = element->GetText();

		if (value.compare("true"))
			return true;

		return false;
	}

	INT8 getByteElement(tinyxml2::XMLElement* parent, std::string name)
	{
		tinyxml2::XMLElement* element = parent->FirstChildElement(name.c_str());

		if (strcmp(element->FindAttribute("type")->Value(), "byte") != 0)
			throw("Element type mismath");

		return std::stoi(element->GetText());
	}

	UINT8 getUByteElement(tinyxml2::XMLElement* parent, std::string name)
	{
		tinyxml2::XMLElement* element = parent->FirstChildElement(name.c_str());

		if (strcmp(element->FindAttribute("type")->Value(), "ubyte") != 0)
			throw("Element type mismath");

		return std::stoi(element->GetText());
	}

	INT16 getShortElement(tinyxml2::XMLElement* parent, std::string name)
	{
		tinyxml2::XMLElement* element = parent->FirstChildElement(name.c_str());

		if (strcmp(element->FindAttribute("type")->Value(), "short") != 0)
			throw("Element type mismath");

		return std::stoi(element->GetText());
	}

	UINT16 getUShortElement(tinyxml2::XMLElement* parent, std::string name)
	{
		tinyxml2::XMLElement* element = parent->FirstChildElement(name.c_str());

		if (strcmp(element->FindAttribute("type")->Value(), "ushort") != 0)
			throw("Element type mismath");

		return std::stoi(element->GetText());
	}

	int getIntElement(tinyxml2::XMLElement* parent, std::string name)
	{
		tinyxml2::XMLElement* element = parent->FirstChildElement(name.c_str());

		if (strcmp(element->FindAttribute("type")->Value(), "int") != 0)
			throw("Element type mismath");

		return std::stoi(element->GetText());
	}

	UINT getUIntElement(tinyxml2::XMLElement* parent, std::string name)
	{
		tinyxml2::XMLElement* element = parent->FirstChildElement(name.c_str());

		if (strcmp(element->FindAttribute("type")->Value(), "uint") != 0)
			throw("Element type mismath");

		return std::stoi(element->GetText());
	}

	INT64 getInt64Element(tinyxml2::XMLElement* parent, std::string name)
	{
		tinyxml2::XMLElement* element = parent->FirstChildElement(name.c_str());

		if (strcmp(element->FindAttribute("type")->Value(), "int64") != 0)
			throw("Element type mismath");

		return std::stoll(element->GetText());
	}

	UINT64 getUInt64Element(tinyxml2::XMLElement* parent, std::string name)
	{
		tinyxml2::XMLElement* element = parent->FirstChildElement(name.c_str());

		if (strcmp(element->FindAttribute("type")->Value(), "uint64") != 0)
			throw("Element type mismath");

		return std::stoull(element->GetText());
	}

	float getFloatElement(tinyxml2::XMLElement* parent, std::string name)
	{
		tinyxml2::XMLElement* element = parent->FirstChildElement(name.c_str());

		if (strcmp(element->FindAttribute("type")->Value(), "float") != 0)
			throw("Element type mismath");

		return std::stof(element->GetText());
	}

	void getVector3Element(tinyxml2::XMLElement* parent, std::string name, float* buf)
	{
		tinyxml2::XMLElement* element = parent->FirstChildElement(name.c_str());

		if (strcmp(element->FindAttribute("type")->Value(), "vector3") != 0)
			throw("Element type mismath");

		buf[0] = RXML::getFloatElement(element, "x");
		buf[1] = RXML::getFloatElement(element, "y");
		buf[2] = RXML::getFloatElement(element, "z");
	}

	void getVector4Element(tinyxml2::XMLElement* parent, std::string name, float* buf)
	{
		tinyxml2::XMLElement* element = parent->FirstChildElement(name.c_str());

		if (strcmp(element->FindAttribute("type")->Value(), "vector4") != 0)
			throw("Element type mismath");

		buf[0] = RXML::getFloatElement(element, "x");
		buf[1] = RXML::getFloatElement(element, "y");
		buf[2] = RXML::getFloatElement(element, "z");
		buf[3] = RXML::getFloatElement(element, "w");
	}
}