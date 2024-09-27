#include "MCNUtils.h"
#include <string>

namespace MCNUtils
{
	ME::NodeExportXML* getParentStateMachine(ME::NetworkDefExportXML* netDef, ME::NodeExportXML* node)
	{
		int parentIndex = node->getDownstreamParentID();

		while (parentIndex != MR::INVALID_NODE_ID)
		{
			ME::NodeExportXML* parent = static_cast<ME::NodeExportXML*>(netDef->getNode(parentIndex));

			if (parent->getTypeID() == NODE_TYPE_STATE_MACHINE)
				return parent;

			parentIndex = parent->getDownstreamParentID();
		}

		return nullptr;
	}

	std::string getNodeName(ME::NodeExportXML* node)
	{
		std::string input = node->getName();
		std::string delimiter = "|";

		size_t pos = input.find(delimiter);

		if (pos != std::string::npos) {
			std::string name = input.substr(pos + delimiter.length());
			return name;
		}

		return input;
	}

	bool isNodeBlendTree(ME::NodeExportXML* nodeXml)
	{
		if (nodeXml->getNumCommonConditionSets() || nodeXml->getNumConditionSets())
			return true;

		return false;
	}

	bool isNodeControlParam(ME::NodeExportXML* nodeDef)
	{
		switch (nodeDef->getTypeID())
		{
		case NODE_TYPE_CP_BOOL:
		case NODE_TYPE_CP_INT:
		case NODE_TYPE_CP_UINT:
		case NODE_TYPE_CP_FLOAT:
		case NODE_TYPE_CP_VECTOR3:
		case NODE_TYPE_CP_VECTOR4:
			return true;
		default:
			return false;
		}

		return false;
	}

	tinyxml2::XMLElement* createBoolElement(tinyxml2::XMLElement* parent, const char* name, bool value)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);
		element->SetAttribute("type", "bool");

		element->SetText((int)value);

		return element;
	}

	tinyxml2::XMLElement* createVector3Element(tinyxml2::XMLElement* parent, std::string name, float x, float y, float z)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name.c_str());

		element->SetAttribute("type", "vector3");
		RXML::createFloatElement(element, "X", x);
		RXML::createFloatElement(element, "Y", y);
		RXML::createFloatElement(element, "Z", z);

		return element;
	}

	tinyxml2::XMLElement* createVector4Element(tinyxml2::XMLElement* parent, std::string name, float x, float y, float z, float w)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name.c_str());

		element->SetAttribute("type", "quaternion");
		RXML::createFloatElement(element, "X", x);
		RXML::createFloatElement(element, "Y", y);
		RXML::createFloatElement(element, "Z", z);
		RXML::createFloatElement(element, "W", w);

		return element;
	}

	tinyxml2::XMLElement* createQuaternionElement(tinyxml2::XMLElement* parent, std::string name, float x, float y, float z, float w)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name.c_str());

		element->SetAttribute("type", "quaternion");
		RXML::createFloatElement(element, "X", x);
		RXML::createFloatElement(element, "Y", y);
		RXML::createFloatElement(element, "Z", z);
		RXML::createFloatElement(element, "W", w);

		return element;
	}

	tinyxml2::XMLElement* createEnumElement(tinyxml2::XMLElement* parent, const char* name, const char* value)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);
		element->SetAttribute("type", "enum");

		element->SetText(value);

		return element;
	}

	tinyxml2::XMLElement* createStringElement(tinyxml2::XMLElement* parent, const char* name, const char* value)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);
		element->SetAttribute("type", "string");

		element->SetText(value);

		return element;
	}

	void addMatrixRow(tinyxml2::XMLElement* parent, NMP::Matrix34 matrix, int rowIdx)
	{
		char rowName[256];
		sprintf_s(rowName, "R%d", rowIdx);

		tinyxml2::XMLElement* row = parent->InsertNewChildElement(rowName);
		tinyxml2::XMLElement* c0 = row->InsertNewChildElement("C0");
		c0->SetText(matrix.r[rowIdx].x);

		tinyxml2::XMLElement* c1 = row->InsertNewChildElement("C1");
		c1->SetText(matrix.r[rowIdx].y);

		tinyxml2::XMLElement* c2 = row->InsertNewChildElement("C2");
		c2->SetText(matrix.r[rowIdx].z);
	}

	tinyxml2::XMLElement* createMatrix34Element(tinyxml2::XMLElement* parent, const char* name, NMP::Matrix34 value)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);
		element->SetAttribute("type", "matrix34");

		addMatrixRow(element, value, 0);
		addMatrixRow(element, value, 1);
		addMatrixRow(element, value, 2);
		addMatrixRow(element, value, 3);

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

	tinyxml2::XMLElement* craetePointerElement(tinyxml2::XMLElement* parent, const char* name, tinyxml2::XMLElement* ptrTo)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);
		element->SetAttribute("type", "pointer");

		element->SetText(getMorphemeDBPointer(ptrTo).c_str());

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

		pointer = "MorphemeDB." + pointer;

		return pointer;
	}
}