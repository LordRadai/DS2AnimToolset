#include "ProjectNode.h"
#include "NodeEditor/EditorProject/EditorProject.h"

namespace NodeEditor
{
	namespace Project
	{
		ProjectNode::ProjectNode(EditorProject* project, ProjectNode* parentContainer, tinyxml2::XMLElement* xmlElement) : ProjectEntity(project, xmlElement, "Node"),
			m_parentNodeContainer(parentContainer)
		{
		}

		bool ProjectNode::loadFromXMLElement(tinyxml2::XMLElement* xmlElement)
		{
			if (xmlElement)
				m_xmlElement = xmlElement;

			tinyxml2::XMLElement* attribList = m_xmlElement->FirstChildElement("Attributes");

			if (attribList)
			{
				tinyxml2::XMLElement* attribElem = attribList->FirstChildElement("Attribute");

				while (attribElem)
				{
					ProjectAttribute* attribute = new ProjectAttribute(m_project, this, attribElem);

					attribute->loadFromXMLElement(attribElem);
					m_attributes.push_back(attribute);

					attribElem = attribElem->NextSiblingElement("Attribute");
				}
			}

			tinyxml2::XMLElement* inputNodeList = m_xmlElement->FirstChildElement("InputNodes");

			if (inputNodeList)
			{
				tinyxml2::XMLElement* inputNodeElem = inputNodeList->FirstChildElement("Node");

				while (inputNodeElem)
				{
					ProjectNode* node = new ProjectNode(m_project, m_parentNodeContainer, inputNodeElem);

					node->loadFromXMLElement(inputNodeElem);
					m_inputNodes.push_back(node);

					inputNodeElem = inputNodeElem->NextSiblingElement("Node");
				}
			}

			tinyxml2::XMLElement* inputCPList = m_xmlElement->FirstChildElement("InputCPConnections");

			if (inputCPList)
			{
				tinyxml2::XMLElement* inputCPElem = inputCPList->FirstChildElement("CPConnection");

				while (inputCPElem)
				{
					if (inputCPElem->GetText())
						m_inputCPConnections.push_back(new InputCPConnection(inputCPElem->IntAttribute("nodeID"), inputCPElem->IntAttribute("pinIndex")));

					inputCPElem = inputCPElem->NextSiblingElement("CPConnection");
				}
			}

			return false;
		}

		int ProjectNode::getNodeID() const
		{
			if (m_xmlElement)
				return m_xmlElement->IntAttribute("nodeID");

			throw std::runtime_error("XML Element is null, cannot get Node ID.");
		}

		void ProjectNode::setNodeID(int id)
		{
			if (m_xmlElement)
				m_xmlElement->SetAttribute("nodeID", id);

			throw std::runtime_error("XML Element is null, cannot set Node ID.");
		}

		int ProjectNode::getNumOutputCPPins() const
		{
			if (m_xmlElement)
				return m_xmlElement->IntAttribute("numOutputCPPins");

			throw std::runtime_error("XML Element is null, cannot get Number of Output CP Pins.");
		}

		void ProjectNode::setNumOutputCPPins(int numPins)
		{
			if (m_xmlElement)
				m_xmlElement->SetAttribute("numOutputCPPins", numPins);
		}

		const std::string ProjectNode::getNodeTypeName() const
		{
			if (m_xmlElement)
				return m_xmlElement->Attribute("nodeType");
		}

		void ProjectNode::setNodeTypeName(const std::string& typeName)
		{
			if (m_xmlElement)
				m_xmlElement->SetAttribute("nodeType", typeName.c_str());
		}

		int ProjectNode::getParentNodeID() const
		{
			if (m_xmlElement)
				return m_xmlElement->IntAttribute("parentNodeID");

			throw std::runtime_error("XML Element is null, cannot get Parent Node ID.");
		}

		void ProjectNode::setParentNodeID(int id)
		{
			if (m_xmlElement)
				m_xmlElement->SetAttribute("parentNodeID", id);
		}

		ProjectAttribute* ProjectNode::getAttribute(size_t index) const
		{
			if (index < m_attributes.size())
				return m_attributes[index];

			return nullptr;
		}

		ProjectAttribute* ProjectNode::getAttribute(const std::string& name) const
		{
			for (ProjectAttribute* attribute : m_attributes)
			{
				if (attribute->getName() == name)
					return attribute;
			}

			return nullptr;
		}

		void ProjectNode::addAttribute(ProjectAttribute* attribute)
		{
			m_attributes.push_back(attribute);

			tinyxml2::XMLElement* attribList = m_xmlElement->FirstChildElement("Attributes");

			if (!attribList)
				attribList = m_xmlElement->InsertNewChildElement("Attributes");

			attribList->InsertEndChild(attribute->getXMLElement());
		}

		ProjectAttribute* ProjectNode::createAttribute(const std::string& name)
		{
			tinyxml2::XMLElement* attribList = m_xmlElement->FirstChildElement("Attributes");

			ProjectAttribute* attribute = new ProjectAttribute(m_project, this, attribList);
			attribute->setName(name);
			addAttribute(attribute);

			return attribute;
		}

		ProjectNode* ProjectNode::getInputNodeAtIndex(size_t index) const
		{
			if (index < m_inputNodes.size())
				return m_inputNodes[index];

			return nullptr;
		}

		ProjectNode* ProjectNode::getInputNode(int nodeID) const
		{
			for (ProjectNode* inputNode : m_inputNodes)
			{
				if (inputNode->getNodeID() == nodeID)
					return inputNode;
			}
			return nullptr;
		}

		void ProjectNode::addInputNode(ProjectNode* node)
		{
			m_inputNodes.push_back(node);

			tinyxml2::XMLElement* inputNodeList = m_xmlElement->FirstChildElement("InputNodes");

			if (!inputNodeList)
				inputNodeList = m_xmlElement->InsertNewChildElement("InputNodes");

			inputNodeList->InsertEndChild(node->getXMLElement());
		}

		ProjectNode* ProjectNode::createInputNode(const std::string& name, int nodeID, const std::string& typeName)
		{
			tinyxml2::XMLElement* inputNodeList = m_xmlElement->FirstChildElement("InputNodes");

			if (!inputNodeList)
				inputNodeList = m_xmlElement->InsertNewChildElement("InputNodes");

			ProjectNode* node = new ProjectNode(m_project, m_parentNodeContainer, inputNodeList);
			node->setName(name);
			node->setNodeID(nodeID);
			node->setNodeTypeName(typeName);

			addInputNode(node);
			return node;
		}

		ProjectNode* ProjectNode::createInputBlendTreeNode(const std::string& name, int nodeID)
		{
			return createInputNode(name, nodeID, "BlendTree");
		}

		ProjectNode* ProjectNode::createInputStateMachineNode(const std::string& name, int nodeID)
		{
			return createInputNode(name, nodeID, "StateMachine");
		}

		ProjectNode* ProjectNode::getChildNodeAtIndex(size_t index) const
		{
			if (index < m_childrenNodes.size())
				return m_childrenNodes[index];

			return nullptr;
		}

		ProjectNode* ProjectNode::getChildNode(int nodeID) const
		{
			for (ProjectNode* childNode : m_childrenNodes)
			{
				if (childNode->getNodeID() == nodeID)
					return childNode;
			}

			return nullptr;
		}

		void ProjectNode::addChildNode(ProjectNode* node)
		{
			m_childrenNodes.push_back(node);
			tinyxml2::XMLElement* childNodeList = m_xmlElement->FirstChildElement("ChildNodes");

			if (!childNodeList)
				childNodeList = m_xmlElement->InsertNewChildElement("ChildNodes");

			childNodeList->InsertEndChild(node->getXMLElement());
		}

		ProjectNode* ProjectNode::createChildNode(const std::string& name, int nodeID, const std::string& typeName)
		{
			tinyxml2::XMLElement* childNodeList = m_xmlElement->FirstChildElement("ChildNodes");

			if (!childNodeList)
				childNodeList = m_xmlElement->InsertNewChildElement("ChildNodes");

			ProjectNode* node = new ProjectNode(m_project, m_parentNodeContainer, childNodeList);
			node->setName(name);
			node->setNodeID(nodeID);
			node->setNodeTypeName(typeName);

			addChildNode(node);

			return node;
		}

		ProjectNode* ProjectNode::createChildBlendTreeNode(const std::string& name, int nodeID)
		{
			return createChildNode(name, nodeID, "BlendTree");
		}

		ProjectNode* ProjectNode::createChildStateMachineNode(const std::string& name, int nodeID)
		{
			return createChildNode(name, nodeID, "StateMachine");
		}

		ProjectTransition* ProjectNode::getTransition(size_t index) const
		{
			if (index < m_transitions.size())
				return m_transitions[index];

			return nullptr;
		}

		void ProjectNode::addTransition(ProjectTransition* transition)
		{
			m_transitions.push_back(transition);
			tinyxml2::XMLElement* transitionList = m_xmlElement->FirstChildElement("Transitions");

			if (!transitionList)
				transitionList = m_xmlElement->InsertNewChildElement("Transitions");

			transitionList->InsertEndChild(transition->getXMLElement());
		}

		ProjectTransition* ProjectNode::createTransition(int sourceNodeID, int destinationNodeID)
		{
			if (!isNodeStateMachine())
				throw std::runtime_error("Transitions can only be created for State Machine nodes.");

			if (!getChildNode(sourceNodeID) || !getChildNode(destinationNodeID))
				throw std::runtime_error("Source or Destination Node ID does not exist among child nodes.");

			tinyxml2::XMLElement* transitionList = m_xmlElement->FirstChildElement("Transitions");

			if (!transitionList)
				transitionList = m_xmlElement->InsertNewChildElement("Transitions");

			ProjectTransition* transition = new ProjectTransition(m_project, transitionList);
			transition->setSourceNodeID(sourceNodeID);
			transition->setDestinationNodeID(destinationNodeID);
			addTransition(transition);
			return transition;
		}

		ProjectNode::InputCPConnection* ProjectNode::getInputCPConnection(size_t index) const
		{
			if (index < m_inputCPConnections.size())
				return m_inputCPConnections[index];

			return nullptr;
		}

		void ProjectNode::addInputCPConnection(ProjectControlParameter* cp)
		{
			m_inputCPConnections.push_back(new InputCPConnection(cp->getControlParameterID(), 0));
			tinyxml2::XMLElement* inputCPList = m_xmlElement->FirstChildElement("InputCPConnections");

			if (!inputCPList)
				inputCPList = m_xmlElement->InsertNewChildElement("InputCPConnections");

			tinyxml2::XMLElement* cpElem = inputCPList->InsertNewChildElement("CPConnection");
			cpElem->SetAttribute("nodeID", cp->getControlParameterID());
			cpElem->SetAttribute("pinIndex", 0);
		}

		void ProjectNode::addInputCPConnection(ProjectNode* node, int pinIndex)
		{
			m_inputCPConnections.push_back(new InputCPConnection(node->getNodeID(), pinIndex));
			tinyxml2::XMLElement* inputCPList = m_xmlElement->FirstChildElement("InputCPConnections");

			if (!inputCPList)
				inputCPList = m_xmlElement->InsertNewChildElement("InputCPConnections");

			tinyxml2::XMLElement* cpElem = inputCPList->InsertNewChildElement("CPConnection");
			cpElem->SetAttribute("nodeID", node->getNodeID());
			cpElem->SetAttribute("pinIndex", pinIndex);
		}
	}
}
