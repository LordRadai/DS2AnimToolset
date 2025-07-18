#include "NodePins.h"
#include <stdexcept>

namespace MCN
{
	namespace Node
	{
		const std::string FunctionalPin::getInterface(size_t index) const
		{
			if (index < m_interfaces.size())
				return m_interfaces[index];

			throw std::out_of_range("Index out of range for FunctionalPin interfaces");
			return ""; // This line will never be reached, but is needed to avoid compiler warnings
		}
	}
}