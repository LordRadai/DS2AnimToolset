#include "Pin.h"
#include "../mcnSerializer/mcnSerializer.h"

namespace db
{
	bool FunctionalPin::isValid() const
	{
		if (!Pin::isValid())
			return false;

		for (size_t i = 0; i < m_interfaces.size(); i++)
		{
			if (m_interfaces[i].empty())
				return false;
		}

		return true;
	}

	tinyxml2::XMLElement* FunctionalPin::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* pinElement = Node::serialize(parent);
		pinElement->SetName("FunctionalPin");
		
		mcnSerializer::createStringArrayElement(pinElement, "Interfaces", m_interfaces);
	}

	std::string FunctionalPin::getInterface(int index) const
	{
		if (index < 0 || index >= m_interfaces.size())
			throw std::out_of_range("Index out of range");
		return m_interfaces[index];
	}

	void FunctionalPin::addInterface(const std::string& interf)
	{
		m_interfaces.push_back(interf);
	}

	void FunctionalPin::removeInterface(int index)
	{
		if (index < 0 || index >= m_interfaces.size())
			throw std::out_of_range("Index out of range");

		m_interfaces.erase(m_interfaces.begin() + index);
	}

	bool DataPin::isValid() const
	{
		if (!Pin::isValid())
			return false;

		if (m_dataType == "bool")
			return true;
		else if (m_dataType == "int")
			return true;
		else if (m_dataType == "uint")
			return true;
		else if (m_dataType == "float")
			return true;
		else if (m_dataType == "vector3")
			return true;
		else if (m_dataType == "vector4")
			return true;
		else if (m_dataType == "quaternion")
			return true;
		else if (m_dataType == "matrix34")
			return true;

		return false;
	}

	tinyxml2::XMLElement* DataPin::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* pinElement = Node::serialize(parent);
		pinElement->SetName("DataPin");

		mcnSerializer::createEnumElement(pinElement, "Type", m_dataType);

		return pinElement;
	}

	tinyxml2::XMLElement* PassDownPin::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* pinElement = Node::serialize(parent);
		pinElement->SetName("PassDownPin");

		mcnSerializer::createBoolElement(pinElement, "Reference", m_bReference);

		return pinElement;
	}
}