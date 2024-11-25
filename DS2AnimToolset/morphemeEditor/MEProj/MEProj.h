#include "AnimSet.h"
#include <vector>

namespace MEProject
{
	class MEProject : public RXML::XMLFileObj
	{
	public:
		MEProject() {}
		~MEProject() {}

		void setRootDir(std::string rootDir);
		void setModel(std::string modelPath);
		void setNetwork(std::string networkPath);
		void setTimeAct(std::string timeActPath);
		AnimSet* addAnimSet(std::string name, std::string rig, std::string characterController);

		void destroy();
	private:

		std::vector<AnimSet*> m_animSets;
	};
}