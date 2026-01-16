#pragma once
#include "RCore.h"
#include "XMLElement/XMLElement.h"

namespace db
{
	class IntAttribute;
	class BoolAttribute;
	class EnumAttribute;
	class DoubleAttribute;
	class FloatAttribute;
	class StringAttribute;
	class Vector3Attribute;
	class Matrix34Attribute;
	class PointerAttribute;
	class QuaternionAttribute;
	class IntArrayAttribute;
	class BoolArrayAttribute;
	class EnumArrayAttribute;
	class FloatArrayAttribute;
	class DoubleArrayAttribute;
	class StringArrayAttribute;
	class Vector3ArrayAttribute;
	class Matrix34ArrayAttribute;
	class QuaternionArrayAttribute;
	class PointerArrayAttribute;
	class CompositeAttribute;
	class NodeContainer;
	class AttributeArray;
	class CompoundAttribute;
	class Node;
	class Database;
	class SingleValueAttribute;
	class ArrayAttribute;
	class SaverXML;
	class LoaderXML;

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
		virtual IntAttribute* asInt() const { return nullptr; }
		virtual BoolAttribute* asBool() const { return nullptr; }
		virtual EnumAttribute* asEnum() const { return nullptr; }
		virtual DoubleAttribute* asDouble() const { return nullptr; }
		virtual FloatAttribute* asFloat() const { return nullptr; }
		virtual StringAttribute* asString() const { return nullptr; }
		virtual Vector3Attribute* asVector3() const { return nullptr; }
		virtual Matrix34Attribute* asMatrix34() const { return nullptr; }
		virtual PointerAttribute* asPointer() const { return nullptr; }
		virtual QuaternionAttribute* asQuaternion() const { return nullptr; }
		virtual IntArrayAttribute* asIntArray() const { return nullptr; }
		virtual BoolArrayAttribute* asBoolArray() const { return nullptr; }
		virtual EnumArrayAttribute* asEnumArray() const { return nullptr; }
		virtual FloatArrayAttribute* asFloatArray() const { return nullptr; }
		virtual DoubleArrayAttribute* asDoubleArray() const { return nullptr; }
		virtual StringArrayAttribute* asStringArray() const { return nullptr; }
		virtual Vector3ArrayAttribute* asVector3Array() const { return nullptr; }
		virtual Matrix34ArrayAttribute* asMatrix34Array() const { return nullptr; }
		virtual PointerArrayAttribute* asPointerArray() const { return nullptr; }
		virtual QuaternionArrayAttribute* asQuaternionArray() const { return nullptr; }
		virtual CompositeAttribute* asComposite() const { return nullptr; }
		virtual NodeContainer* asNodeContainer() const { return nullptr; }
		virtual AttributeArray* asAttributeArray() const { return nullptr; }
		virtual CompoundAttribute* asCompound() const { return nullptr; }
		virtual Node* asNode() const { return nullptr; }
		virtual Database* asDatabase() const { return nullptr; }
		virtual SingleValueAttribute* asSingleValue() const { return nullptr; }
		virtual ArrayAttribute* asArray() const { return nullptr; }
		virtual bool isInt() const { return false; }
		virtual bool isBool() const { return false; }
		virtual bool isEnum() const { return false; }
		virtual bool isDouble() const { return false; }
		virtual bool isFloat() const { return false; }
		virtual bool isString() const { return false; }
		virtual bool isVector3() const { return false; }
		virtual bool isMatrix34() const { return false; }
		virtual bool isPointer() const { return false; }
		virtual bool isQuaternion() const { return false; }
		virtual bool isIntArray() const { return false; }
		virtual bool isBoolArray() const { return false; }
		virtual bool isEnumArray() const { return false; }
		virtual bool isFloatArray() const { return false; }
		virtual bool isDoubleArray() const { return false; }
		virtual bool isStringArray() const { return false; }
		virtual bool isVector3Array() const { return false; }
		virtual bool isMatrix34Array() const { return false; }
		virtual bool isPointerArray() const { return false; }
		virtual bool isQuaternionArray() const { return false; }
		virtual bool isComposite() const { return false; }
		virtual bool isNodeContainer() const { return false; }
		virtual bool isAttributeArray() const { return false; }
		virtual bool isSingleValue() const { return false; }
		virtual bool isArray() const { return false; }
		virtual bool isCompound() const { return false; }
		virtual bool isNode() const { return false; }
		virtual bool isDatabase() const { return false; }
		virtual std::string getEscapedName() const;
		virtual std::string getName() const { return m_name; }
		virtual void setName(const std::string& name) { m_name = name; }

		/**
		 * \brief Reads the XML element with a type attribute.
		 * 
		 * \return true if succeeded, false otherwise.
		 */
		virtual bool readValueXML(int format, db::XMLElement* xmlElement, LoaderXML* loader) = 0;

		/**
		 * \brief Writes the XMLElement with a type attribute.
		 * 
		 * \return true if succeeded, false otherwise.
		 */
		virtual bool writeStartElementXML(int format, SaverXML* saver);

		/**
		 * Writes the value to the previously created XMLElement. This method needs to be overridden by derived classes.
		 * 
		 * \return true if succeeded, false otherwise.
		 */
		virtual bool writeValueXML(int format, SaverXML* saver) = 0;

		/**
		 * \brief Writes the XML representation of the attribute.
		 *
		 * \return true if succeeded, false otherwise.
		 */
		bool writeXML(int format, SaverXML* saver);

		/**
		 * \brief Converts the attribute to a database path.
		 *
		 * \return The database path as a string.
		 */
		std::string toDatabasePath() const;

		template<typename T>
		bool isOfType() const
		{
			return dynamic_cast<const T*>(this) != nullptr;
		}

		Attribute* attributeFromDatabasePath(const std::string& path, bool resolveLast) const;
	};
}
