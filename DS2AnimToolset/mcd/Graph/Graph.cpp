#include "Graph.h"
#include "XMLWriter.h"

namespace mcd
{
	tinyxml2::XMLElement* Graph::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);

		XMLWriter::createFloatElement(element, "PanX", m_panX);
		XMLWriter::createFloatElement(element, "PanY", m_panY);

		if (m_type != "")
			XMLWriter::createStringElement(element, "NodeType", m_type);

		if (m_version != 0)
			XMLWriter::createUIntElement(element, "ManifestVersion", m_version);

		return element;
	}
}
