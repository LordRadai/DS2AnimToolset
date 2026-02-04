#pragma once
#include "NodeEditor/Editor/Node/Node.h"
#include "NodeEditor/Editor/Condition/Condition.h"

namespace NodeEditor
{
	class Node;

	class Transition : public Node
	{
		Node* m_sourceNode;
		Node* m_destinationNode;
		std::vector<Condition*> m_conditions;

	public:
		Transition(Editor* editor, Graph* parent, int nodeID, const std::string& typeName, Node* sourceNode, Node* destinationNode);
		virtual ~Transition() override {}

		virtual void draw() override;
		virtual bool editorGUI() override;

		Node* getSourceNode() const { return m_sourceNode; }
		Node* getDestinationNode() const { return m_destinationNode; }

		void addCondition(Condition* condition);
		Condition* getCondition(size_t index) const;
		Condition* getCondition(const std::string& name) const;
		size_t getNumConditions() const { return m_conditions.size(); }
		Condition* createCondition(const std::string& typeName);

		const std::string makeConditionNameValid(const std::string& desiredName);
	};
}
