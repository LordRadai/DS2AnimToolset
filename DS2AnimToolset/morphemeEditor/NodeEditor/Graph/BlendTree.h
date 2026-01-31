#pragma once
#include "Graph.h"
#include "NodeEditor/Node/BlendTreeOutputNode.h"

namespace NodeEditor
{
	class BlendTree : public Graph
	{
		BlendTreeOutputNode* m_resultNode;
		std::vector<Link*> m_links;
		ImVec2 m_controlParamsNodePos;
	public:
		BlendTree(NodeEditor* editor, Graph* parent, const std::string& name);
		~BlendTree() override;

		virtual void draw() override;

		void addLink(Link* link) { m_links.push_back(link); }
		const std::vector<Link*>& getLinks() const { return m_links; }

		Node* createNode(int nodeID, const std::string& typeName, const std::string& name = "");

		BlendTreeOutputNode* getOutputNode() const { return m_resultNode; }

		bool connectToOutput(Pin* outputPin);
	};
}
