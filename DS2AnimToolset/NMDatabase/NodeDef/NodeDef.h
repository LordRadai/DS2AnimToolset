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

		NodeDef() {};
		NodeDef(std::string name, std::string nodeType, int manifestVersion)
			: Node(name), m_nodeType(nodeType), m_manifestVersion(manifestVersion) {
		};

		~NodeDef() {};

	public:
		std::string getNodeType() const { return m_nodeType; }
		int getManifestVersion() const { return m_manifestVersion; }
	};
}