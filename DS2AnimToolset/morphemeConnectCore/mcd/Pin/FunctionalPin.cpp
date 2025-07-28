#include "FunctionalPin.h"

namespace mcd
{
	FunctionalPin::FunctionalPin(db::Node* parent, const std::string& name)
		: Pin(parent, "FunctionalPin", name),
		  m_interfaces(std::make_unique<db::StringArrayAttribute>(this, "Interfaces")),
		  m_passThroughEnabled(std::make_unique<db::BoolAttribute>(this, "PassThroughEnabled", false))
	{
		addAttribute(m_interfaces.get());
	}

	void FunctionalPin::setPassThroughEnabled(bool enabled)
	{
		removeAttribute(m_passThroughEnabled.get());

		// Only add to the list if it is enabled.
		if (enabled)
			addAttribute(m_passThroughEnabled.get());

		m_passThroughEnabled->setValue(enabled);
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