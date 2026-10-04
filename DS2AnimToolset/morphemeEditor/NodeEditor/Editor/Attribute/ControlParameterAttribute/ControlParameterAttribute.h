#pragma once
#include "../Attribute.h"
#include "NodeEditor/Editor/ControlParameter/ControlParameter.h"

namespace NodeEditor
{
	class ControlParameterAttribute : public Attribute
	{
		ControlParameter* m_controlParameter;
	public:
		ControlParameterAttribute(Entity* owner, const std::string& name)
			: Attribute(owner, name, "controlParameter"), m_controlParameter(nullptr) {}

		virtual ~ControlParameterAttribute() override {}
		virtual bool editorGUI() override;
		virtual void setValue(const std::vector<std::any>& values) override;

		ControlParameter* getControlParameter() const { return m_controlParameter; }
	};
}
