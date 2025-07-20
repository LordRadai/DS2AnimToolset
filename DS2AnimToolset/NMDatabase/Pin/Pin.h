#pragma once
#include "RCore.h"
#include "../Node/Node.h"

namespace db
{
	typedef std::string Interface;

	class Pin : public Node
	{
	protected:
		Pin(Node* parent, std::string name) : Node(parent, name) {};
		virtual ~Pin() {};
		
	public:
		virtual bool isValid() const { return Node::isValid(); }
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent) { return nullptr; }

		std::string getName() const { return m_name; };
	};

	class PassDownPin : public Pin
	{
		bool m_bReference;

	public:
		PassDownPin(Node* parent, std::string name, bool bReference = true) : Pin(parent, name), m_bReference(bReference) {};
		virtual ~PassDownPin() {};
		virtual bool isValid() const { return true; }
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		bool isReference() const { return m_bReference; };
	};

	class FunctionalPin : public Pin
	{
		std::vector<Interface> m_interfaces;

	public:
		FunctionalPin(Node* parent, std::string name) : Pin(parent, name) {};

		virtual ~FunctionalPin() {};
		virtual bool isValid() const;
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		std::string getInterface(int index) const;
		void addInterface(const std::string& interf);
		void removeInterface(int index);

		size_t getNumInterfaces() const { return m_interfaces.size(); }
	};

	class DataPin : public Pin
	{
		std::string m_dataType;

	public:
		DataPin(Node* parent, std::string name, std::string dataType) : Pin(parent, name), m_dataType(dataType) {};

		virtual ~DataPin() {};
		virtual bool isValid() const;
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		std::string getDataType() const { return m_dataType; };
	};
}