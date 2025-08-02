#include "FunctionalPin.h"

namespace mcd
{
	FunctionalPin::FunctionalPin(db::Node* parent, const std::string& name)
		: Pin(parent, "FunctionalPin", name),
		  m_interfaces(std::make_unique<db::StringArrayAttribute>(this, "Interfaces")),
		  m_passThroughEnabled(std::make_unique<db::BoolAttribute>(this, "PassThroughEnabled", false)),
	      m_input(std::make_unique<db::BoolAttribute>(this, "Input", false))
	{
	}

	void FunctionalPin::addInterface(const std::string& interfaceName)
	{
		if (m_interfaces->empty())
			addAttribute(m_interfaces.get());

		m_interfaces->add(interfaceName);
	}

	void FunctionalPin::removeInterface(const uint32_t index)
	{
		if (index < 0 || index >= m_interfaces->size())
			throw std::out_of_range("FunctionalPin::removeInterface() - Index out of range");

		m_interfaces->removeAt(index);

		if (m_interfaces->empty())
			removeAttribute(m_interfaces.get());
	}

	void FunctionalPin::setPassThroughEnabled(bool enabled)
	{
		removeAttribute(m_passThroughEnabled.get());

		// Only add to the list if it is enabled.
		if (enabled)
			addAttribute(m_passThroughEnabled.get());

		m_passThroughEnabled->setValue(enabled);
	}

	void FunctionalPin::setInput(bool input)
	{
		removeAttribute(m_input.get());

		// Only add to the list if it is an input.
		if (input)
			addAttribute(m_input.get());

		m_input->setValue(input);
	}

	std::string FunctionalPin::findInterface(const std::string& name) const
	{
		for (size_t i = 0; i < m_interfaces->size(); i++)
		{
			std::string element = m_interfaces->getElement(i);

			if (element == name)
				return element;
		}

		return "";
	}
}