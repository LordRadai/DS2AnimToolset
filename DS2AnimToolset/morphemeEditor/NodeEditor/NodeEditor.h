#pragma once
#include <stack>
#include "Editor/Graph/Graph.h"
#include "Editor/Graph/BlendTree.h"
#include "Editor/Graph/StateMachine.h"
#include "Editor/Node/ControlParametersNode.h"
#include "Editor/ControlParameter/ControlParameter.h"
#include "Editor/Request/Request.h"
#include "Editor/Transition/Transition.h"

#include "Registry/Registry.h"
#include "Manifest/Manifest.h"

namespace NodeEditor
{
	enum NodeEditorStyleCol
	{
		NodeEditorStyleCol_StateNodeBackground,
		NodeEditorStyleCol_StateNodeBackgroundHovered,
		NodeEditorStyleCol_StateNodeBackgroundSelected,
		NodeEditorStyleCol_StateNodeTitleBar,
		NodeEditorStyleCol_StateNodeTitleBarHovered,
		NodeEditorStyleCol_StateNodeTitleBarSelected,
		NodeEditorStyleCol_StateNodeOutline,
		NodeEditorStyleCol_StateNodeOutlineHovered,
		NodeEditorStyleCol_StateNodeOutlineSelected,

		NodeEditorStyleCol_ControlParamNodeBackground,
		NodeEditorStyleCol_ControlParamNodeBackgroundHovered,
		NodeEditorStyleCol_ControlParamNodeBackgroundSelected,
		NodeEditorStyleCol_ControlParamNodeTitleBar,
		NodeEditorStyleCol_ControlParamNodeTitleBarHovered,
		NodeEditorStyleCol_ControlParamNodeTitleBarSelected,
		NodeEditorStyleCol_ControlParamNodeOutline,
		NodeEditorStyleCol_ControlParamNodeOutlineHovered,
		NodeEditorStyleCol_ControlParamNodeOutlineSelected,

		NodeEditorStyleCol_FloatDataPin,
		NodeEditorStyleCol_IntDataPin,
		NodeEditorStyleCol_UIntDataPin,
		NodeEditorStyleCol_BoolDataPin,
		NodeEditorStyleCol_Vector3DataPin,
		NodeEditorStyleCol_Vector4DataPin,
		NodeEditorStyleCol_QuaternionDataPin,

		NodeEditorStyleCol_NumStyleCols
	};

	enum NodeEditorFlags
	{
		NodeEditorFlags_None = 0,
		NodeEditorFlags_ReadOnly = 1 << 0
	};

	struct StyleSettings
	{
		float NodeMinContentHeight;
		float NodeMinWidth;
		float StateNodeCornerRounding;
		float StateNodeOutlineThickness;
		unsigned int Colors[NodeEditorStyleCol_NumStyleCols];

		StyleSettings();
	};

	class Editor
	{
	protected:
		int m_flags;
		StyleSettings m_styleSettings;
		Graph* m_rootGraph;
		std::stack<Graph*> m_graphStack;
		std::vector<ControlParameter*> m_controlParameters;
		std::vector<Message*> m_messages;
		ControlParametersNode* m_controlParametersNode;
		Registry* m_registry;
		Manifest::Manifest* m_manifest;
		bool m_showStyleEditor;

	public:
		Editor(int flags = 0);
		virtual ~Editor();

		virtual bool initialise();
		virtual void shutdown();
		virtual void update(float dt);
		virtual void reset();
		virtual void draw();
		virtual void handleUserInput();
		virtual void infoGui();
		virtual void navigatorGui();

		int getFlags() const { return m_flags; }
		void setFlags(int flags) { m_flags = flags; }

		StyleSettings& getStyleSettings() { return m_styleSettings; }
		Registry* getRegistry() const { return m_registry; }
		Manifest::Manifest* getManifest() const { return m_manifest; }

		ControlParameter* getControlParameter(int id) const;
		ControlParameter* getControlParameter(const std::string& name) const;
		ControlParameter* getControlParameterAtIndex(size_t index) const { return m_controlParameters[index]; }
		size_t getNumControlParameters() const { return m_controlParameters.size(); }
		bool removeControlParameter(ControlParameter* parameter);
		bool hasControlParameter(const std::string& name) const;

		Message* createMessage(int id, const std::string& name, const std::string& type);
		Message* createRequest(int id, const std::string& name);
		Message* getRequest(int id);
		Message* getRequest(const std::string& name) const;
		size_t getNumRequests() const { return m_messages.size(); }
		void addMessage(Message* request) { m_messages.push_back(request); }
		bool removeRequest(Message* request);
		bool hasRequest(int id) const;
		bool hasRequest(const std::string& name) const;

		void getAllNodes(std::vector<Node*>& outNodes) const;
		void getAllGraphs(std::vector<Graph*>& outGraphs) const;
		void getAllPins(std::vector<Pin*>& outPins) const;
		void getAllAttributes(std::vector<Attribute*>& outAttributes) const;
		void getAllTransitions(std::vector<Transition*>& outTransitions) const;

		Node* getNode(int nodeID) const;
		Node* getNode(const std::string& name) const;
		Node* findNodeByPath(const std::string& path) const;

		Graph* getGraph(int graphID) const;
		Graph* getGraph(const std::string& name) const;
		Graph* findGraphByPath(const std::string& path) const;

		Attribute* findAttributeByPath(const std::string& path) const;
		Pin* findPinByPath(const std::string& path) const;
		Transition* findTransitionByPath(const std::string& path) const;

		Transition* getTransitionBetweenNodes(Node* sourceNode, Node* destinationNode) const;

		Node* getSelectedNode() const;
		Transition* getSelectedTransition() const;
		void clearSelection();

		Node* getControlParametersNode() const { return m_controlParametersNode; }

		void addControlParameter(ControlParameter* parameter);
		ControlParameter* createControlParameterFloat(int id, const std::string& name);
		ControlParameter* createControlParameterInt(int id, const std::string& name);
		ControlParameter* createControlParameterUInt(int id, const std::string& name);
		ControlParameter* createControlParameterBool(int id, const std::string& name);
		ControlParameter* createControlParameterVector3(int id, const std::string& name);
		ControlParameter* createControlParameterVector4(int id, const std::string& name);
		ControlParameter* createControlParameterQuaternion(int id, const std::string& name);

		Graph* getRootGraph() const { return m_rootGraph; }
		Graph* getCurrentGraph() const { return m_graphStack.empty() ? nullptr : m_graphStack.top(); }

		BlendTree* createRootBlendTree(int rootNodeID = 0);
		StateMachine* createRootStateMachine(int rootNodeID = 0);

		void pushGraph(Graph* graph);
		void popGraph();

		void styleEditor();
	private:
		void initStyle();

		bool isNodeIDAvailable(int nodeID) const;

		ControlParameter* createControlParameter(int id, const std::string& name, ControlParameter::ParameterType parameterType);
	};
}