#pragma once
#include "NodeEditor/Entity/Entity.h"
#include "NodeEditor/Pin/DataPin.h"

namespace NodeEditor
{
	class ControlParameter : public Entity
	{
	public:
		enum ParameterType
		{
			kParameterTypeFloat,
			kParameterTypeInt,
			kParameterTypeUInt,
			kParameterTypeBool,
			kParameterTypeVector3,
			kParameterTypeVector4,
			kParameterTypeQuaternion,

			kNumParameterTypes
		};
	private:
		DataPin* m_output;
		ParameterType m_parameterType;
	public:
		ControlParameter(NodeEditorBase* editor, const std::string& name, ParameterType parameterType);

		virtual ~ControlParameter() override {}

		virtual void draw() override {}

		DataPin* getOutputPin() const { return m_output; }
		ParameterType getParameterType() const { return m_parameterType; }
	};
}
