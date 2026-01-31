#include "EditorProject.h"
#include "morpheme/mrDefines.h"

namespace NodeEditor
{
	namespace Project
	{
		EditorProject::EditorProject() : m_rootElement(nullptr)
		{
			m_xmlDoc = new tinyxml2::XMLDocument();
			m_rootElement = m_xmlDoc->NewElement("EditorProject");
		}

		EditorProject::~EditorProject()
		{
			delete m_xmlDoc;
		}

		const std::string EditorProject::getProjectName() const
		{
			if (m_rootElement)
			{
				const char* nameAttr = m_rootElement->Attribute("name");

				if (nameAttr)
					return std::string(nameAttr);
			}

			return "";
		}

		void EditorProject::setProjectName(const std::string& name)
		{
			if (m_rootElement)
				m_rootElement->SetAttribute("name", name.c_str());
		}

		void EditorProject::setRootNode(ProjectNode* node)
		{
			m_rootNode = node;
			tinyxml2::XMLElement* rootNodeElem = m_rootElement->FirstChildElement("RootNode");

			if (!rootNodeElem)
				rootNodeElem = m_rootElement->InsertNewChildElement("RootNode");

			rootNodeElem->InsertEndChild(node->getXMLElement());
		}

		ProjectNode* EditorProject::createRootNode(const std::string& name, int nodeID, int typeID)
		{
			tinyxml2::XMLElement* rootNodeElem = m_rootElement->FirstChildElement("RootNode");

			if (!rootNodeElem)
				rootNodeElem = m_rootElement->InsertNewChildElement("RootNode");

			ProjectNode* node = new ProjectNode(rootNodeElem);
			node->setName(name);
			node->setNodeID(nodeID);

			setRootNode(node);

			return node;
		}

		ProjectNode* EditorProject::createRootBlendTreeNode(const std::string& name, int nodeID)
		{
			ProjectNode* node = createRootNode(name, nodeID, -1);

			node->setContainerType("BlendTree");

			return node;
		}

		ProjectNode* EditorProject::createRootStateMachineNode(const std::string& name, int nodeID)
		{
			ProjectNode* node = createRootNode(name, nodeID, NODE_TYPE_STATE_MACHINE);

			node->setContainerType("StateMachine");

			return node;
		}

		ProjectControlParameter* EditorProject::getControlParameter(size_t index) const
		{
			if (index < m_controlParameters.size())
				return m_controlParameters[index];

			return nullptr;
		}

		ProjectControlParameter* EditorProject::getControlParameter(const std::string& name) const
		{
			for (ProjectControlParameter* cp : m_controlParameters)
			{
				if (cp->getName() == name)
					return cp;
			}

			return nullptr;
		}

		void EditorProject::addControlParameter(ProjectControlParameter* parameter)
		{
			m_controlParameters.push_back(parameter);

			tinyxml2::XMLElement* cpList = m_rootElement->FirstChildElement("ControlParameters");

			if (!cpList)
				cpList = m_rootElement->InsertNewChildElement("ControlParameters");

			cpList->InsertEndChild(parameter->getXMLElement());
		}

		ProjectControlParameter* EditorProject::createControlParameter(const std::string& name, const std::string& type)
		{
			tinyxml2::XMLElement* cpList = m_rootElement->FirstChildElement("ControlParameters");

			if (!cpList)
				cpList = m_rootElement->InsertNewChildElement("ControlParameters");

			ProjectControlParameter* cp = new ProjectControlParameter(cpList);

			cp->setName(name);
			cp->setType(type);
			addControlParameter(cp);

			return cp;
		}

		bool EditorProject::saveProject(const std::string& filePath)
		{
			m_xmlDoc->InsertFirstChild(m_rootElement);

			m_xmlDoc->SaveFile(filePath.c_str());

			return true;
		}

		bool EditorProject::loadProject(const std::string& filePath)
		{
			m_xmlDoc->LoadFile(filePath.c_str());

			m_rootElement = m_xmlDoc->FirstChildElement("EditorProject");

			tinyxml2::XMLElement* rootNode = m_rootElement->FirstChildElement("RootNode");

			if (rootNode)
			{
				tinyxml2::XMLElement* nodeElem = rootNode->FirstChildElement("Node");

				ProjectNode* node = new ProjectNode(nodeElem);

				node->loadFromXMLElement(nodeElem);
				m_rootNode = node;
			}

			tinyxml2::XMLElement* cpList = m_rootElement->FirstChildElement("ControlParameters");

			if (cpList)
			{
				tinyxml2::XMLElement* cpElem = cpList->FirstChildElement("ControlParameter");

				while (cpElem)
				{
					ProjectControlParameter* cp = new ProjectControlParameter(cpElem);

					cp->loadFromXMLElement(cpElem);
					m_controlParameters.push_back(cp);

					cpElem = cpElem->NextSiblingElement("ControlParameter");
				}
			}

			return true;
		}
	}	
}