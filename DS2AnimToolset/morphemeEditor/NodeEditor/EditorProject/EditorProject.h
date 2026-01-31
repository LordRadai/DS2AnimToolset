#pragma once
#include "RCore.h"
#include "Node/ProjectNode.h"
#include "ControlParameter/ProjectControlParameter.h"

namespace NodeEditor
{
	namespace Project
	{
		class EditorProject
		{
			tinyxml2::XMLDocument* m_xmlDoc;
			tinyxml2::XMLElement* m_rootElement;

			ProjectNode* m_rootNode;
			std::vector<ProjectControlParameter*> m_controlParameters;
		public:
			EditorProject();
			~EditorProject();

			const std::string getProjectName() const;
			void setProjectName(const std::string& name);

			ProjectNode* getRootNode() const { return m_rootNode; }
			void setRootNode(ProjectNode* node);

			ProjectNode* createRootBlendTreeNode(const std::string& name, int nodeID);
			ProjectNode* createRootStateMachineNode(const std::string& name, int nodeID);

			ProjectControlParameter* getControlParameter(size_t index) const;
			ProjectControlParameter* getControlParameter(const std::string& name) const;
			size_t getNumControlParameters() const { return m_controlParameters.size(); }
			void addControlParameter(ProjectControlParameter* parameter);
			ProjectControlParameter* createControlParameter(const std::string& name, const std::string& type);

			bool saveProject(const std::string& filePath);
			bool loadProject(const std::string& filePath);

		private:
			ProjectNode* createRootNode(const std::string& name, int nodeID, int typeID);
		};
	}
}