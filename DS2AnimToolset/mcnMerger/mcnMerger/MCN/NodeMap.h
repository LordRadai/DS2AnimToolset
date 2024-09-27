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
		kBlendTree,
		kStateMachine,
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
	private:
		ME::NodeExportXML* m_nodeXml = nullptr;
		int m_nodeID = -1;
		tinyxml2::XMLElement* m_dbNodeEntry = nullptr;
		NodeCategory m_nodeCategory = kInvalid;
		std::string m_name = "";
	};
}