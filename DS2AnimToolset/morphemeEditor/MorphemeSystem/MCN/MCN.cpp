#include "MCN.h"
#include "MCNUtils.h"

namespace MCN
{
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

	Network* Network::create(Networks* owner, const char* name, const char* animLibraryPath, const char* animSetName)
	{
		Network* net = new Network;
		net->m_owner = owner;
		net->m_xmlElement = MCNUtils::createNodeElement(owner->m_xmlElement, "Network", "Network");

		tinyxml2::XMLElement* graphEntry = MCNUtils::createNodeContainerElement(net->m_xmlElement, "GraphEntry");
		tinyxml2::XMLElement* animLocs = MCNUtils::createNodeContainerElement(net->m_xmlElement, "AnimationLocations");
		tinyxml2::XMLElement* scripts = MCNUtils::createNodeContainerElement(net->m_xmlElement, "PreviewScripts");

		net->addPreviewScript("Preview script", "Data\\res\\DefaultPreviewScript.lua");

		GUID guid;
		CoCreateGuid(&guid);

		MCNUtils::createStringElement(net->m_xmlElement, "GUID", RString::guidToString(guid).c_str());

		net->createAnimLibraryRef(animLibraryPath);

		MCNUtils::createStringElement(net->m_xmlElement, "AssetManagerSelectedSet", animSetName);

		net->createPassDownPin();
		net->createControlParametersNode();

		MCNUtils::createNodeElement(net->m_xmlElement, "EmittedControlParametersNode", "EmittedControlParameters");
		MCNUtils::createNodeElement(net->m_xmlElement, "AnimationSetsNode", "AnimationSets");
		MCNUtils::createNodeElement(net->m_xmlElement, "SceneGraphRootNode", "SceneGraphRoot");
		MCNUtils::createNodeElement(net->m_xmlElement, "LayersNode", "Layers");
		MCNUtils::createNodeElement(net->m_xmlElement, "BodyGroupsNode", "BodyGroups");
		MCNUtils::createNodeElement(net->m_xmlElement, "RequestsNode", "Requests");
		MCNUtils::createNodeElement(net->m_xmlElement, "RequestPresetsNode", "RequestPresets");
		MCNUtils::createNodeElement(net->m_xmlElement, "MessagePresetGroupsNode", "MessagePresetGroups");
		MCNUtils::createNodeElement(net->m_xmlElement, "CharacterStartPointsNode", "CharacterStartPoints");

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

		std::string dbPointer = MCNUtils::getMorphemeDBPointer(targetAnimSet);
	}

	void Network::createAnimLibraryRef(const char* animLibPath)
	{
		tinyxml2::XMLDocument animLibraryDoc;
		std::string animLibGuid = "";

		if (animLibraryDoc.LoadFile(animLibPath) == tinyxml2::XMLError::XML_SUCCESS)
			animLibGuid = animLibraryDoc.FirstChildElement("AnimationLibrary")->Attribute("guid");

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

	Networks* Networks::create(MorphemeDB* owner, const char* animLibraryPath, const char* animSetName)
	{
		Networks* nets = new Networks;
		nets->m_owner = owner;
		nets->m_xmlElement = MCNUtils::createNodeContainerElement(owner->m_xmlElement, "Networks");

		nets->addNetwork("Network", animLibraryPath, animSetName);

		return nets;
	}

	Network* Networks::addNetwork(const char* name, const char* animLibraryPath, const char* animSetName)
	{
		Network* net = Network::create(this, name, animLibraryPath, animSetName);

		this->m_network.push_back(net);

		return net;
	}

	MorphemeDB* MorphemeDB::create(MCNFile* owner)
	{
		MorphemeDB* db = new MorphemeDB;
		db->m_owner = owner;
		db->m_xmlElement = MCNUtils::createNodeElement(owner->m_xmlElement, "MorphemeDB", "MorphemeDB");
		db->m_xmlElement->SetAttribute("namespace", "mcd");

		db->m_networks = Networks::create(db, "", "");

		return db;
	}

	MCNFile* MCNFile::createMcn(std::string filename)
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

		mcn->m_morphemeDB = MorphemeDB::create(mcn);

		mcn->m_fileName = filename;

		return mcn;
	}

	void MCNFile::addAnimLocation(std::string sourceDir, std::string markupDir, bool includeSubDirs)
	{
		this->m_morphemeDB->m_networks->m_network[0]->addAnimLocation(sourceDir.c_str(), markupDir.c_str(), includeSubDirs);
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