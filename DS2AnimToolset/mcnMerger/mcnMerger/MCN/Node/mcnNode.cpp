#include "mcnNode.h"
#include <stdexcept>

namespace MCN
{
	namespace Node
	{
		const NodeAttribute* mcnNode::getAttribute(size_t index) const
		{
			if (index < m_attributes.size())
				return &m_attributes[index];

			throw std::out_of_range("Index out of range for MCNNode attributes");
			return nullptr;
		}

		const FunctionalPin* mcnNode::getFunctionalPin(size_t index) const
		{
			if (index < m_functionalPins.size())
				return &m_functionalPins[index];

			throw std::out_of_range("Index out of range for MCNNode functional pins");
			return nullptr;
		}

		const DataPin* mcnNode::getDataPin(size_t index) const
		{
			if (index < m_dataPins.size())
				return &m_dataPins[index];

			throw std::out_of_range("Index out of range for MCNNode data pins");
			return nullptr;
		}
	}
}