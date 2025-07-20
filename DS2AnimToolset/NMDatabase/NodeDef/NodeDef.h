#pragma once
#include "RCore.h"
#include "Morpheme/mrNodeDef.h"
#include "../Node/Node.h"

namespace db
{
	class NodeDef : public Node
	{
	protected:
		std::string m_nodeType;
		int m_manifestVersion;

		NodeDef(Node* parent, std::string name, std::string nodeType, int manifestVersion) : Node(parent, name), m_nodeType(nodeType), m_manifestVersion(manifestVersion) {};
		
		virtual ~NodeDef() {};

	public:
		virtual bool isValid() const;
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		std::string getNodeType() const { return m_nodeType; }
		int getManifestVersion() const { return m_manifestVersion; }
	};
}