#pragma once
#include "Graph.h"

namespace NodeEditor
{
	class BlendTree : public Graph
	{
		Node* m_controlParametersNode;
		std::vector<Link*> m_links;

	public:
		BlendTree(NodeEditorBase* editor, Graph* parent, const std::string& name);
		~BlendTree() override;

		virtual void draw() override;

		Node* getControlParametersNode() const { return m_controlParametersNode; }

		void addLink(Link* link) { m_links.push_back(link); }
		const std::vector<Link*>& getLinks() const { return m_links; }

		void createControlParameterFloat(const std::string& name);
		void createControlParameterInt(const std::string& name);
		void createControlParameterUInt(const std::string& name);
		void createControlParameterBool(const std::string& name);
		void createControlParameterVector3(const std::string& name);
		void createControlParameterVector4(const std::string& name);
		void createControlParameterQuaternion(const std::string& name);
	};
}
