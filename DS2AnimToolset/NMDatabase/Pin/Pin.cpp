#include "Pin.h"

namespace db
{
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
}