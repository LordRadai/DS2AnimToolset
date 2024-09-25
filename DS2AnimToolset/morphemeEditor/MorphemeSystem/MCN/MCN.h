#pragma once
#include <vector>
#include "RCore.h"

namespace MCN
{
	class MCNFile;
	class MorphemeDB;
	class Networks;
	class Network;

	class AnimationSet
	{
	public:
		//static AnimationSet* create(Network* owner, const char* name);

	private:
		AnimationSet() {}
		~AnimationSet() {}

	protected:

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
		friend class MCNFile;
		friend class AnimLocation;
		friend class PreviewScript;
		friend class Networks;
	public:
		static Network* create(Networks* owner, const char* name, const char* animLibraryPath, const char* animSetName);

		void addAnimLocation(const char* sourceDir, const char* markupDir, bool includeSubDirectories);
		void addPreviewScript(const char* name, const char* filepath);
		void addAnimSet(const char* name);
		void addStartPoint(const char* name);
	private:
		Network() {}
		~Network() {}

		void createAnimLibraryRef(const char* animLibPath);
		void createPassDownPin();
		void createControlParametersNode();
		void createStartPointNode();
	protected:
		Networks* m_owner = nullptr;
		std::vector<AnimLocation*> m_animLocations;
		std::vector<PreviewScript*> m_scripts;

		tinyxml2::XMLElement* m_xmlElement = nullptr;
	};

	class Networks
	{
		friend class MCNFile;
		friend class Network;

	public:
		static Networks* create(MorphemeDB* owner, const char* animLibraryPath, const char* animSetName);

		Network* addNetwork(const char* name, const char* animLibraryPath, const char* animSetName);
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
		static MorphemeDB* create(MCNFile* owner);

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
		static MCNFile* createMcn(std::string filename);

		void addAnimLocation(std::string sourceDir, std::string markupDir, bool includeSubDirs);

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