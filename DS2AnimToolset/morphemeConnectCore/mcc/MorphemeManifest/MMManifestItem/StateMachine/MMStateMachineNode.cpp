#include "MMStateMachineNode.h"
#include "mcc/AttributeInfo/AttributeInfo.h"

namespace mcc
{
	mcd::StateMachineNode* MMStateMachineNode::createStateMachineNode(const std::string name)
	{
		mcd::StateMachineNode* node = new mcd::StateMachineNode(nullptr, name, 0.f, 0.f, 100.f, 50.f, this->getName(), this->getVersion());

		for (size_t i = 0; i < this->m_attributes.size(); i++)
		{
			mcc::AttributeInfo attrInfo(this->m_attributes[i]);

			attrInfo.createDatabaseAttribute(node->getAttributes());
		}

		return node;
	}
}