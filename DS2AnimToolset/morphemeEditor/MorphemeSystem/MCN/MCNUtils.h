#pragma once
#include <vector>
#include "RCore.h"

namespace MCNUtils
{
	tinyxml2::XMLElement* createStringElement(tinyxml2::XMLElement* parent, const char* name, const char* value);
	tinyxml2::XMLElement* createBlendTree(tinyxml2::XMLElement* parent, const char* name);
	tinyxml2::XMLElement* createNodeElement(tinyxml2::XMLElement* parent, const char* name, const char* nodeName);
	tinyxml2::XMLElement* createNodeContainerElement(tinyxml2::XMLElement* parent, const char* name);

	std::string getMorphemeDBPointer(tinyxml2::XMLElement* element);
}