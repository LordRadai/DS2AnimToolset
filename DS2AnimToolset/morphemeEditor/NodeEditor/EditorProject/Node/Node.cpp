#include "Node.h"
#include "morpheme/mrDefines.h"

namespace NodeEditor
{
	namespace Project
	{
		bool Node::loadFromXMLElement(tinyxml2::XMLElement* xmlElement)
		{
			if (xmlElement)
				m_xmlElement = xmlElement;

			tinyxml2::XMLElement* attribList = m_xmlElement->FirstChildElement("Attributes");

			if (attribList)
			{
				tinyxml2::XMLElement* attribElem = attribList->FirstChildElement("Attribute");

				while (attribElem)
				{
					Attribute* attribute = new Attribute(this, attribElem);

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
					Node* node = new Node(inputNodeElem);

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

		int Node::getNodeID() const
		{
			if (m_xmlElement)
				return m_xmlElement->IntAttribute("nodeID");

			throw std::runtime_error("XML Element is null, cannot get Node ID.");
		}

		void Node::setNodeID(int id)
		{
			if (m_xmlElement)
				m_xmlElement->SetAttribute("nodeID", id);
		}

		int Node::getNodeTypeID()
		{
			if (m_xmlElement)
				return m_xmlElement->IntAttribute("typeID");
			throw std::runtime_error("XML Element is null, cannot get Node Type ID.");
		}

		void Node::setNodeTypeID(int typeID)
		{
			if (m_xmlElement)
				m_xmlElement->SetAttribute("typeID", typeID);
		}

		int Node::getParentNodeID() const
		{
			if (m_xmlElement)
				return m_xmlElement->IntAttribute("parentNodeID");

			throw std::runtime_error("XML Element is null, cannot get Parent Node ID.");
		}

		void Node::setParentNodeID(int id)
		{
			if (m_xmlElement)
				m_xmlElement->SetAttribute("parentNodeID", id);
		}

		Attribute* Node::getAttribute(size_t index) const
		{
			if (index < m_attributes.size())
				return m_attributes[index];

			return nullptr;
		}

		Attribute* Node::getAttribute(const std::string& name) const
		{
			for (Attribute* attribute : m_attributes)
			{
				if (attribute->getName() == name)
					return attribute;
			}

			return nullptr;
		}

		void Node::addAttribute(Attribute* attribute)
		{
			m_attributes.push_back(attribute);

			tinyxml2::XMLElement* attribList = m_xmlElement->FirstChildElement("Attributes");

			if (!attribList)
				attribList = m_xmlElement->InsertNewChildElement("Attributes");

			attribList->InsertEndChild(attribute->getXMLElement());
		}

		Attribute* Node::createAttribute(const std::string& name)
		{
			tinyxml2::XMLElement* attribList = m_xmlElement->FirstChildElement("Attributes");

			Attribute* attribute = new Attribute(this, attribList);
			attribute->setName(name);
			addAttribute(attribute);

			return attribute;
		}

		const std::string Node::getContainerType() const
		{
			if (m_xmlElement && m_xmlElement->FindAttribute("containerType"))
				return m_xmlElement->Attribute("containerType");

			return "";
		}

		void Node::setContainerType(const std::string& type)
		{
			if (m_xmlElement)
				m_xmlElement->SetAttribute("containerType", type.c_str());
		}

		Node* Node::getInputNodeAtIndex(size_t index) const
		{
			if (index < m_inputNodes.size())
				return m_inputNodes[index];

			return nullptr;
		}

		Node* Node::getInputNode(int nodeID) const
		{
			for (Node* inputNode : m_inputNodes)
			{
				if (inputNode->getNodeID() == nodeID)
					return inputNode;
			}
			return nullptr;
		}

		void Node::addInputNode(Node* node)
		{
			m_inputNodes.push_back(node);

			tinyxml2::XMLElement* inputNodeList = m_xmlElement->FirstChildElement("InputNodes");

			if (!inputNodeList)
				inputNodeList = m_xmlElement->InsertNewChildElement("InputNodes");

			inputNodeList->InsertEndChild(node->getXMLElement());
		}

		Node* Node::createInputNode(const std::string& name, int nodeID, int typeID)
		{
			tinyxml2::XMLElement* inputNodeList = m_xmlElement->FirstChildElement("InputNodes");

			if (!inputNodeList)
				inputNodeList = m_xmlElement->InsertNewChildElement("InputNodes");

			Node* node = new Node(inputNodeList);
			node->setName(name);
			node->setNodeID(nodeID);
			node->setNodeTypeID(typeID);

			addInputNode(node);
			return node;
		}

		Node* Node::createInputBlendTreeNode(const std::string& name, int nodeID)
		{
			Node* node = createInputNode(name, nodeID, -1);
			node->setContainerType("BlendTree");
			return node;
		}

		Node* Node::createInputStateMachineNode(const std::string& name, int nodeID)
		{
			Node* node = createInputNode(name, nodeID, NODE_TYPE_STATE_MACHINE);
			node->setContainerType("StateMachine");
			return node;
		}

		Node* Node::getChildNodeAtIndex(size_t index) const
		{
			if (index < m_childrenNodes.size())
				return m_childrenNodes[index];

			return nullptr;
		}

		Node* Node::getChildNode(int nodeID) const
		{
			for (Node* childNode : m_childrenNodes)
			{
				if (childNode->getNodeID() == nodeID)
					return childNode;
			}

			return nullptr;
		}

		void Node::addChildNode(Node* node)
		{
			m_childrenNodes.push_back(node);
			tinyxml2::XMLElement* childNodeList = m_xmlElement->FirstChildElement("ChildNodes");

			if (!childNodeList)
				childNodeList = m_xmlElement->InsertNewChildElement("ChildNodes");

			childNodeList->InsertEndChild(node->getXMLElement());
		}

		Node* Node::createChildNode(const std::string& name, int nodeID, int typeID)
		{
			tinyxml2::XMLElement* childNodeList = m_xmlElement->FirstChildElement("ChildNodes");

			if (!childNodeList)
				childNodeList = m_xmlElement->InsertNewChildElement("ChildNodes");

			Node* node = new Node(childNodeList);
			node->setName(name);
			node->setNodeID(nodeID);
			node->setNodeTypeID(typeID);
			addChildNode(node);
			return node;
		}

		Node* Node::createChildBlendTreeNode(const std::string& name, int nodeID)
		{
			Node* node = createChildNode(name, nodeID, -1);
			node->setContainerType("BlendTree");
			return node;
		}

		Node* Node::createChildStateMachineNode(const std::string& name, int nodeID)
		{
			Node* node = createChildNode(name, nodeID, NODE_TYPE_STATE_MACHINE);
			node->setContainerType("StateMachine");

			return node;
		}

		Transition* Node::getTransition(size_t index) const
		{
			if (index < m_transitions.size())
				return m_transitions[index];

			return nullptr;
		}

		void Node::addTransition(Transition* transition)
		{
			m_transitions.push_back(transition);
			tinyxml2::XMLElement* transitionList = m_xmlElement->FirstChildElement("Transitions");

			if (!transitionList)
				transitionList = m_xmlElement->InsertNewChildElement("Transitions");

			transitionList->InsertEndChild(transition->getXMLElement());
		}

		Transition* Node::createTransition(int sourceNodeID, int destinationNodeID)
		{
			if (!isNodeStateMachine())
				throw std::runtime_error("Transitions can only be created for State Machine nodes.");

			if (!getChildNode(sourceNodeID) || !getChildNode(destinationNodeID))
				throw std::runtime_error("Source or Destination Node ID does not exist among child nodes.");

			tinyxml2::XMLElement* transitionList = m_xmlElement->FirstChildElement("Transitions");

			if (!transitionList)
				transitionList = m_xmlElement->InsertNewChildElement("Transitions");

			Transition* transition = new Transition(transitionList);
			transition->setSourceNodeID(sourceNodeID);
			transition->setDestinationNodeID(destinationNodeID);
			addTransition(transition);
			return transition;
		}

		const std::string Node::getInputControlParameter(size_t index) const
		{
			if (index < m_inputControlParameters.size())
				return m_inputControlParameters[index];

			return "";
		}

		void Node::addInputControlParameter(ControlParameter* cp)
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
