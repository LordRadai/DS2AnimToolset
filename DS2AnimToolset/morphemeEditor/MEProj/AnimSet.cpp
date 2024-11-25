#include "AnimSet.h"

namespace MEProject
{
	void AnimSet::setName(std::string name)
	{
		this->m_xmlElement->SetAttribute("name", name.c_str());
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

	void AnimSet::destroy()
	{
		RXML::XMLElemObj::destroy();
	}
}