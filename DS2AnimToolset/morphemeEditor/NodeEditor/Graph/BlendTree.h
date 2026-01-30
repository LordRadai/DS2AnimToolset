#pragma once
#include "Graph.h"

namespace NodeEditor
{
	class BlendTree : public Graph
	{
		Node* m_resultNode;
		std::vector<Link*> m_links;
		ImVec2 m_controlParamsNodePos;
	public:
		BlendTree(NodeEditor* editor, Graph* parent, const std::string& name);
		~BlendTree() override;

		virtual void draw() override;

		void addLink(Link* link) { m_links.push_back(link); }
		const std::vector<Link*>& getLinks() const { return m_links; }

		Node* getOutputNode() const { return m_resultNode; }

		bool connectToOutput(Pin* outputPin);
	private:
		void drawResultNode();
	};
}
