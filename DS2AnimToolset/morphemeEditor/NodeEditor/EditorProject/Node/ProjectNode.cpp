#include "ProjectNode.h"

namespace NodeEditor
{
	namespace Project
	{
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
					ProjectAttribute* attribute = new ProjectAttribute(this, attribElem);

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
					ProjectNode* node = new ProjectNode(inputNodeElem);

					node->loadFromXMLElement(inputNodeElem);
					m_inputNodes.push_back(node);

					inputNodeElem = inputNodeElem->NextSiblingElement("Node");
				}
			}

			tinyxml2::XMLElement* inputCPList = m_xmlElement->FirstChildElement("InputControlParameters");

			if (inputCPList)
			{
				tinyxml2::XMLElement* inputCPElem = inputCPList->FirstChildElement("ControlParameter");

				while (inputCPElem)
				{
					if (inputCPElem->GetText())
						m_inputControlParameters.push_back(std::string(inputCPElem->GetText()));

					inputCPElem = inputCPElem->NextSiblingElement("ControlParameter");
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

			ProjectAttribute* attribute = new ProjectAttribute(this, attribList);
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

			ProjectNode* node = new ProjectNode(inputNodeList);
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

			ProjectNode* node = new ProjectNode(childNodeList);
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

			ProjectTransition* transition = new ProjectTransition(transitionList);
			transition->setSourceNodeID(sourceNodeID);
			transition->setDestinationNodeID(destinationNodeID);
			addTransition(transition);
			return transition;
		}

		const std::string ProjectNode::getInputControlParameter(size_t index) const
		{
			if (index < m_inputControlParameters.size())
				return m_inputControlParameters[index];

			return "";
		}

		void ProjectNode::addInputControlParameter(ProjectControlParameter* cp)
		{
			std::string controlParamName = cp->getName();

			m_inputControlParameters.push_back(controlParamName);
			tinyxml2::XMLElement* inputCPList = m_xmlElement->FirstChildElement("InputControlParameters");

			if (!inputCPList)
				inputCPList = m_xmlElement->InsertNewChildElement("InputControlParameters");

			tinyxml2::XMLElement* cpElem = inputCPList->InsertNewChildElement("ControlParameter");
			cpElem->SetText(controlParamName.c_str());
		}
	}
}
