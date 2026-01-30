#pragma once
#include <vector>
#include "NodeEditor/EditorProject/Entity/Entity.h"
#include "NodeEditor/EditorProject/Attribute/Attribute.h"
#include "NodeEditor/EditorProject/ControlParameter/ControlParameter.h"

namespace NodeEditor
{
	namespace Project
	{
		class Node : public Entity
		{
			std::vector<Attribute*> m_attributes;
			std::vector<Node*> m_inputNodes;
			std::vector<Node*> m_childrenNodes;
			std::vector<std::string> m_inputControlParameters;
		public:
			Node(tinyxml2::XMLElement* xmlElement) : Entity(xmlElement, "Node") {}
			virtual ~Node() override {}
			virtual bool loadFromXMLElement(tinyxml2::XMLElement* xmlElement) override;

			int getNodeID() const;
			void setNodeID(int id);

			int getNodeTypeID();
			void setNodeTypeID(int typeID);

			int getParentNodeID() const;
			void setParentNodeID(int id);

			const std::string getContainerType() const;
			void setContainerType(const std::string& type);

			Attribute* getAttribute(size_t index) const;
			Attribute* getAttribute(const std::string& name) const;
			size_t getNumAttributes() const { return m_attributes.size(); }
			void addAttribute(Attribute* attribute);
			Attribute* createAttribute(const std::string& name);

			Node* getInputNode(size_t index) const;
			size_t getNumInputNodes() const { return m_inputNodes.size(); }
			void addInputNode(Node* node);
			Node* createInputNode(const std::string& name, int nodeID, int typeID);
			Node* createInputBlendTreeNode(const std::string& name, int nodeID);
			Node* createInputStateMachineNode(const std::string& name, int nodeID);

			Node* getChildNode(size_t index) const;
			size_t getNumChildNodes() const { return m_childrenNodes.size(); }
			void addChildNode(Node* node);
			Node* createChildNode(const std::string& name, int nodeID, int typeID);
			Node* createChildBlendTreeNode(const std::string& name, int nodeID);
			Node* createChildStateMachineNode(const std::string& name, int nodeID);

			const std::string getInputControlParameter(size_t index) const;
			size_t getNumInputControlParameters() const { return m_inputControlParameters.size(); }
			void addInputControlParameter(ControlParameter* cp);
		};
	}
}
