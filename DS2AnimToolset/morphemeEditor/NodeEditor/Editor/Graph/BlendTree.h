#pragma once
#include "Graph.h"
#include "NodeEditor/Editor/Node/BlendTreeOutputNode.h"
#include "NodeEditor/Editor/Node/ControlParametersNode.h"

namespace NodeEditor
{
	class BlendTree : public Graph
	{
		BlendTreeOutputNode* m_resultNode;
		ControlParametersNode* m_controlParametersNode;
		std::vector<Link*> m_links;
	public:
		BlendTree(Editor* editor, Graph* parent, const std::string& name, int graphNodeID);
		~BlendTree() override;

		virtual void draw() override;
		virtual void updateNodePositions() override;

		void addLink(Link* link) { m_links.push_back(link); }
		const std::vector<Link*>& getLinks() const { return m_links; }

		void setControlParamsNodePosition(float x, float y);
		ControlParametersNode* getControlParametersNode() const { return m_controlParametersNode; }

		Node* createNode(int nodeID, const std::string& typeName, const std::string& name = "");
		Node* createNode(int nodeID, const std::string& typeName, const std::string& name, float x, float y);

		BlendTreeOutputNode* getOutputNode() const { return m_resultNode; }
		DataPin* getControlParameterDataPin(const std::string& paramName) const;

		bool connectToOutput(Pin* outputPin);
	};
}
