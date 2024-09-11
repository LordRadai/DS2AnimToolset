#include "TaeTemplateXML.h"
#include "RCore.h"

namespace TimeAct
{
	TaeTemplateArgEnumXML* TaeTemplateArgEnumXML::create(tinyxml2::XMLElement* parent, std::string name, int value)
	{
		TaeTemplateArgEnumXML* enumTemplate = new TaeTemplateArgEnumXML;
		enumTemplate->m_xmlElement = parent->InsertNewChildElement("Enum");
		enumTemplate->m_xmlElement->SetAttribute("value", value);
		enumTemplate->m_xmlElement->SetAttribute("name", name.c_str());

		return enumTemplate;
	}

	DataType TaeTemplateEventArgumentXML::getType()
	{
		std::string type = this->m_xmlElement->FindAttribute("type")->Value();

		if (type.compare("bool") == 0)
			return kBool;
		else if (type.compare("byte") == 0)
			return kByte;
		else if (type.compare("ubyte") == 0)
			return kUByte;
		else if (type.compare("short") == 0)
			return kShort;
		else if (type.compare("ushort") == 0)
			return kUShort;
		else if (type.compare("int") == 0)
			return kInt;
		else if (type.compare("uint") == 0)
			return kUInt;
		else if (type.compare("float") == 0)
			return kFloat;
		else if (type.compare("int64") == 0)
			return kInt64;
		else if (type.compare("uint64") == 0)
			return kUInt64;
		else
			return kTypeInvalid;
	}

	TaeTemplateArgEnumXML* TaeTemplateEventArgumentXML::addEnumValue(std::string name, int value)
	{
		TaeTemplateArgEnumXML* enumValue = TaeTemplateArgEnumXML::create(this->m_xmlElement, name, value);

		this->m_enumValues.push_back(enumValue);

		return enumValue;
	}

	TaeTemplateEventArgumentXML* TaeTemplateEventArgumentXML::create(tinyxml2::XMLElement* parent, std::string name, DataType type, bool hidden)
	{
		TaeTemplateEventArgumentXML* templateEventArgumentXML = new TaeTemplateEventArgumentXML;
		templateEventArgumentXML->m_xmlElement = parent->InsertNewChildElement("ArgTemplate");
		templateEventArgumentXML->m_xmlElement->SetAttribute("name", name.c_str());

		switch (type)
		{
		case kBool:
			templateEventArgumentXML->m_xmlElement->SetAttribute("type", "bool");
			break;
		case kByte:
			templateEventArgumentXML->m_xmlElement->SetAttribute("type", "byte");
			break;
		case kUByte:
			templateEventArgumentXML->m_xmlElement->SetAttribute("type", "ubyte");
			break;
		case kShort:
			templateEventArgumentXML->m_xmlElement->SetAttribute("type", "short");
			break;
		case kUShort:
			templateEventArgumentXML->m_xmlElement->SetAttribute("type", "ushort");
			break;
		case kInt:
			templateEventArgumentXML->m_xmlElement->SetAttribute("type", "int");
			break;
		case kUInt:
			templateEventArgumentXML->m_xmlElement->SetAttribute("type", "uint");
			break;
		case kFloat:
			templateEventArgumentXML->m_xmlElement->SetAttribute("type", "float");
			break;
		case kInt64:
			templateEventArgumentXML->m_xmlElement->SetAttribute("type", "int64");
			break;
		case kUInt64:
			templateEventArgumentXML->m_xmlElement->SetAttribute("type", "uint64");
			break;
		}

		templateEventArgumentXML->m_xmlElement->SetAttribute("hidden", hidden);

		return templateEventArgumentXML;
	}

	TaeTemplateEventArgumentXML* TaeTemplateEventXML::addArgumentTemplate(std::string name, DataType type, bool hidden)
	{
		TaeTemplateEventArgumentXML* argTemplate = TaeTemplateEventArgumentXML::create(this->m_xmlElement, name, type, hidden);

		this->m_argTemplates.push_back(argTemplate);

		return argTemplate;
	}

	TaeTemplateEventXML* TaeTemplateEventXML::create(tinyxml2::XMLElement* parent, std::string name, int id)
	{
		TaeTemplateEventXML* templateEventXML = new TaeTemplateEventXML;
		templateEventXML->m_xmlElement = parent->InsertNewChildElement("EventTemplate");
		templateEventXML->m_xmlElement->SetAttribute("id", id);
		templateEventXML->m_xmlElement->SetAttribute("name", name.c_str());

		return templateEventXML;
	}

	TaeTemplateEventXML* TaeTemplateEventGroupXML::addEventTemplate(std::string name, int id)
	{
		TaeTemplateEventXML* eventTemplate = TaeTemplateEventXML::create(this->m_xmlElement, name, id);

		this->m_eventTemplates.push_back(eventTemplate);

		return eventTemplate;
	}

	TaeTemplateEventGroupXML* TaeTemplateEventGroupXML::create(tinyxml2::XMLElement* parent, std::string name, int id)
	{
		TaeTemplateEventGroupXML* templateGroupXML = new TaeTemplateEventGroupXML;

		templateGroupXML->m_xmlElement = parent->InsertNewChildElement("EventGroupTemplate");
		templateGroupXML->m_xmlElement->SetAttribute("id", id);
		templateGroupXML->m_xmlElement->SetAttribute("name", name.c_str());

		return templateGroupXML;
	}

	TaeTemplateXML* TaeTemplateXML::create()
	{
		TaeTemplateXML* templateXML = new TaeTemplateXML;

		templateXML->m_xmlDoc = new tinyxml2::XMLDocument;
		templateXML->m_xmlElement = templateXML->m_xmlDoc->NewElement("TimeActTemplate");
		templateXML->m_xmlDoc->InsertFirstChild(templateXML->m_xmlElement);

		return templateXML;
	}

	TaeTemplateXML* TaeTemplateXML::loadFromFile(std::wstring filepath)
	{
		tinyxml2::XMLDocument* xml = new tinyxml2::XMLDocument;
		xml->LoadFile(RString::toNarrow(filepath).c_str());

		TaeTemplateXML* templateXML = new TaeTemplateXML(xml);

		return templateXML;
	}

	TaeTemplateEventGroupXML* TaeTemplateXML::addEventGroup(std::string name, int id)
	{
		TaeTemplateEventGroupXML* groupTemplate = TaeTemplateEventGroupXML::create(this->m_xmlElement, name, id);

		this->m_eventGroupTemplates.push_back(groupTemplate);

		return groupTemplate;
	}

	bool TaeTemplateXML::save()
	{
		if (this->m_xmlDoc->SaveFile(this->m_dstFileName.c_str()) == tinyxml2::XML_SUCCESS)
			return true;

		return false;
	}

	void TaeTemplateXML::destroy()
	{
		this->m_xmlDoc->Clear();
		this->m_xmlElement = nullptr;

		delete this->m_xmlDoc;
		delete this;
	}
}