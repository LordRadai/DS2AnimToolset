#include "Transition.h"
#include "NodeEditor/Node/Node.h"
#include "NodeEditor/imnodes/imnodes.h"
#include "NodeEditor/imnodes/imnodes_internal.h"
#include "imgui/imgui_internal.h"
#include <cmath>

namespace NodeEditor
{
	Transition::Transition(NodeEditor* editor, Graph* parent, int nodeID, const std::string& typeName, Node* sourceNode, Node* destinationNode) : Node(editor, parent, nodeID, typeName, sourceNode->getName() + "_" + destinationNode->getName(), nullptr),
		m_sourceNode(sourceNode), m_destinationNode(destinationNode)
	{
	}

    void Transition::draw()
    {
        ImNodes::Transition(
            this->m_id,
            this->m_sourceNode->getID(),
			this->m_destinationNode->getID());
    }

    void Transition::editorGUI()
    {
        Node::editorGUI();

        ImGui::SeparatorText("Conditions");

        for (Condition* condition : m_conditions)
            condition->editorGUI();
	}

    void Transition::addCondition(Condition* condition)
    {
		condition->setName(makeConditionNameValid(condition->getName()));
        m_conditions.push_back(condition);
    }

    Condition* Transition::getCondition(size_t index) const
    {
        if (index < m_conditions.size())
            return m_conditions[index];

        return nullptr;
	}

    Condition* Transition::getCondition(const std::string& name) const
    {
        for (Condition* condition : m_conditions)
        {
            if (condition->getName() == name)
                return condition;
        }

        return nullptr;
	}

    const std::string Transition::makeConditionNameValid(const std::string& desiredName)
    {
        int numConditionsWithName = 0;

        for (Condition* condition : m_conditions)
        {
            if (condition->getName() == desiredName)
                numConditionsWithName++;
        }

        return desiredName + std::to_string(numConditionsWithName + 1);
	}
}
