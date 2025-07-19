#pragma once
#include "RCore.h"
#include "Morpheme/mrNodeDef.h"

namespace db
{
	class Node
	{
		uint32_t m_nodeTypeID;
		int m_manifestVersion;

	public:
		Node(uint32_t nodeTypeID, int manifestVersion)
			: m_nodeTypeID(nodeTypeID), m_manifestVersion(manifestVersion) {
		};

		~Node() {};

		uint32_t getNodeTypeID() const { return m_nodeTypeID; }
		int getManifestVersion() const { return m_manifestVersion; }
	};
}