#include "GraphNode.h"
#include "XMLWriter.h"

namespace mcd
{
	tinyxml2::XMLElement* GraphNode::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);

		XMLWriter::createFloatElement(element, "XPosition", m_xpos);
		XMLWriter::createFloatElement(element, "YPosition", m_ypos);
		XMLWriter::createFloatElement(element, "Width", m_width);
		XMLWriter::createFloatElement(element, "Height", m_height);

		if (m_type != "")
			XMLWriter::createStringElement(element, "NodeType", m_type);

		if (m_version != 0)
			XMLWriter::createUIntElement(element, "ManifestVersion", m_version);
	}
}