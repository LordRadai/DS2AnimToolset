#pragma once
#include <vector>
#include <string>
#include "NodeEditor/Editor/Attribute/Attribute.h"

namespace NodeEditor
{
	class Transition;

	class Condition : public Entity
	{
		Transition* m_owner;
		std::string m_typeName;
		std::vector<Attribute*> m_attributes;

	public:
		Condition(Transition* owner, const std::string& typeName, const std::string& name);

		virtual ~Condition() {}
		virtual bool editorGUI() override;
		virtual const std::string getFullName() const override;

		Transition* getOwnerTransition() const { return m_owner; }
		void setOwnerTransition(Transition* owner) { m_owner = owner; }

		const std::string& getTypeName() const { return m_typeName; }
		void setTypeName(const std::string& typeName) { m_typeName = typeName; }

		void addAttribute(Attribute* attribute) { m_attributes.push_back(attribute); }
		Attribute* getAttribute(size_t index) const;
		Attribute* getAttribute(const std::string& name) const;
		size_t getNumAttributes() const { return m_attributes.size(); }
	};
}