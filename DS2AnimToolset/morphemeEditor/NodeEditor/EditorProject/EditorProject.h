#pragma once
#include "RCore.h"
#include "Node/Node.h"
#include "ControlParameter/ControlParameter.h"

namespace NodeEditor
{
	namespace Project
	{
		class EditorProject
		{
			tinyxml2::XMLDocument* m_xmlDoc;
			tinyxml2::XMLElement* m_rootElement;

			Node* m_rootNode;
			std::vector<ControlParameter*> m_controlParameters;
		public:
			EditorProject();
			~EditorProject();

			const std::string getProjectName() const;
			void setProjectName(const std::string& name);

			Node* getRootNode() const { return m_rootNode; }
			void setRootNode(Node* node);

			Node* createRootBlendTreeNode(const std::string& name, int nodeID);
			Node* createRootStateMachineNode(const std::string& name, int nodeID);

			ControlParameter* getControlParameter(size_t index) const;
			ControlParameter* getControlParameter(const std::string& name) const;
			size_t getNumControlParameters() const { return m_controlParameters.size(); }
			void addControlParameter(ControlParameter* parameter);
			ControlParameter* createControlParameter(const std::string& name, const std::string& type);

			bool saveProject(const std::string& filePath);
			bool loadProject(const std::string& filePath);

		private:
			Node* createRootNode(const std::string& name, int nodeID, int typeID);
		};
	}
}