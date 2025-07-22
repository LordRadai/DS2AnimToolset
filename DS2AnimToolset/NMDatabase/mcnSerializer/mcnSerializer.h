#pragma once
#include "RCore.h"
#include "NMPlatform/NMMatrix34.h"

class mcnSerializer
{
	/**
	 * Creates an XMLElement with a name and a value. Text will be added only if non empty.
	 * 
	 * \param parent The parent XML element to which the new element will be added.
	 * \param name The name of the element to be created.
	 * \param value The value to be set as the text content of the element. If the value is empty, no text will be added.
	 * \return A pointer to the newly created XMLElement, or nullptr if the creation failed.
	 */
	static tinyxml2::XMLElement* createElement(tinyxml2::XMLElement* parent, std::string name, std::string value);

	/**
	 * Creates an XMLElement with a name, type, and value.
	 *
	 * \param parent The parent XML element to which the new element will be added.
	 * \param name The name of the element to be created.
	 * \param type The type of the element (e.g., "string", "int", etc.).
	 * \param value The value to be set as the text content of the element.
	 * \return A pointer to the newly created XMLElement, or nullptr if the creation failed.
	 */
	static tinyxml2::XMLElement* createTypedElement(tinyxml2::XMLElement* parent, std::string name, std::string type, std::string value);

	/**
	 * Creates an XMLElement with a name and a type, and sets the text content to the specified value.
	 *
	 * \param parent The parent XML element to which the new element will be added.
	 * \param name The name of the element to be created.
	 * \param type The type of the element (e.g., "string", "int", etc.).
	 * \param value The value to be set as the text content of the element.
	 * \return A pointer to the newly created XMLElement, or nullptr if the creation failed.
	 */
	static tinyxml2::XMLElement* createNamedAndTypedElement(tinyxml2::XMLElement* parent, std::string name, std::string attribName, std::string type, std::string value);

public:
	/**
	 * Creates an XMLElement string element.
	 * 
	 * \param parent The parent XML element to which the new element will be added.
	 * \param name The name of the element to be created.
	 * \param value The value to be set as the text content of the element. If the value is empty, no text will be added.
	 * \return A pointer to the newly created XMLElement, or nullptr if the creation failed.
	 */
	static tinyxml2::XMLElement* createStringElement(tinyxml2::XMLElement* parent, std::string name, std::string value);

	/**
	 * Creates an XMLElement with a name and a value, where the value is of a specific type.
	 *
	 * \param parent The parent XML element to which the new element will be added.
	 * \param name The name of the element to be created.
	 * \param type The type of the element (e.g., "string", "int", etc.).
	 * \param value The value to be set as the text content of the element.
	 * \return A pointer to the newly created XMLElement, or nullptr if the creation failed.
	 */
	static tinyxml2::XMLElement* createBoolElement(tinyxml2::XMLElement* parent, std::string name, bool value);

	/**
	 * Creates an XMLElement with a name and a value, where the value is of a specific type.
	 *
	 * \param parent The parent XML element to which the new element will be added.
	 * \param name The name of the element to be created.
	 * \param type The type of the element (e.g., "string", "int", etc.).
	 * \param value The value to be set as the text content of the element.
	 * \return A pointer to the newly created XMLElement, or nullptr if the creation failed.
	 */
	static tinyxml2::XMLElement* createIntElement(tinyxml2::XMLElement* parent, std::string name, int value);

	/**
	 * Creates an XMLElement with a name and a value, where the value is of a specific type.
	 *
	 * \param parent The parent XML element to which the new element will be added.
	 * \param name The name of the element to be created.
	 * \param type The type of the element (e.g., "string", "int", etc.).
	 * \param value The value to be set as the text content of the element.
	 * \return A pointer to the newly created XMLElement, or nullptr if the creation failed.
	 */
	static tinyxml2::XMLElement* createUIntElement(tinyxml2::XMLElement* parent, std::string name, uint32_t value);

	/**
	 * Creates an XMLElement with a name and a value, where the value is of a specific type.
	 *
	 * \param parent The parent XML element to which the new element will be added.
	 * \param name The name of the element to be created.
	 * \param type The type of the element (e.g., "string", "int", etc.).
	 * \param value The value to be set as the text content of the element.
	 * \return A pointer to the newly created XMLElement, or nullptr if the creation failed.
	 */
	static tinyxml2::XMLElement* createFloatElement(tinyxml2::XMLElement* parent, std::string name, float value);

	/**
	 * Creates an XMLElement with a name and a value, where the value is of a specific type.
	 *
	 * \param parent The parent XML element to which the new element will be added.
	 * \param name The name of the element to be created.
	 * \param type The type of the element (e.g., "string", "int", etc.).
	 * \param value The value to be set as the text content of the element.
	 * \return A pointer to the newly created XMLElement, or nullptr if the creation failed.
	 */
	static tinyxml2::XMLElement* createVector3Element(tinyxml2::XMLElement* parent, std::string name, NMP::Vector3 value);

	/**
	 * Creates an XMLElement with a name and a value, where the value is of a specific type.
	 *
	 * \param parent The parent XML element to which the new element will be added.
	 * \param name The name of the element to be created.
	 * \param type The type of the element (e.g., "string", "int", etc.).
	 * \param value The value to be set as the text content of the element.
	 * \return A pointer to the newly created XMLElement, or nullptr if the creation failed.
	 */
	static tinyxml2::XMLElement* createQuatElement(tinyxml2::XMLElement* parent, std::string name, NMP::Quat value);

	/**
	 * Creates an XMLElement with a name and a value, where the value is of a specific type.
	 *
	 * \param parent The parent XML element to which the new element will be added.
	 * \param name The name of the element to be created.
	 * \param type The type of the element (e.g., "string", "int", etc.).
	 * \param value The value to be set as the text content of the element.
	 * \return A pointer to the newly created XMLElement, or nullptr if the creation failed.
	 */
	static tinyxml2::XMLElement* createMatrix34Element(tinyxml2::XMLElement* parent, std::string name, NMP::Matrix34 value);

	/**
	 * Creates an XMLElement with a name and a value, where the value is of a specific type.
	 *
	 * \param parent The parent XML element to which the new element will be added.
	 * \param name The name of the element to be created.
	 * \param type The type of the element (e.g., "string", "int", etc.).
	 * \param value The value to be set as the text content of the element.
	 * \return A pointer to the newly created XMLElement, or nullptr if the creation failed.
	 */
	static tinyxml2::XMLElement* createStringArrayElement(tinyxml2::XMLElement* parent, std::string name, std::vector<std::string> array);

	/**
	 * Creates an XMLElement with a name and a value, where the value is of a specific type.
	 *
	 * \param parent The parent XML element to which the new element will be added.
	 * \param name The name of the element to be created.
	 * \param type The type of the element (e.g., "string", "int", etc.).
	 * \param value The value to be set as the text content of the element.
	 * \return A pointer to the newly created XMLElement, or nullptr if the creation failed.
	 */
	static tinyxml2::XMLElement* createEnumElement(tinyxml2::XMLElement* parent, std::string name, std::string value);

	/*	
	* \brief Creates a node element.
	* \param parent The parent element to which the new node will be added.
	* \param name The name of the node element. This is not the attribute name, but the element name itself.
	* \param attributeName The name of the attribute that will be added to the node element.
	* \return A pointer to the newly created node element.
	*/
	static tinyxml2::XMLElement* createNodeElement(tinyxml2::XMLElement* parent, std::string name, std::string attributeName);

	/*
	* @brief Creates a node container element.
	* @param parent The parent element to which the new node container will be added.
	* @param name The name of the node container element. This is not the attribute name, but the element name itself.
	* @return A pointer to the newly created node container element.
	*/
	static tinyxml2::XMLElement* createNodeContainerElement(tinyxml2::XMLElement* parent, std::string name);

	/**
	 * Creates a DBPointer element.
	 *
	 * \param parent The parent element to which the new pointer will be added.
	 * \param name The name of the pointer element.
	 * \param value The value of the pointer, typically a string representing the path or reference.
	 * \return A pointer to the newly created pointer element, or nullptr if the creation failed.
	 */
	static tinyxml2::XMLElement* createPointerElement(tinyxml2::XMLElement* parent, std::string name, std::string value);
};