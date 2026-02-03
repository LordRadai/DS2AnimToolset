#pragma once
#include "../Attribute.h"

namespace NodeEditor
{
	class EnumAttribute : public Attribute
	{
		int m_value;
		std::vector<std::string> m_enumOptions;
	public:
		EnumAttribute(Entity* owner, const std::string& name) : Attribute(owner, name, "enum"), m_value(0) {}

		virtual ~EnumAttribute() override {}
		virtual bool editorGUI() override;
		virtual void setValue(const std::vector<std::any>& values) override;

		int getValue() const { return m_value; }
		std::string getValueAsString() const;
		int getValueForString(const std::string& valueStr) const;

		int getNumOptions() const { return static_cast<int>(m_enumOptions.size()); }
		void setEnumOptions(const std::vector<std::string>& options) { m_enumOptions = options; }
		void addEnumOption(const std::string& option) { m_enumOptions.push_back(option); }
	};
}
