#pragma once
#include "NodeEditor/Entity/Entity.h"
#include "NodeEditor/Pin/DataPin.h"
#include "NodeEditor/EditorProject/ControlParameter/ProjectControlParameter.h"

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
		int m_controlParameterID;
		DataPin* m_output;
		ParameterType m_parameterType;
	public:
		ControlParameter(NodeEditor* editor, int id, const std::string& name, ParameterType parameterType);
		ControlParameter(NodeEditor* editor, Project::ProjectControlParameter* projectParameter);

		virtual ~ControlParameter() override {}

		virtual void draw() override {}

		DataPin* getOutputPin() const { return m_output; }
		ParameterType getParameterType() const { return m_parameterType; }

		static const char* parameterTypeToString(ParameterType type);
		static ParameterType stringToParameterType(const std::string& typeStr);
	};
}
