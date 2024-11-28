#include "MEProj.h"

namespace MEProject
{
	void MEProj::setRootDir(std::string rootDir)
	{
		tinyxml2::XMLElement* rootDirElem = RXML::getUniqueElement(this->m_rootElement->getXmlElement(),"RootDir");

		rootDirElem->SetText(rootDir.c_str());
	}

	void MEProj::setModel(std::string modelPath)
	{
		tinyxml2::XMLElement* rootDirElem = RXML::getUniqueElement(this->m_rootElement->getXmlElement(), "Model");

		rootDirElem->SetText(modelPath.c_str());
	}

	void MEProj::setNetwork(std::string networkPath)
	{
		tinyxml2::XMLElement* rootDirElem = RXML::getUniqueElement(this->m_rootElement->getXmlElement(),"Network");

		rootDirElem->SetText(networkPath.c_str());
	}

	void MEProj::setTimeAct(std::string timeActPath)
	{
		tinyxml2::XMLElement* rootDirElem = RXML::getUniqueElement(this->m_rootElement->getXmlElement(),"TimeAct");

		rootDirElem->SetText(timeActPath.c_str());
	}

	void MEProj::setAssetDir(std::string assetPath)
	{
		tinyxml2::XMLElement* rootDirElem = RXML::getUniqueElement(this->m_rootElement->getXmlElement(), "AssetDir");

		rootDirElem->SetText(assetPath.c_str());
	}

	void MEProj::setMarkupDir(std::string markupDir)
	{
		tinyxml2::XMLElement* rootDirElem = RXML::getUniqueElement(this->m_rootElement->getXmlElement(), "MarkupDir");

		rootDirElem->SetText(markupDir.c_str());
	}

	void MEProj::setGameDir(std::wstring gamePath)
	{
		tinyxml2::XMLElement* rootDirElem = RXML::getUniqueElement(this->m_rootElement->getXmlElement(), "GameDir");

		rootDirElem->SetText(gamePath.c_str());
	}

	void MEProj::addAnimSet(std::string name, std::string rig, std::string characterController)
	{
		AnimSet* animSet = new AnimSet;

		animSet->setName(name);
		animSet->setRig(rig);
		animSet->setCharacterController(characterController);

		this->m_animSets.push_back(animSet);
	}

	std::string MEProj::getRootDir() const
	{
		return this->m_rootElement->getXmlElement()->FirstChildElement("RootDir")->GetText();
	}

	std::string MEProj::getModel() const
	{
		return this->m_rootElement->getXmlElement()->FirstChildElement("Model")->GetText();
	}

	std::string MEProj::getNetwork() const
	{
		return this->m_rootElement->getXmlElement()->FirstChildElement("Network")->GetText();
	}

	std::string MEProj::getTimeAct() const
	{
		return this->m_rootElement->getXmlElement()->FirstChildElement("TimeAct")->GetText();
	}

	std::string MEProj::getAssetDir() const
	{
		return this->m_rootElement->getXmlElement()->FirstChildElement("AssetDir")->GetText();
	}

	std::string MEProj::getMarkupDir() const
	{
		return this->m_rootElement->getXmlElement()->FirstChildElement("MarkupDir")->GetText();
	}

	std::string MEProj::getGameDir() const
	{
		return this->m_rootElement->getXmlElement()->FirstChildElement("MarkupDir")->GetText();
	}

	std::string MEProj::getAnimSetName(int animSetIdx) const
	{
		if (animSetIdx < this->m_animSets.size())
			return this->m_animSets[animSetIdx]->getName();

		return "";
	}

	std::string MEProj::getRig(int animSetIdx) const
	{
		if (animSetIdx < this->m_animSets.size())
			return this->m_animSets[animSetIdx]->getRig();

		return "";
	}

	std::string MEProj::getCharacterController(int animSetIdx) const
	{
		if (animSetIdx < this->m_animSets.size())
			return this->m_animSets[animSetIdx]->getCharacterController();

		return "";
	}

	bool MEProj::load(const char* filename)
	{
		bool status = RXML::XMLFileObj::load(filename);

		if (!status)
			return false;

		return true;
	}

	void MEProj::destroy()
	{
		for (size_t i = 0; i < this->m_animSets.size(); i++)
			this->m_animSets[i]->destroy();

		this->m_animSets.clear();

		RXML::XMLFileObj::destroy();
	}
}