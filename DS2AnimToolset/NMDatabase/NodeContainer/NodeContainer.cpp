#include "NodeContainer.h"

namespace db
{
	void NodeContainer::removeNode(int index) {
		if (index < 0 || index >= m_nodes.size())
			throw std::out_of_range("Index out of range");

		m_nodes.erase(m_nodes.begin() + index);
	}

	Node* NodeContainer::getNode(int index) const {
		if (index < 0 || index >= m_nodes.size())
			throw std::out_of_range("Index out of range");

		return m_nodes[index];
	}

}