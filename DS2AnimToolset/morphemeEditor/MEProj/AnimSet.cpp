#include "AnimSet.h"

namespace MEProject
{
	void AnimSet::setName(std::string name)
	{
		this->m_xmlElement->SetAttribute("Name", name.c_str());
	}

	void AnimSet::setRig(std::string rigPath)
	{
		tinyxml2::XMLElement* rootDirElem = RXML::getUniqueElement(this->m_xmlElement, "Rig");

		rootDirElem->SetText(rigPath.c_str());
	}

	void AnimSet::setCharacterController(std::string ccPath)
	{
		tinyxml2::XMLElement* rootDirElem = RXML::getUniqueElement(this->m_xmlElement, "CharacterController");

		rootDirElem->SetText(ccPath.c_str());
	}

	std::string AnimSet::getName()
	{
		return this->m_xmlElement->FindAttribute("Name")->Value();
	}

	std::string AnimSet::getRig()
	{
		return this->m_xmlElement->FirstChildElement("Rig")->GetText();

	}

	std::string AnimSet::getCharacterController()
	{
		return this->m_xmlElement->FirstChildElement("CharacterController")->GetText();
	}

	void AnimSet::destroy()
	{
		RXML::XMLElemObj::destroy();
	}
}