#pragma once
#include "RCore.h"
#include "../Node/Node.h"

namespace db
{
	typedef std::string Interface;

	class Pin : public Node
	{
	public:
		Pin(Node* parent, std::string name) : Node(parent, name) {};
		~Pin() {};

		std::string getName() const { return m_name; };
	};

	class PassDownPin : public Pin
	{
		bool m_bReference;

	public:
		PassDownPin(Node* parent, std::string name, bool bReference = true) : Pin(parent, name), m_bReference(bReference) {};
		~PassDownPin() {};

		bool isReference() const { return m_bReference; };
	};

	class FunctionalPin : public Pin
	{
		std::vector<Interface> m_interfaces;

	public:
		FunctionalPin(Node* parent, std::string name) : Pin(parent, name){};
		~FunctionalPin() {};

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
		~DataPin() {};

		std::string getDataType() const { return m_dataType; };
	};
}