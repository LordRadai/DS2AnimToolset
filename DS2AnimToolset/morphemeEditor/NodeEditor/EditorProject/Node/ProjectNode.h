#pragma once
#include <vector>
#include "NodeEditor/EditorProject/Entity/ProjectEntity.h"
#include "NodeEditor/EditorProject/Attribute/ProjectAttribute.h"
#include "NodeEditor/EditorProject/ControlParameter/ProjectControlParameter.h"
#include "NodeEditor/EditorProject/Transition/ProjectTransition.h"

namespace NodeEditor
{
	namespace Project
	{
		class ProjectNode : public ProjectEntity
		{
			std::vector<ProjectAttribute*> m_attributes;
			std::vector<ProjectNode*> m_inputNodes;
			std::vector<ProjectNode*> m_childrenNodes;
			std::vector<ProjectTransition*> m_transitions;
			std::vector<std::string> m_inputControlParameters;
		public:
			ProjectNode(tinyxml2::XMLElement* xmlElement) : ProjectEntity(xmlElement, "Node") {}
			virtual ~ProjectNode() override {}
			virtual bool loadFromXMLElement(tinyxml2::XMLElement* xmlElement) override;

			int getNodeID() const;
			void setNodeID(int id);

			const std::string getNodeTypeName() const;
			void setNodeTypeName(const std::string& typeName);

			int getParentNodeID() const;
			void setParentNodeID(int id);

			bool isNodeContainer() const { return isNodeStateMachine() || isNodeBlendTree(); }
			bool isNodeStateMachine() const { return getNodeTypeName() == "StateMachine"; }
			bool isNodeBlendTree() const { return getNodeTypeName() == "BlendTree"; }

			ProjectAttribute* getAttribute(size_t index) const;
			ProjectAttribute* getAttribute(const std::string& name) const;
			size_t getNumAttributes() const { return m_attributes.size(); }
			void addAttribute(ProjectAttribute* attribute);
			ProjectAttribute* createAttribute(const std::string& name);

			ProjectNode* getInputNodeAtIndex(size_t index) const;
			ProjectNode* getInputNode(int nodeID) const;
			size_t getNumInputNodes() const { return m_inputNodes.size(); }
			void addInputNode(ProjectNode* node);
			ProjectNode* createInputNode(const std::string& name, int nodeID, const std::string& typeName);
			ProjectNode* createInputBlendTreeNode(const std::string& name, int nodeID);
			ProjectNode* createInputStateMachineNode(const std::string& name, int nodeID);

			ProjectNode* getChildNodeAtIndex(size_t index) const;
			ProjectNode* getChildNode(int nodeID) const;
			size_t getNumChildNodes() const { return m_childrenNodes.size(); }
			void addChildNode(ProjectNode* node);
			ProjectNode* createChildNode(const std::string& name, int nodeID, const std::string& typeName);
			ProjectNode* createChildBlendTreeNode(const std::string& name, int nodeID);
			ProjectNode* createChildStateMachineNode(const std::string& name, int nodeID);

			ProjectTransition* getTransition(size_t index) const;
			size_t getNumTransitions() const { return m_transitions.size(); }
			void addTransition(ProjectTransition* transition);
			ProjectTransition* createTransition(int sourceNodeID, int destinationNodeID);

			const std::string getInputControlParameter(size_t index) const;
			size_t getNumInputControlParameters() const { return m_inputControlParameters.size(); }
			void addInputControlParameter(ProjectControlParameter* cp);
		};
	}
}
