#pragma once
#include <vector>
#include <string>
#include "NodeEditor/Attribute/Attribute.h"

namespace NodeEditor
{
	class Transition;

	class Condition
	{
		Transition* m_owner;
		std::string m_typeName;
		std::string m_name;
		std::vector<Attribute*> m_attributes;

	public:
		Condition(Transition* owner, const std::string& typeName, const std::string& name) : m_owner(owner), m_typeName(typeName), m_name(name) {}

		virtual ~Condition() {}
		virtual void editorGUI();

		Transition* getOwnerTransition() const { return m_owner; }
		void setOwnerTransition(Transition* owner) { m_owner = owner; }

		const std::string& getTypeName() const { return m_typeName; }
		void setTypeName(const std::string& typeName) { m_typeName = typeName; }

		const std::string& getName() const { return m_name; }
		void setName(const std::string& name) { m_name = name; }

		void addAttribute(Attribute* attribute) { m_attributes.push_back(attribute); }
		Attribute* getAttribute(size_t index) const;
		Attribute* getAttribute(const std::string& name) const;
		size_t getNumAttributes() const { return m_attributes.size(); }
	};
}