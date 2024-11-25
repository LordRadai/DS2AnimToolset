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
		void addAnimSet(std::string name, std::string rig, std::string characterController);

		std::string getRootDir();
		std::string getModel();
		std::string getNetwork();
		std::string getTimeAct();
		std::string getAnimSetName(int animSetIdx);
		std::string getRig(int animSetIdx);
		std::string getCharacterController(int animSetIdx);

		void destroy();
	private:

		std::vector<AnimSet*> m_animSets;
	};
}