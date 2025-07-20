#pragma once
#include "RCore.h"

namespace db
{
	class Node
	{
	protected:
		std::string m_name = "";
		Node* m_parent = nullptr;

	public:
		Node(Node* parent, std::string name) : m_parent(parent), m_name(name) {};

		virtual ~Node() {};
		virtual bool isValid() const { return true; }
		/*
		* @brief Serializes the node to an XML element.
		* Be sure to set the proper name for the XML element since it will be left empty by this function.
		* @param parent The parent XML element to which this node will be added.
		* @return A pointer to the created XML element representing this node.
		*/
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent) const;

		std::string getName() const { return m_name; };
		void setName(const std::string& name) { m_name = name; };

		Node* getParent() const { return m_parent; };
		void setParent(Node* parent) { m_parent = parent; };

		std::string getQualifiedName() const;
	};
}