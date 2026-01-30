#pragma once
#include <vector>
#include <string>
#include "NodeEditor/imnodes/imnodes.h"
#include "NodeEditor/Node/Node.h"
#include "NodeEditor/Transition/Transition.h"
#include "NodeEditor/Link/Link.h"

namespace NodeEditor
{
	class Graph : public Entity
	{
		Graph* m_parentGraph;
		ImNodesContext* m_context;
		Node* m_controlParametersNode;
		std::vector<Node*> m_nodes;
		std::vector<Transition*> m_transitions;
		std::vector<Link*> m_links;
		ImVec2 m_panning;
	public:
		Graph(Graph* parent, const std::string& name);
		~Graph();

		void draw();

		Graph* getParentGraph() const { return m_parentGraph; }
		bool isRootGraph() const { return m_parentGraph == nullptr; }

		ImVec2 getPanning() const { return m_panning; }
		void setPanning(float x, float y);

		Node* getControlParametersNode() const { return m_controlParametersNode; }

		Node* getNode(int nodeID) const;
		Node* getNode(const std::string& name) const;

		Transition* getTransition(int nodeID) const;
		Transition* getTransition(const std::string& name) const;

		Node* createNode(int nodeID, const std::string& name);
		Node* createContainerNode(int nodeID, const std::string& name);
		Transition* createTransition(int nodeID, Node* sourceNode, Node* destinationNode);

		void createControlParameterFloat(const std::string& name);
		void createControlParameterInt(const std::string& name);
		void createControlParameterUInt(const std::string& name);
		void createControlParameterBool(const std::string& name);
		void createControlParameterVector3(const std::string& name);
		void createControlParameterVector4(const std::string& name);
		void createControlParameterQuaternion(const std::string& name);

		void removeNode(Node* node);

		void addLink(Link* link) { m_links.push_back(link); }

		const int getID() const { return m_id; }
		const std::string& getName() const { return m_name; }
		const std::vector<Node*>& getNodes() const { return m_nodes; }
		const std::vector<Link*>& getLinks() const { return m_links; }

		const std::string getFullName() const;

	private:
		void getFreePosition(float& x, float& y);
	};
}
