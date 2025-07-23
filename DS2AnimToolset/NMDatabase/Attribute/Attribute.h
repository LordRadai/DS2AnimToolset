#pragma once
#include "RCore.h"

namespace db
{
	class Database;

	class Attribute
	{
	protected:
		Attribute* m_parent;
		std::string m_name;
		std::string m_type;
		tinyxml2::XMLElement* m_xmlElement = nullptr;

	public:
		Attribute(Attribute* parent, std::string name, std::string type) :
			m_parent(parent),
			m_name(name),
			m_type(type),
			m_xmlElement(nullptr)
		{};

		/**
		 * \brief Get the root node.
		 * 
		 * \return The root node of the database.
		 */
		Database* getDatabase() const;

		/**
		 * \brief Get the parent attribute.
		 * 
		 * \return The parent attribute of this attribute.
		 */
		Attribute* getParentAttribute() const { return m_parent; }

		/**
		 * \brief Get the type of the attribute.
		 * 
		 * \return The type of the attribute.
		 */
		std::string getType() const { return m_type; }

		/**
		 * \brief Get the XML element associated with this attribute.
		 * 
		 * \return The XML element for this attribute.
		 */
		tinyxml2::XMLElement* getXMLElement() const { return m_xmlElement; }

		void setType(const std::string& type) { m_type = type; }
		void setParentAttribute(Attribute* parent) { m_parent = parent; }

		virtual ~Attribute() {};

		virtual bool compare(Attribute* other) = 0;
		virtual void assign(Attribute* other) = 0;

		virtual std::string getName() const { return m_name; }
		virtual void setName(const std::string& name) { m_name = name; }

		/**
		 * \brief Writes the XMLElement with a type attribute.
		 * 
		 * \return true if succeeded, false otherwise.
		 */
		virtual bool writeStartElementXML(int format);

		/**
		 * Writes the value to the previously created XMLElement. This method needs to be overridden by derived classes.
		 * 
		 * \return true if succeeded, false otherwise.
		 */
		virtual bool writeValueXML(int format) = 0;

		/**
		 * \brief Writes the XML representation of the attribute.
		 *
		 * \return true if succeeded, false otherwise.
		 */
		bool writeXML(int format);

		/**
		 * \brief Converts the attribute to a database path.
		 *
		 * \return The database path as a string.
		 */
		std::string toDatabasePath() const;
	};
}
