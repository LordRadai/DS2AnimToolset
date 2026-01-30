#pragma once
#include <string>

namespace NodeEditor
{
	class Entity;

	class Attribute
	{
	public:
		enum AttributeType
		{
			kAttribTypeBool,
			kAttribTypeInt,
			kAttribTypeFloat,
			kAttribTypeVector3,
			kAttribTypeVector4,
			kAttribTypeQuaternion,
			kNumAtributeTypes
		};

	private:
		Entity* m_owner;
		std::string m_name;
		AttributeType m_type;

	public:
		Attribute(Entity* owner, const std::string& name, AttributeType type) : m_owner(owner), m_name(name), m_type(type) {}
		virtual ~Attribute() {}
		virtual void editorGUI();

		const std::string& getName() const { return m_name; }
		AttributeType getType() const { return m_type; }
	};
}
