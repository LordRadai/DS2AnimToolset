#include "Transition.h"
#include "NodeEditor/Node/Node.h"
#include "NodeEditor/imnodes/imnodes.h"
#include "NodeEditor/imnodes/imnodes_internal.h"
#include "imgui/imgui_internal.h"
#include <cmath>

namespace NodeEditor
{
	Transition::Transition(NodeEditorBase* editor, Graph* parent, int nodeID, Node* sourceNode, Node* destinationNode) : Node(editor, parent, nodeID, sourceNode->getName() + "_" + destinationNode->getName(), nullptr),
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
}
