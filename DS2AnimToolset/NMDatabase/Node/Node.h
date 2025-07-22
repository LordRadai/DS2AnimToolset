#pragma once
#include "RCore.h"

namespace db
{
	class Node
	{
	protected:
		std::string m_identifier;
		std::string m_label;
		Node* m_parent;

	public:
		/**
		 * @brief Constructs a Node with a parent, identifier, and label.
		 * @param parent Pointer to the parent node. Can be nullptr if this is a root node.
		 * @param identifier Unique identifier for the node.
		 * @param label Human-readable label for the node.
		 */
		Node(Node* parent, std::string identifier, std::string label) : m_parent(parent), m_identifier(identifier), m_label(label) {};

		virtual ~Node() {};

		/**
		 * @brief Checks if the node is valid.
		 * This function can be overridden by derived classes to implement specific validation logic.
		 * @return True if the node is valid, false otherwise.
		 */
		virtual bool isValid() const { return true; }

		/*
		* @brief Serializes the node to an XML element.
		* Be sure to set the proper name for the XML element since it will be left empty by this function.
		* @param parent The parent XML element to which this node will be added.
		* @return A pointer to the created XML element representing this node.
		*/
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		/**
		 * @brief Returns a qualified name for the node.
		 * The format is "parent.node".
		 * @return A string representing the qualified name of the node.
		 */
		virtual std::string getQualifiedName() const;

		std::string getIdentifier() const { return m_identifier; };

		std::string getName() const { return m_label; };
		void setName(const std::string& name) { m_label = name; };

		Node* getParent() const { return m_parent; };
		void setParent(Node* parent) { m_parent = parent; };
	};
}