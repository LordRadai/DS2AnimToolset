#include "NodeAttributes.h"
#include <stdexcept>

namespace MCN
{
	namespace Node
	{
		const AttributePlaceholder* AnimationSetAttribute::getAttributePlaceholder(size_t index) const
		{
			if (index < m_attributePlaceholders.size())
				return &m_attributePlaceholders[index];

			throw std::out_of_range("Index out of range for AnimationSetAttribute placeholders");
			return nullptr; // This line will never be reached, but is needed to avoid compiler warnings
		}
	}
}