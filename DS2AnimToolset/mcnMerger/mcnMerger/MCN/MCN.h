#pragma once
#include <vector>
#include "NodeMap.h"

namespace MCN
{
	class Node;
	class MCNFile;
	class MorphemeDB;
	class Networks;
	class Network;

	class TransitionEdge
	{
	public:
		static TransitionEdge* create(Node* owner, ME::NodeExportXML* node);

	private:
		TransitionEdge() {}
		~TransitionEdge() {}

	protected:
		Node* m_owner = nullptr;

		tinyxml2::XMLElement* m_xmlElement = nullptr;
	};

	class Node
	{
	public:
		static Node* create(Network* owner, ME::NodeExportXML* node);

	private:
		Node() {}
		~Node() {}

		static Node* createBlendTree(Network* owner, ME::NodeExportXML* node);
		static Node* createStateMachine(Network* owner, ME::NodeExportXML* node);
	protected:
		Network* m_owner = nullptr;

		tinyxml2::XMLElement* m_xmlElement = nullptr;
	};

	class ControlParameter
	{
	public:
		static ControlParameter* create(Network* owner, ME::NodeExportXML* cpNode);

	private:
		ControlParameter() {}
		~ControlParameter() {}

		static ControlParameter* createBoolCp(tinyxml2::XMLElement* parent, ME::NodeExportXML* cpNode);
		static ControlParameter* createFloatCp(tinyxml2::XMLElement* parent, ME::NodeExportXML* cpNode);
		static ControlParameter* createIntCp(tinyxml2::XMLElement* parent, ME::NodeExportXML* cpNode);
		static ControlParameter* createUIntCp(tinyxml2::XMLElement* parent, ME::NodeExportXML* cpNode);
		static ControlParameter* createVector3Cp(tinyxml2::XMLElement* parent, ME::NodeExportXML* cpNode);
		static ControlParameter* createVector4Cp(tinyxml2::XMLElement* parent, ME::NodeExportXML* cpNode);
	protected:
		Network* m_owner = nullptr;

		tinyxml2::XMLElement* m_xmlElement = nullptr;
	};

	class AnimationSet
	{
	public:
		static AnimationSet* create(Network* owner, const char* name, ME::RigExportXML* rig);

	private:
		AnimationSet() {}
		~AnimationSet() {}

		void setRigChannels(ME::RigExportXML* rig);
		tinyxml2::XMLElement* addSkin(const char* name, const char* filename);
	protected:
		Network* m_owner = nullptr;

		tinyxml2::XMLElement* m_xmlElement = nullptr;
	};

	class AnimLocation
	{
		friend class MCNFile;
	public:
		static AnimLocation* create(Network* owner, const char* sourceDir, const char* markupDir, bool includeSubDirs);

		void setSourceDirectory(const char* sourceDir);
		void setMarkupDirectory(const char* markupDir);
		void setIncludeSubDirs(bool include);

	private:
		AnimLocation() {}
		~AnimLocation() {}

	protected:
		Network* m_owner = nullptr;

		tinyxml2::XMLElement* m_xmlElement = nullptr;
	};

	class PreviewScript
	{
	public:
		static PreviewScript* loadFromFile(Network* owner, const char* name);
		static PreviewScript* create(Network* owner, const char* name, const char* filename);

		void setScriptData(const char* filename);
	private:
		PreviewScript() {}
		~PreviewScript() {}

	protected:
		Network* m_owner = nullptr;

		tinyxml2::XMLElement* m_xmlElement = nullptr;
	};

	class Network
	{
		friend class Node;
		friend class ControlParameter;
		friend class AnimationSet;
		friend class MCNFile;
		friend class AnimLocation;
		friend class PreviewScript;
		friend class Networks;
	public:
		static Network* loadFromFile(Networks* owner, const char* name);
		static Network* create(Networks* owner, const char* name, ME::AnimationLibraryXML* animLibrary, ME::RigExportXML* rig, ME::NetworkDefExportXML* netDef, ME::MessagePresetLibraryExportXML* messagePresets, const char* animSetName);

		void addAnimLocation(const char* sourceDir, const char* markupDir, bool includeSubDirectories);
		void addPreviewScript(const char* name, const char* filepath);
		void addAnimSet(const char* name, ME::RigExportXML* rig);
		void addStartPoint(const char* name);
		void addRequest(ME::MessageExportXML* request);
		void addControlParameter(ME::NodeExportXML* cp);
		void addNode(int nodeID);
		void setStateMachinesDefaultStates();
		void buildNodeMap(ME::NetworkDefExportXML* netDef, ME::AnimationLibraryXML* animLibrary);

		NodeMap* getNodeMap(int nodeID);

	private:
		Network() {}
		~Network() {}

		void createRootBlendTree();
		void createAnimLibraryRef(ME::AnimationLibraryXML* animLibPath);
		void createPassDownPin();
		void createControlParametersNode();

		NodeMap* getParentNodeContainer(int nodeID);
		void writeNode(NodeMap* nodeMap, NodeMap* parentNodeMap);

		void writeNonContainerNode(NodeMap* nodeMap, tinyxml2::XMLElement* parent);
		void writeStateMachineNode(NodeMap* nodeMap, tinyxml2::XMLElement* parent);
		void writeBlendTreeNode(NodeMap* nodeMap, tinyxml2::XMLElement* parent);
		void writeTransitionNode(NodeMap* nodeMap, tinyxml2::XMLElement* parent);
	protected:
		Networks* m_owner = nullptr;
		std::vector<AnimLocation*> m_animLocations;
		std::vector<PreviewScript*> m_scripts;
		std::vector<AnimationSet*> m_animSets;
		std::vector<ControlParameter*> m_controlParameters;
		std::vector<Node*> m_nodes;
		std::vector<NodeMap*> m_nodesMap;

		tinyxml2::XMLElement* m_xmlElement = nullptr;
	};

	class Networks
	{
		friend class MCNFile;
		friend class Network;

	public:
		static Networks* loadFromFile(MorphemeDB* owner);
		static Networks* create(MorphemeDB* owner, const char* name, ME::AnimationLibraryXML* animLibrary, ME::RigExportXML* rig, ME::NetworkDefExportXML* netDef, ME::MessagePresetLibraryExportXML* messagePresets, const char* animSetName);

		Network* addNetwork(const char* name, ME::AnimationLibraryXML* animLibrary, ME::RigExportXML* rig, ME::NetworkDefExportXML* netDef, ME::MessagePresetLibraryExportXML* messagePresets, const char* animSetName);
	private:
		Networks() {}
		~Networks() {}

	protected:
		MorphemeDB* m_owner = nullptr;
		std::vector<Network*> m_network;

		tinyxml2::XMLElement* m_xmlElement = nullptr;
	};

	class MorphemeDB
	{
		friend class MCNFile;
		friend class Networks;
	public:
		static MorphemeDB* loadFromFile(MCNFile* owner);
		static MorphemeDB* create(MCNFile* owner, ME::NetworkDefExportXML* netDef, ME::RigExportXML* rig, ME::AnimationLibraryXML* animLibrary, ME::MessagePresetLibraryExportXML* messagePresets);

	private:
		MorphemeDB() {}
		~MorphemeDB() {}

	protected:
		MCNFile* m_owner = nullptr;
		Networks* m_networks = nullptr;

		tinyxml2::XMLElement* m_xmlElement = nullptr;
	};

	class MCNFile
	{
		friend class MorphemeDB;

	public:
		static MCNFile* loadFile(std::string filename);
		static MCNFile* createMcn(std::string filename, ME::NetworkDefExportXML* netDef, ME::RigExportXML* rig, ME::AnimationLibraryXML* animLibrary, ME::MessagePresetLibraryExportXML* messagePresets);

		void setDestFileName(std::string filename) { this->m_fileName = filename; }

		void addAnimLocation(std::string sourceDir, std::string markupDir, bool includeSubDirs);
		void addControlParameter(ME::NodeExportXML* node);
		void addRequest(ME::MessageExportXML* request);
		void buildNodeMap(ME::NetworkDefExportXML* netDef, ME::AnimationLibraryXML* animLibrary);
		void addNode(int nodeID);
		void setStateMachinesDefaultStates();

		NodeMap* getNodeMap(int nodeId);

		bool save();
		void destroy();
	private:
		MCNFile() {}
		~MCNFile() {}

	protected:
		std::string m_fileName = "";
		MorphemeDB* m_morphemeDB = nullptr;

		tinyxml2::XMLDocument* m_xmlDoc = nullptr;
		tinyxml2::XMLElement* m_xmlElement = nullptr;
	};
}