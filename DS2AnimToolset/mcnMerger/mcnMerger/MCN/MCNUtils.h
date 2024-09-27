#pragma once
#include <vector>
#include "RCore.h"
#include "NMPlatform/NMMatrix34.h"
#include "export/mcExportXml.h"

namespace MCNUtils
{
	ME::NodeExportXML* getParentStateMachine(ME::NetworkDefExportXML* netDef, ME::NodeExportXML* node);
	std::string getNodeName(ME::NodeExportXML* node);
	bool isNodeBlendTree(ME::NodeExportXML* nodeXml);
	bool isNodeControlParam(ME::NodeExportXML* nodeDef);

	tinyxml2::XMLElement* createBoolElement(tinyxml2::XMLElement* parent, const char* name, bool value);
	tinyxml2::XMLElement* createVector3Element(tinyxml2::XMLElement* parent, std::string name, float x, float y, float z);
	tinyxml2::XMLElement* createVector4Element(tinyxml2::XMLElement* parent, std::string name, float x, float y, float z, float w);
	tinyxml2::XMLElement* createQuaternionElement(tinyxml2::XMLElement* parent, std::string name, float x, float y, float z, float w);
	tinyxml2::XMLElement* createEnumElement(tinyxml2::XMLElement* parent, const char* name, const char* value);
	tinyxml2::XMLElement* createStringElement(tinyxml2::XMLElement* parent, const char* name, const char* value);
	tinyxml2::XMLElement* createMatrix34Element(tinyxml2::XMLElement* parent, const char* name, NMP::Matrix34 value);
	tinyxml2::XMLElement* createBlendTree(tinyxml2::XMLElement* parent, const char* name);
	tinyxml2::XMLElement* createNodeElement(tinyxml2::XMLElement* parent, const char* name, const char* nodeName);
	tinyxml2::XMLElement* createNodeContainerElement(tinyxml2::XMLElement* parent, const char* name);
	tinyxml2::XMLElement* craetePointerElement(tinyxml2::XMLElement* parent, const char* name, tinyxml2::XMLElement* ptrTo);

	std::string getMorphemeDBPointer(tinyxml2::XMLElement* element);
}