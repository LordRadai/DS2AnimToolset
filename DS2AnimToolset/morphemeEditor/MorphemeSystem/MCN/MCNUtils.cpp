#include "MCNUtils.h"

namespace MCNUtils
{
	tinyxml2::XMLElement* createStringElement(tinyxml2::XMLElement* parent, const char* name, const char* value)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);
		element->SetAttribute("type", "string");

		element->SetText(value);

		return element;
	}

	tinyxml2::XMLElement* createBlendTree(tinyxml2::XMLElement* parent, const char* name)
	{
		return createNodeElement(parent, "BlendTree", name);
	}

	tinyxml2::XMLElement* createNodeElement(tinyxml2::XMLElement* parent, const char* name, const char* nodeName)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);

		element->SetAttribute("name", nodeName);
		element->SetAttribute("type", "node");

		return element;
	}

	tinyxml2::XMLElement* createNodeContainerElement(tinyxml2::XMLElement* parent, const char* name)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);

		element->SetAttribute("type", "nodeContainer");

		return element;
	}

	std::string getElementName(tinyxml2::XMLElement* element)
	{
		if (strcmp(element->Attribute("type"), "nodeContainer") != 0)
			return element->Attribute("name");

		return element->Name();
	}

	std::string getMorphemeDBPointer(tinyxml2::XMLElement* element)
	{
		std::string pointer = getElementName(element);
		tinyxml2::XMLElement* parent = static_cast<tinyxml2::XMLElement*>(element->Parent());

		while (strcmp(parent->Name(), "MorphemeDB") != 0)
		{
			pointer = getElementName(parent) + "." + pointer;
			parent = static_cast<tinyxml2::XMLElement*>(parent->Parent());
		}

		return "MorphemeDB." + pointer;
	}
}