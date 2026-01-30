#pragma once
#include <stack>
#include "Graph/Graph.h"
#include "ControlParametersNode/ControlParametersNode.h"
#include "ControlParameter/ControlParameter.h"
#include "Registry/Registry.h"

namespace NodeEditor
{
	struct StyleSettings
	{
		float NodeMinContentHeight;
		float NodeMinWidth;
		float NodePinSpacing;
		unsigned int ControlParametersNodeBackground;
		unsigned int ControlParametersNodeBackgroundHovered;
		unsigned int ControlParametersNodeBackgroundSelected;
		unsigned int ControlParametersNodeTitleBar;
		unsigned int ControlParametersNodeTitleBarHovered;
		unsigned int ControlParametersNodeTitleBarSelected;

		StyleSettings();
	};

	class NodeEditor
	{
	protected:
		StyleSettings m_styleSettings;
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
		virtual void draw();

		StyleSettings& getStyleSettings() { return m_styleSettings; }
		Registry* getRegistry() const { return m_registry; }

		ControlParameter* getControlParameter(const std::string& name) const;
		ControlParameter* getControlParameterAtIndex(size_t index) const { return m_controlParameters[index]; }
		size_t getNumControlParameters() const { return m_controlParameters.size(); }
		bool removeControlParameter(ControlParameter* parameter);
		bool hasControlParameter(const std::string& name) const;

		Node* getControlParametersNode() const { return m_controlParametersNode; }

		ControlParameter* createControlParameterFloat(const std::string& name);
		ControlParameter* createControlParameterInt(const std::string& name);
		ControlParameter* createControlParameterUInt(const std::string& name);
		ControlParameter* createControlParameterBool(const std::string& name);
		ControlParameter* createControlParameterVector3(const std::string& name);
		ControlParameter* createControlParameterVector4(const std::string& name);
		ControlParameter* createControlParameterQuaternion(const std::string& name);

		Graph* getCurrentGraph() const { return m_graphStack.top(); }
		void pushGraph(Graph* graph) { m_graphStack.push(graph); }
		void popGraph();

		void styleEditor();
	private:
		void initStyle();

		ControlParameter* createControlParameter(const std::string& name, ControlParameter::ParameterType parameterType);
	};
}