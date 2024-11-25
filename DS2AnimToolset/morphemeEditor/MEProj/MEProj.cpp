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

	void MEProject::addAnimSet(std::string name, std::string rig, std::string characterController)
	{
		AnimSet* animSet = new AnimSet;

		animSet->setName(name);
		animSet->setRig(rig);
		animSet->setCharacterController(characterController);

		this->m_animSets.push_back(animSet);
	}

	std::string MEProject::getRootDir()
	{
		return this->m_rootElement->getXmlElement()->FirstChildElement("RootDir")->GetText();
	}

	std::string MEProject::getModel()
	{
		return this->m_rootElement->getXmlElement()->FirstChildElement("Model")->GetText();
	}

	std::string MEProject::getNetwork()
	{
		return this->m_rootElement->getXmlElement()->FirstChildElement("Network")->GetText();
	}

	std::string MEProject::getTimeAct()
	{
		return this->m_rootElement->getXmlElement()->FirstChildElement("TimeAct")->GetText();
	}

	std::string MEProject::getAnimSetName(int animSetIdx)
	{
		if (animSetIdx < this->m_animSets.size())
			return this->m_animSets[animSetIdx]->getName();

		return "";
	}

	std::string MEProject::getRig(int animSetIdx)
	{
		if (animSetIdx < this->m_animSets.size())
			return this->m_animSets[animSetIdx]->getRig();

		return "";
	}

	std::string MEProject::getCharacterController(int animSetIdx)
	{
		if (animSetIdx < this->m_animSets.size())
			return this->m_animSets[animSetIdx]->getCharacterController();

		return "";
	}

	void MEProject::destroy()
	{
		for (size_t i = 0; i < this->m_animSets.size(); i++)
			this->m_animSets[i]->destroy();

		this->m_animSets.clear();

		RXML::XMLFileObj::destroy();
	}
}