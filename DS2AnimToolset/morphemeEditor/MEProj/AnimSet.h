#include "RCore.h"

namespace MEProject
{
	class AnimSet : public RXML::XMLElemObj
	{
	public:
		AnimSet() {}
		~AnimSet() {}

		void setName(std::string name);
		void setRig(std::string rigPath);
		void setCharacterController(std::string ccPath);

		void destroy();
	};
}