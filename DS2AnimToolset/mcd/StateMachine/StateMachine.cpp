#include "StateMachine.h"
#include "XMLWriter.h"

namespace mcd
{
	tinyxml2::XMLElement* StateMachine::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Graph::serialize(parent);

		m_stateMachineNodes.serialize(element);
		m_transitionEdges.serialize(element);

		XMLWriter::createPointerElement(element, "DefaultState", m_defaultState->getQualifiedName());
	}
}