#pragma once
#include "RCore.h"

namespace MEProject
{
	class MEProj;

	class AnimSet : public RXML::XMLElemObj
	{
		friend class MEProj;

	public:
		void setName(std::string name);
		void setRig(std::string rigPath);
		void setCharacterController(std::string ccPath);

		std::string getName();
		std::string getRig();
		std::string getCharacterController();

		void destroy();
	protected:
		AnimSet(tinyxml2::XMLElement* elem) : RXML::XMLElemObj(elem) {}
		~AnimSet() {}
	};
}