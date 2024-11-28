#include "AnimSet.h"
#include <vector>

namespace MEProject
{
	class MEProj : public RXML::XMLFileObj
	{
	public:
		MEProj() : RXML::XMLFileObj() 
		{
			this->m_rootElement->getXmlElement()->SetName("MeProject");

			this->setRootDir("");
			this->setAssetDir("");
			this->setMarkupDir("");
			this->setModel("");
			this->setNetwork("");
			this->setTimeAct("");
		}

		MEProj(const char* filename) : RXML::XMLFileObj(filename) 
		{
			this->m_rootElement->getXmlElement()->SetName("MeProject");

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
		void setGameDir(std::wstring gamePath);
		void addAnimSet(std::string name, std::string rig, std::string characterController);

		std::string getRootDir() const;
		std::string getModel() const;
		std::string getNetwork() const;
		std::string getTimeAct() const;
		std::string getAssetDir() const;
		std::string getMarkupDir() const;
		std::string getGameDir() const;
		std::string getAnimSetName(int animSetIdx) const;
		std::string getRig(int animSetIdx) const;
		std::string getCharacterController(int animSetIdx) const;

		bool load(const char* filename);
		void destroy();
	private:
		~MEProj() {}

		std::vector<AnimSet*> m_animSets;
	};
}