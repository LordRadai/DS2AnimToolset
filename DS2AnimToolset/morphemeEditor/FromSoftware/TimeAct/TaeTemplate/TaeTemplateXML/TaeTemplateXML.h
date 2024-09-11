#pragma once
#include <string>
#include <vector>
#include "RCore.h"

namespace TimeAct
{
	enum DataType
	{
		kBool,
		kByte,
		kUByte,
		kShort,
		kUShort,
		kInt,
		kUInt,
		kFloat,
		kInt64,
		kUInt64,
		kTypeInvalid = -1
	};

	class TaeTemplateArgEnumXML
	{
		friend class TaeTemplateEventArgumentXML;

	public:
		int getValue() const { return this->m_xmlElement->FindAttribute("value")->IntValue(); }
		std::string getName() const { return this->m_xmlElement->FindAttribute("name")->Value(); }

	protected:
		TaeTemplateArgEnumXML() {}

		TaeTemplateArgEnumXML(tinyxml2::XMLElement* element)
		{
			this->m_xmlElement = element;
		}

		~TaeTemplateArgEnumXML() {}

		static TaeTemplateArgEnumXML* create(tinyxml2::XMLElement* parent, std::string name, int value);

		tinyxml2::XMLElement* m_xmlElement = nullptr;
	};

	class TaeTemplateEventArgumentXML
	{
		friend class TaeTemplateEventXML;

	public:
		std::string getName() const { return this->m_xmlElement->FindAttribute("name")->Value(); }
		DataType getType();
		bool isHidden() const { return this->m_xmlElement->FindAttribute("hidden")->BoolValue(); }

		int getNumValues() const { return this->m_xmlElement->ChildElementCount(); }
		TaeTemplateArgEnumXML* getEnumValue(int idx) const { return this->m_enumValues[idx]; }

		TaeTemplateArgEnumXML* addEnumValue(std::string name, int value);
	protected:
		TaeTemplateEventArgumentXML() {}

		TaeTemplateEventArgumentXML(tinyxml2::XMLElement* element)
		{
			this->m_xmlElement = element;

			for (tinyxml2::XMLElement* child = element->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
				this->m_enumValues.push_back(new TaeTemplateArgEnumXML(child));
		}

		~TaeTemplateEventArgumentXML() {}

		static TaeTemplateEventArgumentXML* create(tinyxml2::XMLElement* parent, std::string name, DataType type, bool hidden = false);

		std::vector<TaeTemplateArgEnumXML*> m_enumValues;

		tinyxml2::XMLElement* m_xmlElement = nullptr;
	};

	class TaeTemplateEventXML
	{
		friend class TaeTemplateEventGroupXML;
	public:
		std::string getName() const { return this->m_xmlElement->FindAttribute("name")->Value(); }
		int getId() const { return this->m_xmlElement->FindAttribute("id")->IntValue(); }
		int getNumArguments() const { return this->m_xmlElement->ChildElementCount(); }
		TaeTemplateEventArgumentXML* getArgumentTemplate(int idx) const { return this->m_argTemplates[idx]; }

		TaeTemplateEventArgumentXML* addArgumentTemplate(std::string name, DataType type, bool hidden = false);

	protected:
		TaeTemplateEventXML() {}

		TaeTemplateEventXML(tinyxml2::XMLElement* element) 
		{
			this->m_xmlElement = element;

			for (tinyxml2::XMLElement* child = element->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
				this->m_argTemplates.push_back(new TaeTemplateEventArgumentXML(child));
		}

		~TaeTemplateEventXML() {}

		static TaeTemplateEventXML* create(tinyxml2::XMLElement* parent, std::string name, int id);

		std::vector<TaeTemplateEventArgumentXML*> m_argTemplates;
		tinyxml2::XMLElement* m_xmlElement = nullptr;
	};

	class TaeTemplateEventGroupXML
	{
		friend class TaeTemplateXML;
	public:
		std::string getName() const { return this->m_xmlElement->FindAttribute("name")->Value(); }
		int getId() const { return this->m_xmlElement->FindAttribute("id")->IntValue(); }
		int getNumEvents() const { return this->m_xmlElement->ChildElementCount(); }
		TaeTemplateEventXML* getEventTemplate(int idx) const { return this->m_eventTemplates[idx]; }

		TaeTemplateEventXML* addEventTemplate(std::string name, int id);
	protected:
		TaeTemplateEventGroupXML() {}

		TaeTemplateEventGroupXML(tinyxml2::XMLElement* element)
		{
			this->m_xmlElement = element;

			for (tinyxml2::XMLElement* child = element->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
				this->m_eventTemplates.push_back(new TaeTemplateEventXML(child));
		}

		~TaeTemplateEventGroupXML() {}

		static TaeTemplateEventGroupXML* create(tinyxml2::XMLElement* parent, std::string name, int id);

		std::vector<TaeTemplateEventXML*> m_eventTemplates;
		tinyxml2::XMLElement* m_xmlElement = nullptr;
	};

	class TaeTemplateXML
	{
	public:
		static TaeTemplateXML* create();
		static TaeTemplateXML* loadFromFile(std::wstring filepath);

		std::string getDstFileName() const { return this->m_dstFileName; }
		int getNumEventGroups() const { return this->m_xmlElement->ChildElementCount(); }
		TaeTemplateEventGroupXML* getEventGroupTemplate(int idx) const { return this->m_eventGroupTemplates[idx]; }

		void setDstFileName(std::string filepath) { this->m_dstFileName = filepath; }

		TaeTemplateEventGroupXML* addEventGroup(std::string name, int id);

		bool save();

		void destroy();
	protected:
		TaeTemplateXML() {}

		TaeTemplateXML(tinyxml2::XMLDocument* xml)
		{
			this->m_xmlDoc = xml;
			this->m_xmlElement = xml->FirstChildElement();

			for (tinyxml2::XMLElement* child = this->m_xmlElement->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
				this->m_eventGroupTemplates.push_back(new TaeTemplateEventGroupXML(child));
		}

		~TaeTemplateXML() {}

		std::string m_dstFileName = "";
		std::vector<TaeTemplateEventGroupXML*> m_eventGroupTemplates;

		tinyxml2::XMLDocument* m_xmlDoc = nullptr;
		tinyxml2::XMLElement* m_xmlElement = nullptr;
	};
}
