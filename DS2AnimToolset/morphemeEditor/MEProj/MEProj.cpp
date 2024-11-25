#include "MEProj.h"

namespace MEProject
{
	void MEProject::setRootDir(std::string rootDir)
	{
		tinyxml2::XMLElement* rootDirElem = RXML::getUniqueElement(this->m_rootElement->getXmlElement(),"RootDir");

		rootDirElem->SetText(rootDir.c_str());
	}

	void MEProject::setModel(std::string modelPath)
	{
		tinyxml2::XMLElement* rootDirElem = RXML::getUniqueElement(this->m_rootElement->getXmlElement(), "Model");

		rootDirElem->SetText(modelPath.c_str());
	}

	void MEProject::setNetwork(std::string networkPath)
	{
		tinyxml2::XMLElement* rootDirElem = RXML::getUniqueElement(this->m_rootElement->getXmlElement(),"Network");

		rootDirElem->SetText(networkPath.c_str());
	}

	void MEProject::setTimeAct(std::string timeActPath)
	{
		tinyxml2::XMLElement* rootDirElem = RXML::getUniqueElement(this->m_rootElement->getXmlElement(),"TimeAct");

		rootDirElem->SetText(timeActPath.c_str());
	}

	AnimSet* MEProject::addAnimSet(std::string name, std::string rig, std::string characterController)
	{
		AnimSet* animSet = new AnimSet;

		animSet->setName(name);
		animSet->setRig(rig);
		animSet->setCharacterController(characterController);

		this->m_animSets.push_back(animSet);

		return animSet;
	}

	void MEProject::destroy()
	{
		for (size_t i = 0; i < this->m_animSets.size(); i++)
			this->m_animSets[i]->destroy();

		RXML::XMLFileObj::destroy();
	}
}