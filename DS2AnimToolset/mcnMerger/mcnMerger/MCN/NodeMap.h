#pragma once
#include "RCore.h"

#include "export/mcExportXml.h"
#include "export/mcExportMessagePresetsXml.h"
#include "export/mcExportControllerXml.h"

#include "morpheme/mrEventTrackDuration.h"
#include "morpheme/mrEventTrackDiscrete.h"
#include "morpheme/mrNodeDef.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/AnimSource/mrAnimSource.h"

#include "simpleBundle/simpleAnimRuntimeIDtoFilenameLookup.h"

namespace MCN
{
	enum NodeCategory
	{
		kNode = 0,
		kNetwork,
		kBlendTree,
		kStateMachine,
		kTransition,
		kInvalid = -1
	};

	class NodeMap
	{
	public:
		NodeMap() {}
		NodeMap(ME::NodeExportXML* src, NodeCategory category, std::string name);
		~NodeMap() {}

		ME::NodeExportXML* getSourceNode() const { return this->m_nodeXml; }
		tinyxml2::XMLElement* getDbEntry() const { return this->m_dbNodeEntry; }
		std::string getName() const { return this->m_name; }
		int getNodeID() const { return this->m_nodeID; }
		int getParentNodeID() const { return this->m_parentNodeID; }
		NodeCategory getNodeCategory() const { return this->m_nodeCategory; }

		void setDbEntry(tinyxml2::XMLElement* entry) { this->m_dbNodeEntry = entry; }

	private:
		ME::NodeExportXML* m_nodeXml = nullptr;
		int m_nodeID = -1;
		int m_parentNodeID = -1;
		tinyxml2::XMLElement* m_dbNodeEntry = nullptr;
		NodeCategory m_nodeCategory = kInvalid;
		std::string m_name = "";
	};
}