#pragma once
#include <stack>
#include "Graph/Graph.h"
#include "Node/ControlParametersNode.h"
#include "ControlParameter/ControlParameter.h"
#include "Registry/Registry.h"
#include "EditorProject/EditorProject.h"

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

	struct StyleSettings
	{
		float NodeMinContentHeight;
		float NodeMinWidth;
		float StateNodeCornerRounding;
		float StateNodeOutlineThickness;
		unsigned int Colors[NodeEditorStyleCol_NumStyleCols];

		StyleSettings();
	};

	class NodeEditor
	{
	protected:
		StyleSettings m_styleSettings;
		Graph* m_rootGraph;
		std::stack<Graph*> m_graphStack;
		std::vector<ControlParameter*> m_controlParameters;
		ControlParametersNode* m_controlParametersNode;
		Registry* m_registry;
		bool m_showStyleEditor;

	public:
		NodeEditor();
		virtual ~NodeEditor();

		virtual bool initialise();
		virtual void shutdown();
		virtual void update(float dt);
		virtual void reset();
		virtual void draw();
		virtual void handleUserInput();

		StyleSettings& getStyleSettings() { return m_styleSettings; }
		Registry* getRegistry() const { return m_registry; }

		ControlParameter* getControlParameter(const std::string& name) const;
		ControlParameter* getControlParameterAtIndex(size_t index) const { return m_controlParameters[index]; }
		size_t getNumControlParameters() const { return m_controlParameters.size(); }
		bool removeControlParameter(ControlParameter* parameter);
		bool hasControlParameter(const std::string& name) const;

		Node* getSelectedNode() const;
		Transition* getSelectedTransition() const;
		void clearSelection();

		Node* getControlParametersNode() const { return m_controlParametersNode; }

		void addControlParameter(ControlParameter* parameter);
		ControlParameter* createControlParameterFloat(const std::string& name);
		ControlParameter* createControlParameterInt(const std::string& name);
		ControlParameter* createControlParameterUInt(const std::string& name);
		ControlParameter* createControlParameterBool(const std::string& name);
		ControlParameter* createControlParameterVector3(const std::string& name);
		ControlParameter* createControlParameterVector4(const std::string& name);
		ControlParameter* createControlParameterQuaternion(const std::string& name);

		Graph* getRootGraph() const { return m_rootGraph; }
		Graph* getCurrentGraph() const { return m_graphStack.top(); }
		void pushGraph(Graph* graph);
		void popGraph();

		void styleEditor();

		bool loadProject(const std::string& filePath);
	private:
		void initStyle();

		ControlParameter* createControlParameter(const std::string& name, ControlParameter::ParameterType parameterType);
	};
}