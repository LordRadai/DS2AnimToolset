#include "MCN.h"
#include "MCNUtils.h"
#include "RCore.h"
#include "extern.h"

namespace MCN
{
	Node* Node::create(Network* owner, ME::NodeExportXML* nodeXml)
	{
		Node* node = new Node;
		node->m_owner = owner;
		node->m_xmlElement = owner->m_xmlElement->FirstChildElement("GraphEntry")->InsertNewChildElement("");

		return node;
	}

	TransitionEdge* TransitionEdge::create(Node* owner, ME::NodeExportXML* node)
	{
		TransitionEdge* transitEdge = new TransitionEdge;

		return transitEdge;
	}

	ControlParameter* ControlParameter::createBoolCp(tinyxml2::XMLElement* parent, ME::NodeExportXML* cpNode)
	{
		ControlParameter* cp = new ControlParameter;
		cp->m_xmlElement = MCNUtils::createNodeElement(parent, "ControlParameter", MCNUtils::getNodeNameWithoutParent(cpNode).c_str());
		tinyxml2::XMLElement* pinEntry = MCNUtils::createNodeContainerElement(cp->m_xmlElement, "DataPinEntry");
		tinyxml2::XMLElement* dataPin = MCNUtils::createNodeElement(pinEntry, "DataPin", "Result");
		MCNUtils::createEnumElement(dataPin, "Type", "bool");

		bool out;
		cpNode->getDataBlock()->readBool(out, "DefaultBool");

		MCNUtils::createBoolElement(cp->m_xmlElement, "DefaultBool", out);

		return cp;
	}

	ControlParameter* ControlParameter::createFloatCp(tinyxml2::XMLElement* parent, ME::NodeExportXML* cpNode)
	{
		ControlParameter* cp = new ControlParameter;
		cp->m_xmlElement = MCNUtils::createNodeElement(parent, "ControlParameter", MCNUtils::getNodeNameWithoutParent(cpNode).c_str());
		tinyxml2::XMLElement* pinEntry = MCNUtils::createNodeContainerElement(cp->m_xmlElement, "DataPinEntry");
		tinyxml2::XMLElement* dataPin = MCNUtils::createNodeElement(pinEntry, "DataPin", "Result");

		float out;
		cpNode->getDataBlock()->readFloat(out, "DefaultValue_0");

		RXML::createFloatElement(cp->m_xmlElement, "DefaultFloat", out);

		if (out < 0.f)
			RXML::createFloatElement(cp->m_xmlElement, "Min", out);
		else if(out > 1.f)
			RXML::createFloatElement(cp->m_xmlElement, "Max", out);

		return cp;
	}

	ControlParameter* ControlParameter::createIntCp(tinyxml2::XMLElement* parent, ME::NodeExportXML* cpNode)
	{
		ControlParameter* cp = new ControlParameter;
		cp->m_xmlElement = MCNUtils::createNodeElement(parent, "ControlParameter", MCNUtils::getNodeNameWithoutParent(cpNode).c_str());
		tinyxml2::XMLElement* pinEntry = MCNUtils::createNodeContainerElement(cp->m_xmlElement, "DataPinEntry");
		tinyxml2::XMLElement* dataPin = MCNUtils::createNodeElement(pinEntry, "DataPin", "Result");
		MCNUtils::createEnumElement(dataPin, "Type", "int");

		int out;
		cpNode->getDataBlock()->readInt(out, "DefaultInt");

		RXML::createIntElement(cp->m_xmlElement, "DefaultInt", out);

		if (out < 0)
			RXML::createIntElement(cp->m_xmlElement, "Min", out);
		else if (out > 10)
			RXML::createIntElement(cp->m_xmlElement, "Max", out);

		return cp;
	}

	ControlParameter* ControlParameter::createUIntCp(tinyxml2::XMLElement* parent, ME::NodeExportXML* cpNode)
	{
		ControlParameter* cp = new ControlParameter;
		cp->m_xmlElement = MCNUtils::createNodeElement(parent, "ControlParameter", MCNUtils::getNodeNameWithoutParent(cpNode).c_str());
		tinyxml2::XMLElement* pinEntry = MCNUtils::createNodeContainerElement(cp->m_xmlElement, "DataPinEntry");
		tinyxml2::XMLElement* dataPin = MCNUtils::createNodeElement(pinEntry, "DataPin", "Result");
		MCNUtils::createEnumElement(dataPin, "Type", "uint");

		unsigned int out;
		cpNode->getDataBlock()->readUInt(out, "DefaultInt");

		RXML::createIntElement(cp->m_xmlElement, "DefaultInt", out);

		if (out < 0)
			RXML::createIntElement(cp->m_xmlElement, "Min", out);
		else if (out > 10)
			RXML::createIntElement(cp->m_xmlElement, "Max", out);

		return cp;
	}

	ControlParameter* ControlParameter::createVector3Cp(tinyxml2::XMLElement* parent, ME::NodeExportXML* cpNode)
	{
		ControlParameter* cp = new ControlParameter;
		cp->m_xmlElement = MCNUtils::createNodeElement(parent, "ControlParameter", MCNUtils::getNodeNameWithoutParent(cpNode).c_str());
		tinyxml2::XMLElement* pinEntry = MCNUtils::createNodeContainerElement(cp->m_xmlElement, "DataPinEntry");
		tinyxml2::XMLElement* dataPin = MCNUtils::createNodeElement(pinEntry, "DataPin", "Result");
		MCNUtils::createEnumElement(dataPin, "Type", "vector3");

		float x;
		float y;
		float z;
		cpNode->getDataBlock()->readFloat(x, "DefaultValue_0");
		cpNode->getDataBlock()->readFloat(y, "DefaultValue_1");
		cpNode->getDataBlock()->readFloat(z, "DefaultValue_2");

		MCNUtils::createVector3Element(cp->m_xmlElement, "DefaultVector3", x, y, z);

		float maxVal = std::fmaxf(std::fmaxf(x, y), z);
		float minVal = std::fminf(std::fminf(x, y), z);

		if (maxVal > 1.f)
			RXML::createFloatElement(cp->m_xmlElement, "Max", maxVal);

		if (minVal < 0.f)
			RXML::createFloatElement(cp->m_xmlElement, "Min", minVal);

		return cp;
	}

	ControlParameter* ControlParameter::createVector4Cp(tinyxml2::XMLElement* parent, ME::NodeExportXML* cpNode)
	{
		ControlParameter* cp = new ControlParameter;
		cp->m_xmlElement = MCNUtils::createNodeElement(parent, "ControlParameter", MCNUtils::getNodeNameWithoutParent(cpNode).c_str());
		tinyxml2::XMLElement* pinEntry = MCNUtils::createNodeContainerElement(cp->m_xmlElement, "DataPinEntry");
		tinyxml2::XMLElement* dataPin = MCNUtils::createNodeElement(pinEntry, "DataPin", "Result");
		MCNUtils::createEnumElement(dataPin, "Type", "vector3");

		float x;
		float y;
		float z;
		float w;
		cpNode->getDataBlock()->readFloat(x, "DefaultValue_0");
		cpNode->getDataBlock()->readFloat(y, "DefaultValue_1");
		cpNode->getDataBlock()->readFloat(z, "DefaultValue_2");
		cpNode->getDataBlock()->readFloat(w, "DefaultValue_3");

		MCNUtils::createVector4Element(cp->m_xmlElement, "DefaultQuat", x, y, z, w);

		float maxVal = std::fmaxf(std::fmaxf(std::fmaxf(x, y), z), w);
		float minVal = std::fminf(std::fminf(std::fminf(x, y), z), w);

		if (maxVal > 1.f)
			RXML::createFloatElement(cp->m_xmlElement, "Max", maxVal);

		if (minVal < 0.f)
			RXML::createFloatElement(cp->m_xmlElement, "Min", minVal);

		return cp;
	}

	ControlParameter* ControlParameter::create(Network* owner, ME::NodeExportXML* cpNode)
	{
		tinyxml2::XMLElement* cpArray = owner->m_xmlElement->FirstChildElement("ControlParametersNode")->FirstChildElement("ControlParameterArray");

		switch (cpNode->getTypeID())
		{
		case NODE_TYPE_CP_BOOL:
			return createBoolCp(cpArray, cpNode);
		case NODE_TYPE_CP_INT:
			return createIntCp(cpArray, cpNode);
		case NODE_TYPE_CP_UINT:
			return createUIntCp(cpArray, cpNode);
		case NODE_TYPE_CP_FLOAT:
			return createFloatCp(cpArray, cpNode);
		case NODE_TYPE_CP_VECTOR3:
			return createVector3Cp(cpArray, cpNode);
		case NODE_TYPE_CP_VECTOR4:
			return createVector4Cp(cpArray, cpNode);
		default:
			throw("Node %d is not a ControlParam node", cpNode->getNodeID());
			break;
		}
	}

	AnimationSet* AnimationSet::create(Network* owner, const char* name, ME::RigExportXML* rig)
	{
		AnimationSet* animSet = new AnimationSet;

		animSet->m_owner = owner;

		tinyxml2::XMLElement* animSetArray = owner->m_xmlElement->FirstChildElement("AnimationSetsNode")->FirstChildElement("AnimationSetArray");

		if (animSetArray == nullptr)
			animSetArray = MCNUtils::createNodeContainerElement(owner->m_xmlElement->FirstChildElement("AnimationSetsNode"), "AnimationSetArray");

		animSet->m_xmlElement = MCNUtils::createNodeElement(animSetArray, "AnimationSet", name);

		char rigPath[256];
		sprintf_s(rigPath, "$(RootDir)\\%s\\XMD\\Rig\\%s", name, std::filesystem::path(rig->getDestFilename()).filename().string().c_str());

		MCNUtils::createStringElement(animSet->m_xmlElement, "Rig", rigPath);

		char skinPath[256];
		sprintf_s(skinPath, "$(RootDir)\\%s\\XMD\\Rig\\%s.mcskin", name, name);

		MCNUtils::createNodeContainerElement(animSet->m_xmlElement, "Skins");
		tinyxml2::XMLElement* skin = animSet->addSkin(name, skinPath);

		MCNUtils::craetePointerElement(animSet->m_xmlElement, "AssetManagerSkin", skin);
		MCNUtils::createStringElement(animSet->m_xmlElement, "Format", "nsa");
		animSet->setRigChannels(rig);

		MCNUtils::createStringElement(animSet->m_xmlElement, "NetworkFollowJoint", "CharacterWorldSpaceTM");
		MCNUtils::createStringElement(animSet->m_xmlElement, "AssetManagerFollowJoint", "CharacterWorldSpaceTM");
		MCNUtils::createStringElement(animSet->m_xmlElement, "Template", "$(AppRoot)resources\\rigTemplates\\Humanoid.mctmpl");

		NMP::Matrix34 retargetSrcStartPos = NMP::Matrix34Identity();
		retargetSrcStartPos.setTranslation(NMP::Vector3(1.5f, 0.f, 0.f));
		MCNUtils::createMatrix34Element(animSet->m_xmlElement, "RetargetSrcStartPointLocation", retargetSrcStartPos);

		return animSet;
	}

	void AnimationSet::setRigChannels(ME::RigExportXML* rig)
	{
		tinyxml2::XMLElement* bones = this->m_xmlElement->InsertNewChildElement("RigChannelNames");
		bones->SetAttribute("type", "stringArray");
		bones->SetAttribute("size", rig->getNumJoints());

		for (size_t i = 0; i < rig->getNumJoints(); i++)
		{
			tinyxml2::XMLElement* rigChannelJoint = bones->InsertNewChildElement("e");
			rigChannelJoint->SetText(rig->getJoint(i)->getName());
		}
	}

	tinyxml2::XMLElement* AnimationSet::addSkin(const char* name, const char* filename)
	{
		tinyxml2::XMLElement* skins = this->m_xmlElement->FirstChildElement("Skins");
		tinyxml2::XMLElement* skin = MCNUtils::createNodeElement(skins, "Skin", name);
		MCNUtils::createStringElement(skin, "Path", filename);

		return skin;
	}

	AnimLocation* AnimLocation::create(Network* owner, const char* sourceDir, const char* markupDir, bool includeSubDirs)
	{
		AnimLocation* animLoc = new AnimLocation;
		animLoc->m_owner = owner;
		animLoc->m_xmlElement = MCNUtils::createNodeElement(owner->m_xmlElement->FirstChildElement("AnimationLocations"), "AnimationLocation", "AnimationLocation");

		animLoc->setSourceDirectory(sourceDir);
		animLoc->setMarkupDirectory(markupDir);
		animLoc->setIncludeSubDirs(includeSubDirs);

		return animLoc;
	}

	void AnimLocation::setSourceDirectory(const char* sourceDir)
	{
		tinyxml2::XMLElement* xmlElement = this->m_xmlElement->InsertNewChildElement("SourceDirectory");
		xmlElement->SetAttribute("type", "string");
		xmlElement->SetText(sourceDir);
	}

	void AnimLocation::setMarkupDirectory(const char* markupDir)
	{
		tinyxml2::XMLElement* xmlElement = this->m_xmlElement->InsertNewChildElement("MarkupDirectory");
		xmlElement->SetAttribute("type", "string");
		xmlElement->SetText(markupDir);
	}

	void AnimLocation::setIncludeSubDirs(bool include)
	{
		tinyxml2::XMLElement* xmlElement = this->m_xmlElement->InsertNewChildElement("IncludeSubDirectories");
		xmlElement->SetAttribute("type", "bool");
		xmlElement->SetText((int)include);
	}

	PreviewScript* PreviewScript::loadFromFile(Network* owner, const char* name)
	{
		PreviewScript* script = new PreviewScript;
		script->m_owner = owner;

		script->m_xmlElement = owner->m_xmlElement->FirstChildElement("PreviewScripts")->FirstChildElement("PreviewScriptNode");

		return script;
	}

	PreviewScript* PreviewScript::create(Network* owner, const char* name, const char* filename)
	{
		PreviewScript* script = new PreviewScript;
		script->m_owner = owner;
		script->m_xmlElement = MCNUtils::createNodeElement(owner->m_xmlElement->FirstChildElement("PreviewScripts"), "PreviewScriptNode", name);
		
		script->setScriptData(filename);

		return script;
	}

	void PreviewScript::setScriptData(const char* filename)
	{
		tinyxml2::XMLElement* scriptData = this->m_xmlElement->InsertNewChildElement("ScriptData");
		scriptData->SetAttribute("type", "string");
		
		std::ifstream file(filename);

		if (!file.is_open())
			return;

		std::stringstream buffer;
		buffer << file.rdbuf();

		std::string fileContent =  buffer.str();

		file.close();

		tinyxml2::XMLText* cdata = this->m_xmlElement->GetDocument()->NewText(fileContent.c_str());
		cdata->SetCData(true);

		scriptData->InsertEndChild(cdata);
	}

	Network* Network::loadFromFile(Networks* owner, const char* name)
	{
		Network* net = new Network;
		net->m_owner = owner;
		net->m_xmlElement = owner->m_xmlElement->FirstChildElement(name);

		if (net->m_xmlElement == nullptr)
		{
			throw("Invalid mcn file");
			return nullptr;
		}

		net->m_scripts.push_back(PreviewScript::loadFromFile(net, "Preview script"));

		return net;
	}

	Network* Network::create(Networks* owner, const char* name, ME::AnimationLibraryXML* animLibrary, ME::RigExportXML* rig, ME::NetworkDefExportXML* netDef, ME::MessagePresetLibraryExportXML* messagePresets, const char* animSetName)
	{
		Network* net = new Network;
		net->m_owner = owner;
		net->m_xmlElement = MCNUtils::createNodeElement(owner->m_xmlElement, "Network", "Network");

		tinyxml2::XMLElement* graphEntry = MCNUtils::createNodeContainerElement(net->m_xmlElement, "GraphEntry");

		net->createRootBlendTree();

		tinyxml2::XMLElement* animLocs = MCNUtils::createNodeContainerElement(net->m_xmlElement, "AnimationLocations");
		tinyxml2::XMLElement* scripts = MCNUtils::createNodeContainerElement(net->m_xmlElement, "PreviewScripts");

		net->addPreviewScript("Preview script", "Data\\res\\DefaultPreviewScript.lua");

		GUID guid;
		CoCreateGuid(&guid);

		MCNUtils::createStringElement(net->m_xmlElement, "GUID", RString::guidToString(guid).c_str());

		net->createAnimLibraryRef(animLibrary);

		MCNUtils::createStringElement(net->m_xmlElement, "AssetManagerSelectedSet", animSetName);

		net->createPassDownPin();
		net->createControlParametersNode();

		for (size_t i = 0; i < netDef->getNumNodes(); i++)
		{
			ME::NodeExportXML* nodeDef = static_cast<ME::NodeExportXML*>(netDef->getNode(i));

			if (MCNUtils::isNodeControlParam(nodeDef))
				net->addControlParameter(nodeDef);
		}

		MCNUtils::createNodeElement(net->m_xmlElement, "EmittedControlParametersNode", "EmittedControlParameters");
		MCNUtils::createNodeElement(net->m_xmlElement, "AnimationSetsNode", "AnimationSets");
		net->addAnimSet(animSetName, rig);

		MCNUtils::createNodeElement(net->m_xmlElement, "SceneGraphRootNode", "SceneGraphRoot");
		MCNUtils::createNodeElement(net->m_xmlElement, "LayersNode", "Layers");
		MCNUtils::createNodeElement(net->m_xmlElement, "BodyGroupsNode", "BodyGroups");
		MCNUtils::createNodeElement(net->m_xmlElement, "RequestsNode", "Requests");

		for (size_t i = 0; i < netDef->getNumMessages(); i++)
			net->addRequest((ME::MessageExportXML*)netDef->getMessage(i));

		MCNUtils::createNodeElement(net->m_xmlElement, "RequestPresetsNode", "RequestPresets");
		MCNUtils::createNodeElement(net->m_xmlElement, "MessagePresetGroupsNode", "MessagePresetGroups");
		MCNUtils::createNodeElement(net->m_xmlElement, "CharacterStartPointsNode", "CharacterStartPoints");
		net->addStartPoint(animSetName);

		return net;
	}

	void Network::addAnimLocation(const char* sourceDir, const char* markupDir, bool includeSubDirectories)
	{
		AnimLocation* animLoc = AnimLocation::create(this, sourceDir, markupDir, includeSubDirectories);

		this->m_animLocations.push_back(animLoc);
	}

	void Network::addPreviewScript(const char* name, const char* filepath)
	{
		PreviewScript* script = PreviewScript::create(this, name, filepath);

		this->m_scripts.push_back(script);
	}

	void Network::addAnimSet(const char* name, ME::RigExportXML* rig)
	{
		AnimationSet* animSet = AnimationSet::create(this, name, rig);

		this->m_animSets.push_back(animSet);
	}

	void Network::addStartPoint(const char* name)
	{
		tinyxml2::XMLElement* animSetArray = this->m_xmlElement->FirstChildElement("AnimationSetsNode")->FirstChildElement("AnimationSetArray");
		tinyxml2::XMLElement* targetAnimSet = nullptr;

		for (tinyxml2::XMLElement* animSet = animSetArray->FirstChildElement(); animSet != nullptr; animSet = animSet->NextSiblingElement())
		{
			if (strcmp(animSet->Attribute("name"), name) == 0)
			{
				targetAnimSet = animSet;
				break;
			}
		}
		
		tinyxml2::XMLElement* startPointNode = this->m_xmlElement->FirstChildElement("CharacterStartPointsNode");
		tinyxml2::XMLElement* startPoints = startPointNode->FirstChildElement("CharacterStartPointsArray");

		if (startPoints == nullptr)
			startPoints = startPointNode->InsertNewChildElement("CharacterStartPointsArray");

		tinyxml2::XMLElement* linkNode = MCNUtils::createNodeElement(startPoints, "CharacterStartPointLinkNode", "StartPoint");
		MCNUtils::craetePointerElement(linkNode, "AnimationSet", targetAnimSet);
	}

	void Network::addRequest(ME::MessageExportXML* request)
	{
		tinyxml2::XMLElement* requestNode = this->m_xmlElement->FirstChildElement("RequestsNode");
		tinyxml2::XMLElement* requests = requestNode->FirstChildElement("RequestArray");

		if (requests == nullptr)
			requests = MCNUtils::createNodeContainerElement(requestNode, "RequestArray");

		if (requests->FirstChildElement(request->getName()) == nullptr)
			MCNUtils::createNodeElement(requests, "Request", request->getName());
	}

	void Network::addControlParameter(ME::NodeExportXML* cp)
	{
		tinyxml2::XMLElement* cpNode = this->m_xmlElement->FirstChildElement("ControlParametersNode");
		tinyxml2::XMLElement* cpArray = cpNode->FirstChildElement("ControlParameterArray");

		if (cpArray == nullptr)
			cpArray = MCNUtils::createNodeContainerElement(cpNode, "ControlParameterArray");

		if (cpArray->FirstChildElement(MCNUtils::getNodeNameWithoutParent(cp).c_str()) == nullptr)
		{
			ControlParameter* controlParam = ControlParameter::create(this, cp);

			this->m_controlParameters.push_back(controlParam);
		}
	}

	NodeMap* Network::getParentNodeContainer(int nodeID)
	{
		int parentNodeID = this->getNodeMap(nodeID)->getParentBTNodeID();

		if (parentNodeID == -1)
			parentNodeID = this->getNodeMap(nodeID)->getParentSMNodeID();

		while (parentNodeID != -1)
		{
			NodeMap* nodeMap = this->getBTNodeMap(parentNodeID);

			if (nodeMap == nullptr)
				nodeMap = this->getNodeMap(parentNodeID);

			if ((nodeMap->getNodeCategory() == kNetwork) || (nodeMap->getNodeCategory() == kBlendTree) || (nodeMap->getNodeCategory() == kStateMachine))
				return nodeMap;

			parentNodeID = nodeMap->getParentBTNodeID();

			if (parentNodeID == -1)
				parentNodeID = nodeMap->getParentSMNodeID();
		}

		return nullptr;
	}

	const char* getNodeEntryTypeName(NodeMap* nodeMap)
	{
		switch (nodeMap->getNodeCategory())
		{
		case kBlendTree:
			return "BlendTree";
		case kStateMachine:
			return "StateMachine";
		default:
			return "";
		}
	}

	void Network::writeNonContainerNode(NodeMap* nodeMap, tinyxml2::XMLElement* parent)
	{
		g_appLog->debugMessage(MsgLevel_Info, "Adding node %s to %s\n", nodeMap->getName().c_str(), MCNUtils::getMorphemeDBPointer(parent).c_str());

		//TODO: Implement
	}

	void Network::writeStateMachineNode(NodeMap* nodeMap, tinyxml2::XMLElement* parent)
	{
		g_appLog->debugMessage(MsgLevel_Info, "Adding node %s to %s\n", nodeMap->getName().c_str(), MCNUtils::getMorphemeDBPointer(parent).c_str());

		tinyxml2::XMLElement* nodeEntry = MCNUtils::createNodeElement(parent, "StateMachine", "StateMachine");

		RXML::createFloatElement(nodeEntry, "PanX", 441);
		RXML::createFloatElement(nodeEntry, "PanY", 174);
		MCNUtils::createStringElement(nodeEntry, "NodeType", "StateMachine");
		RXML::createIntElement(nodeEntry, "ManifestVersion", 1);
		MCNUtils::createNodeContainerElement(nodeEntry, "StateMachineNodes");
		MCNUtils::createNodeContainerElement(nodeEntry, "TransitionEdges");

		nodeMap->setDbEntry(nodeEntry);
	}

	void Network::writeBlendTreeNode(NodeMap* nodeMap, tinyxml2::XMLElement* parent)
	{
		g_appLog->debugMessage(MsgLevel_Info, "Adding node %s to %s\n", nodeMap->getName().c_str(), MCNUtils::getMorphemeDBPointer(parent).c_str());

		//TODO: Implement
		tinyxml2::XMLElement* nodeEntry = MCNUtils::createNodeElement(parent, "BlendTree", "BlendTree");

		MCNUtils::createNodeContainerElement(nodeEntry, "FlowEdges");
		RXML::createFloatElement(nodeEntry, "ZoomFactor", 1.f);
		RXML::createFloatElement(nodeEntry, "PanX", 513.f);
		RXML::createFloatElement(nodeEntry, "PanY", 77.f);
		MCNUtils::createNodeContainerElement(nodeEntry, "BlendTreeNodes");
		RXML::createFloatElement(nodeEntry, "ControlParamXPosition", -420.f);
		RXML::createFloatElement(nodeEntry, "ControlParamYPosition", 380.f);
		RXML::createFloatElement(nodeEntry, "ControlParamWidth", 155.f);
		RXML::createFloatElement(nodeEntry, "ControlParamHeight", 134.f);
		RXML::createFloatElement(nodeEntry, "OutputPinXPosition", 450.f);
		RXML::createFloatElement(nodeEntry, "OutputPinYPosition", 270.f);
		RXML::createFloatElement(nodeEntry, "OutputPinWidth", 62.f);
		RXML::createFloatElement(nodeEntry, "OutputPinHeight", 45.f);

		nodeMap->setDbEntry(nodeEntry);
	}

	void Network::writeTransitionNode(NodeMap* nodeMap, tinyxml2::XMLElement* parent)
	{
		g_appLog->debugMessage(MsgLevel_Info, "Adding node %s to %s\n", nodeMap->getName().c_str(), MCNUtils::getMorphemeDBPointer(parent).c_str());

		//TODO: Implement
	}

	void Network::setNodeMapParentBTs()
	{

	}

	int Network::getNumBTNodes()
	{
		int count = 0;

		for (size_t i = 0; i < this->m_nodesMap.size(); i++)
		{
			if (this->m_nodesMap[i]->getNodeCategory() == kBlendTree)
				count++;
		}

		return count;
	}

	tinyxml2::XMLElement* createBlendTreeNodeEntry(tinyxml2::XMLElement* where, const char* name, float x, float y)
	{
		tinyxml2::XMLElement* node = MCNUtils::createNodeContainerElement(MCNUtils::createNodeElement(where->FirstChildElement("BlendTreeNodes"), "BlendTreeNode", name), "GraphEntry");

		return node;
	}

	tinyxml2::XMLElement* createStateMachineNodeEntry(tinyxml2::XMLElement* where, const char* name, float x, float y)
	{
		tinyxml2::XMLElement* node = MCNUtils::createNodeContainerElement(MCNUtils::createNodeElement(where->FirstChildElement("StateMachineNodes"), "StateMachineNode", name), "GraphEntry");

		return node;
	}

	void Network::writeNode(NodeMap* nodeMap, NodeMap* parentNodeMap)
	{
		tinyxml2::XMLElement* whereToAdd = parentNodeMap->getDbEntry();

		if (nodeMap->getNodeCategory() != kTransition)
		{
			switch (parentNodeMap->getNodeCategory())
			{
			case MCN::kBlendTree:
				whereToAdd = createBlendTreeNodeEntry(whereToAdd, nodeMap->getName().c_str(), 0, 0);
				break;
			case MCN::kStateMachine:
				whereToAdd = createStateMachineNodeEntry(whereToAdd, nodeMap->getName().c_str(), 0, 0);
				break;
			default:
				break;
			}
		}

		switch (nodeMap->getNodeCategory())
		{
		case MCN::kNode:
			writeNonContainerNode(nodeMap, whereToAdd);
			break;
		case MCN::kNetwork:
			throw("A node of type Network should already be a part of the MorphemeDB");
			break;
		case MCN::kBlendTree:
			writeBlendTreeNode(nodeMap, whereToAdd);
			break;
		case MCN::kStateMachine:
			writeStateMachineNode(nodeMap, whereToAdd);
			break;
		case MCN::kTransition:
			writeTransitionNode(nodeMap, whereToAdd->FirstChildElement("TransitionEdges"));
			break;
		default:
			break;
		}
	}

	void Network::addNode(int nodeID)
	{
		NodeMap* nodeMap = getNodeMap(nodeID);

		if (nodeMap == nullptr || nodeMap->getDbEntry() != nullptr)
			return;

		NodeMap* parentNodeMap = getParentNodeContainer(nodeID);

		if (parentNodeMap == nullptr)
		{
			throw("Invalid Network hierarchy. Node %d has no valid parent", nodeMap->getNodeID());
			return;
		}

		if (parentNodeMap->getDbEntry() == nullptr)
			addNode(parentNodeMap->getNodeID());

		writeNode(nodeMap, parentNodeMap);
	}

	void Network::setStateMachinesDefaultStates()
	{
		for (size_t i = 0; i < this->m_nodesMap.size(); i++)
		{
			NodeMap* nodeMap = this->m_nodesMap[i];

			if (nodeMap->getNodeCategory() == kStateMachine)
			{
				int defaultStateId = 0;
				nodeMap->getSourceNode()->getDataBlock()->readNetworkNodeId(defaultStateId, "DefaultNodeID");
				MCNUtils::craetePointerElement(nodeMap->getDbEntry(), "DefaultState", (tinyxml2::XMLElement*)this->getNodeMap(defaultStateId)->getDbEntry()->Parent()->Parent());
			}
		}
	}

	void Network::buildNodeMap(ME::NetworkDefExportXML* netDef, ME::AnimationLibraryXML* animLibrary)
	{
		this->m_xmlElement->FirstChildElement("GraphEntry")->DeleteChildren();

		for (size_t i = 0; i < netDef->getNumNodes(); i++)
		{
			ME::NodeExportXML* node = static_cast<ME::NodeExportXML*>(netDef->getNode(i));
			int nodeType = node->getTypeID();

			//If the node is a control parameter then do nothing
			if ((nodeType < NODE_TYPE_ANIMATION) && (nodeType != NODE_TYPE_STATE_MACHINE) && (nodeType != NODE_TYPE_NETWORK))
				continue;

			bool isBlendTree = MCNUtils::isNodeBlendTree(node);

			NodeCategory nodeCategory = kInvalid;

			if (isBlendTree)
				nodeCategory = kBlendTree;
			else if (nodeType == NODE_TYPE_NETWORK)
				nodeCategory = kNetwork;
			else if (nodeType == NODE_TYPE_STATE_MACHINE)
				nodeCategory = kStateMachine;
			else if ((nodeType == NODE_TYPE_TRANSIT) || (nodeType == NODE_TYPE_TRANSIT_SYNC_EVENTS))
				nodeCategory = kTransition;
			else
				nodeCategory = kNode;

			std::string nodeName = node->getName();
			int blendTreeID = this->getNumBTNodes();

			if (nodeName == "")
			{
				if (nodeCategory == kNetwork)
				{
					nodeName = "rootNetworkNode";

					blendTreeID = -1;
				}
				else if (nodeCategory == kStateMachine)
				{
					char name[256];
					sprintf_s(name, "StateMachine_%d", node->getNodeID());

					nodeName = name;

					blendTreeID = -1;
				}
				else if (nodeCategory == kBlendTree)
				{
					char name[256];
					sprintf_s(name, "BlendTree_%d", blendTreeID);

					nodeName = MCNUtils::getNodeName(node);

					if (nodeType == NODE_TYPE_ANIM_EVENTS)
						nodeName = MCNUtils::getAnimNodeName(node, animLibrary);

					NodeMap* btNode = new NodeMap(node, blendTreeID, kBlendTree, -1, name);
					this->m_nodesMap.push_back(btNode);

					nodeCategory = kNode;
				}
				else if (nodeCategory == kTransition)
				{
					char name[256];
					sprintf_s(name, "Transit_%d", node->getNodeID());

					nodeName = name;

					blendTreeID = -1;
				}
				else
				{
					nodeName = MCNUtils::getNodeName(node);

					if (nodeType == NODE_TYPE_ANIM_EVENTS)
						nodeName = MCNUtils::getAnimNodeName(node, animLibrary);

					blendTreeID = -1;
				}
			}

			NodeMap* nodeMap = new NodeMap(node, -1, nodeCategory, blendTreeID, nodeName);

			if (nodeCategory == kNetwork)
				nodeMap->setDbEntry(this->m_xmlElement->FirstChildElement("GraphEntry"));

			this->m_nodesMap.push_back(nodeMap);
		}
	}

	NodeMap* Network::getNodeMap(int nodeID)
	{
		for (size_t i = 0; i < this->m_nodesMap.size(); i++)
		{
			if (this->m_nodesMap[i]->getNodeID() == nodeID)
				return this->m_nodesMap[i];
		}

		return nullptr;
	}

	NodeMap* Network::getBTNodeMap(int btID)
	{
		for (size_t i = 0; i < this->m_nodesMap.size(); i++)
		{
			if (this->m_nodesMap[i]->getBlendTreeID() == btID)
				return this->m_nodesMap[i];
		}

		return nullptr;
	}

	void Network::createRootBlendTree()
	{
		tinyxml2::XMLElement* graphEntry = this->m_xmlElement->FirstChildElement("GraphEntry");

		tinyxml2::XMLElement* rootBt = MCNUtils::createBlendTree(graphEntry, "RootBlendTree");
		RXML::createFloatElement(rootBt, "ControlParamWidth", 155.f);
		RXML::createFloatElement(rootBt, "ControlParamHeight", 32.5f);
		RXML::createFloatElement(rootBt, "OutputPinWidth", 121.f);
		RXML::createFloatElement(rootBt, "OutputPinHeight", 45.f);
	}

	void Network::createAnimLibraryRef(ME::AnimationLibraryXML* animLibrary)
	{
		std::string animLibGuid = "";

		if (animLibrary != nullptr)
			animLibGuid = animLibrary->getGUID();

		MCNUtils::createStringElement(this->m_xmlElement, "AnimLibraryGUID", animLibGuid.c_str());
	}

	void Network::createPassDownPin()
	{
		tinyxml2::XMLElement* passDownNodePin = this->m_xmlElement->InsertNewChildElement("PassDownPin");
		passDownNodePin->SetAttribute("name", "Result");
		passDownNodePin->SetAttribute("type", "node");

		tinyxml2::XMLElement* reference = passDownNodePin->InsertNewChildElement("Reference");
		reference->SetAttribute("type", "bool");
		reference->SetText(1);
	}

	void Network::createControlParametersNode()
	{
		tinyxml2::XMLElement* cpNode = MCNUtils::createNodeElement(this->m_xmlElement, "ControlParametersNode", "ControlParameters");
		MCNUtils::createBlendTree(cpNode, "ControlParameterBlendTree");
	}

	Networks* Networks::loadFromFile(MorphemeDB* owner)
	{
		Networks* nets = new Networks;
		nets->m_owner = owner;
		nets->m_xmlElement = owner->m_xmlElement->FirstChildElement("Networks");

		if (nets->m_xmlElement == nullptr)
		{
			throw("Invalid mcn file");
			return nullptr;
		}

		for (tinyxml2::XMLElement* child = nets->m_xmlElement->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
			nets->m_network.push_back(Network::loadFromFile(nets, child->Attribute("name")));

		return nets;
	}

	Networks* Networks::create(MorphemeDB* owner, const char* name, ME::AnimationLibraryXML* animLibrary, ME::RigExportXML* rig, ME::NetworkDefExportXML* netDef, ME::MessagePresetLibraryExportXML* messagePresets, const char* animSetName)
	{
		Networks* nets = new Networks;
		nets->m_owner = owner;
		nets->m_xmlElement = MCNUtils::createNodeContainerElement(owner->m_xmlElement, "Networks");

		nets->addNetwork("Network", animLibrary, rig, netDef, messagePresets, animSetName);

		return nets;
	}

	Network* Networks::addNetwork(const char* name, ME::AnimationLibraryXML* animLibrary, ME::RigExportXML* rig, ME::NetworkDefExportXML* netDef, ME::MessagePresetLibraryExportXML* messagePresets, const char* animSetName)
	{
		Network* net = Network::create(this, name, animLibrary, rig, netDef, messagePresets, animSetName);

		this->m_network.push_back(net);

		return net;
	}

	MorphemeDB* MorphemeDB::loadFromFile(MCNFile* owner)
	{
		MorphemeDB* db = new MorphemeDB;
		db->m_owner = owner;
		db->m_xmlElement = owner->m_xmlElement->FirstChildElement("MorphemeDB");

		if (db->m_xmlElement == nullptr)
		{
			throw("Invalid mcn file");
			return nullptr;
		}

		db->m_networks = Networks::loadFromFile(db);

		return db;
	}

	MorphemeDB* MorphemeDB::create(MCNFile* owner, ME::NetworkDefExportXML* netDef, ME::RigExportXML* rig, ME::AnimationLibraryXML* animLibrary, ME::MessagePresetLibraryExportXML* messagePresets)
	{
		MorphemeDB* db = new MorphemeDB;
		db->m_owner = owner;
		db->m_xmlElement = MCNUtils::createNodeElement(owner->m_xmlElement, "MorphemeDB", "MorphemeDB");
		db->m_xmlElement->SetAttribute("namespace", "mcd");

		db->m_networks = Networks::create(db, animLibrary->getDestFilename(), animLibrary, rig, netDef, messagePresets, "Character");

		return db;
	}

	MCNFile* MCNFile::loadFile(std::string filename)
	{
		MCNFile* mcn = new MCNFile;

		try
		{
			tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument;
			doc->LoadFile(filename.c_str());

			mcn->m_fileName = filename;
			mcn->m_xmlDoc = doc;
			mcn->m_xmlElement = doc->FirstChildElement("NaturalMotion");

			if (mcn->m_xmlElement == nullptr)
				throw("Invalid mcn file %s", filename.c_str());

			mcn->m_morphemeDB = MorphemeDB::loadFromFile(mcn);
		}
		catch (const std::exception& e)
		{
			//g_appLog->alertMessage(MsgLevel_Error, "Invalid mcn file %s", filename.c_str());
			return nullptr;
		}

		return mcn;
	}

	MCNFile* MCNFile::createMcn(std::string filename, ME::NetworkDefExportXML* netDef, ME::RigExportXML* rig, ME::AnimationLibraryXML* animLibrary, ME::MessagePresetLibraryExportXML* messagePresets)
	{
		MCNFile* mcn = new MCNFile;

		mcn->m_xmlDoc = new tinyxml2::XMLDocument;
		tinyxml2::XMLDeclaration* decl = mcn->m_xmlDoc->NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");

		mcn->m_xmlElement = mcn->m_xmlDoc->NewElement("NaturalMotion");

		mcn->m_xmlElement->SetAttribute("typeString", "ConnectNetwork");
		mcn->m_xmlElement->SetAttribute("productVersion", "3.6.2");
		mcn->m_xmlElement->SetAttribute("dataVersion", 3506);
		mcn->m_xmlElement->SetAttribute("formatVersion", 5);

		mcn->m_xmlDoc->InsertEndChild(decl);
		mcn->m_xmlDoc->InsertEndChild(mcn->m_xmlElement);

		mcn->m_morphemeDB = MorphemeDB::create(mcn, netDef, rig, animLibrary, messagePresets);

		mcn->m_fileName = filename;

		return mcn;
	}

	void MCNFile::addAnimLocation(std::string sourceDir, std::string markupDir, bool includeSubDirs)
	{
		this->m_morphemeDB->m_networks->m_network[0]->addAnimLocation(sourceDir.c_str(), markupDir.c_str(), includeSubDirs);
	}

	void MCNFile::addControlParameter(ME::NodeExportXML* node)
	{
		this->m_morphemeDB->m_networks->m_network[0]->addControlParameter(node);
	}

	void MCNFile::addRequest(ME::MessageExportXML* request)
	{
		this->m_morphemeDB->m_networks->m_network[0]->addRequest(request);
	}

	void MCNFile::buildNodeMap(ME::NetworkDefExportXML* netDef, ME::AnimationLibraryXML* animLibrary)
	{
		this->m_morphemeDB->m_networks->m_network[0]->buildNodeMap(netDef, animLibrary);
	}

	void MCNFile::addNode(int nodeID)
	{
		this->m_morphemeDB->m_networks->m_network[0]->addNode(nodeID);
	}

	void MCNFile::setStateMachinesDefaultStates()
	{
		this->m_morphemeDB->m_networks->m_network[0]->setStateMachinesDefaultStates();
	}

	NodeMap* MCNFile::getNodeMap(int nodeId)
	{
		return this->m_morphemeDB->m_networks->m_network[0]->getNodeMap(nodeId);
	}

	bool MCNFile::save()
	{
		if (this->m_xmlDoc->SaveFile(this->m_fileName.c_str()) == tinyxml2::XML_SUCCESS)
			return true;

		return false;
	}

	void MCNFile::destroy()
	{
		this->m_xmlDoc->Clear();
		delete this;
	}
}