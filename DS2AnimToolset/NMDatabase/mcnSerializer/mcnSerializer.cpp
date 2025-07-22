#include "mcnSerializer.h"

tinyxml2::XMLElement* mcnSerializer::createElement(tinyxml2::XMLElement* parent, std::string name, std::string text)
{
	tinyxml2::XMLElement* element = parent->InsertNewChildElement(name.c_str());

	if (!text.empty())
		element->SetText(text.c_str());

	return element;
}

tinyxml2::XMLElement* mcnSerializer::createTypedElement(tinyxml2::XMLElement* parent, std::string name, std::string type, std::string text)
{
	tinyxml2::XMLElement* element = createElement(parent, name, text);
	element->SetAttribute("type", type.c_str());

	return element;
}

tinyxml2::XMLElement* mcnSerializer::createNamedAndTypedElement(tinyxml2::XMLElement* parent, std::string name, std::string attribName, std::string type, std::string text)
{
	tinyxml2::XMLElement* element = createElement(parent, name, text);
	element->SetAttribute("name", attribName.c_str());
	element->SetAttribute("type", type.c_str());

	return element;
}

tinyxml2::XMLElement* mcnSerializer::createStringElement(tinyxml2::XMLElement* parent, std::string name, std::string value)
{
	return createTypedElement(parent, name, "string", value);
}

tinyxml2::XMLElement* mcnSerializer::createBoolElement(tinyxml2::XMLElement* parent, std::string name, bool value)
{
	return createTypedElement(parent, name, "bool", std::to_string(value));
}

tinyxml2::XMLElement* mcnSerializer::createIntElement(tinyxml2::XMLElement* parent, std::string name, int value)
{
	return createTypedElement(parent, name, "int", std::to_string(value));
}

tinyxml2::XMLElement* mcnSerializer::createUIntElement(tinyxml2::XMLElement* parent, std::string name, uint32_t value)
{
	return createTypedElement(parent, name, "uint", std::to_string(value));
}

tinyxml2::XMLElement* mcnSerializer::createFloatElement(tinyxml2::XMLElement* parent, std::string name, float value)
{
	return createTypedElement(parent, name, "float", std::to_string(value));
}

tinyxml2::XMLElement* mcnSerializer::createVector3Element(tinyxml2::XMLElement* parent, std::string name, NMP::Vector3 value)
{
	tinyxml2::XMLElement* element = createTypedElement(parent, name, "vector3", "");
	createElement(element, "X", std::to_string(value.x));
	createElement(element, "Y", std::to_string(value.y));
	createElement(element, "Z", std::to_string(value.z));

	return element;
}

tinyxml2::XMLElement* mcnSerializer::createQuatElement(tinyxml2::XMLElement* parent, std::string name, NMP::Quat value)
{
	tinyxml2::XMLElement* element = createTypedElement(parent, name, "quat", "");
	createElement(element, "X", std::to_string(value.x));
	createElement(element, "Y", std::to_string(value.y));
	createElement(element, "Z", std::to_string(value.z));
	createElement(element, "W", std::to_string(value.w));
	return element;
}

tinyxml2::XMLElement* mcnSerializer::createMatrix34Element(tinyxml2::XMLElement* parent, std::string name, NMP::Matrix34 value)
{
	tinyxml2::XMLElement* element = createTypedElement(parent, name, "matrix34", "");

	for (uint32_t i = 0; i < 4; i++)
	{
		char rName[256];
		sprintf(rName, "R%d", i);

		tinyxml2::XMLElement* r = createElement(element, rName, "");

		for (uint32_t j = 0; j < 3; j++)
		{
			char cName[256];
			sprintf(cName, "C%d", j);

			createElement(r, cName, std::to_string(value.r[i][j]));
		}
	}

	return element;
}

tinyxml2::XMLElement* mcnSerializer::createStringArrayElement(tinyxml2::XMLElement* parent, std::string name, std::vector<std::string> array)
{
	tinyxml2::XMLElement* element = createTypedElement(parent, name, "stringArray", "");
	element->SetAttribute("size", static_cast<uint32_t>(array.size()));

	for (const auto& str : array)
	{
		tinyxml2::XMLElement* item = element->InsertNewChildElement("e");
		item->SetText(str.c_str());
	}

	return element;
}

tinyxml2::XMLElement* mcnSerializer::createEnumElement(tinyxml2::XMLElement* parent, std::string name, std::string value)
{
	return createTypedElement(parent, name, "enum", value);
}

tinyxml2::XMLElement* mcnSerializer::createNodeElement(tinyxml2::XMLElement* parent, std::string name, std::string attributeName)
{
	return createNamedAndTypedElement(parent, name, attributeName, "node", "");
}

tinyxml2::XMLElement* mcnSerializer::createNodeContainerElement(tinyxml2::XMLElement* parent, std::string name)
{
	return createTypedElement(parent, name, "nodeContainer", "");
}

tinyxml2::XMLElement* mcnSerializer::createPointerElement(tinyxml2::XMLElement* parent, std::string name, std::string value)
{
	return createTypedElement(parent, name, "pointer", value);
}
