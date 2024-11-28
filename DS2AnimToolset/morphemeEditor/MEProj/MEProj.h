#include "AnimSet.h"
#include <vector>

namespace MEProject
{
	class MEProject : public RXML::XMLFileObj
	{
	public:
		MEProject() : RXML::XMLFileObj() 
		{
			this->setRootDir("");
			this->setAssetDir("");
			this->setMarkupDir("");
			this->setModel("");
			this->setNetwork("");
			this->setTimeAct("");
		}

		MEProject(const char* filename) : RXML::XMLFileObj(filename) 
		{
			this->setRootDir("");
			this->setAssetDir("");
			this->setMarkupDir("");
			this->setModel("");
			this->setNetwork("");
			this->setTimeAct("");
		}

		void setRootDir(std::string rootDir);
		void setModel(std::string modelPath);
		void setNetwork(std::string networkPath);
		void setTimeAct(std::string timeActPath);
		void setAssetDir(std::string assetPath);
		void setMarkupDir(std::string markupDir);
		void addAnimSet(std::string name, std::string rig, std::string characterController);

		std::string getRootDir();
		std::string getModel();
		std::string getNetwork();
		std::string getTimeAct();
		std::string getAssetDir();
		std::string getMarkupDir();
		std::string getAnimSetName(int animSetIdx);
		std::string getRig(int animSetIdx);
		std::string getCharacterController(int animSetIdx);

		bool load(const char* filename);
		void destroy();
	private:
		~MEProject() {}

		std::vector<AnimSet*> m_animSets;
	};
}