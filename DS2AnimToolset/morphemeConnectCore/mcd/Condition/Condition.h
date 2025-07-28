#pragma once
#include "NMDatabase/NMDatabase.h"
#include "mcd/Attribute/Attribute.h"
#include "NMDBExtensions/TypedNodeContainer.inl"

namespace mcd
{
	class Condition : public db::Node
	{
		std::unique_ptr<db::TypedNodeContainer<mcd::Attribute>> m_attributes;
		std::unique_ptr<db::StringAttribute> m_conditionType;
		std::unique_ptr<db::IntAttribute> m_manifestVersion;

	public:
		Condition(db::Node* parent, const std::string& name, const std::string& type, const int manifestVersion);

		virtual ~Condition() override {};

		const std::string& getConditionType() const { return m_conditionType->getValue(); }
		int getManifestVersion() const { return m_manifestVersion->getValue(); }

		void setConditionType(const std::string& type) { m_conditionType->setValue(type); }
		void setManifestVersion(int version) { m_manifestVersion->setValue(version); }

		void addAttribute(mcd::Attribute* attribute) { m_attributes->add(attribute); }
		mcd::Attribute* getAttribute(uint32_t index) const { return m_attributes->getNode(index); }
		mcd::Attribute* findAttribute(const std::string& name) const { return m_attributes->find(name); }
		uint32_t getAttributeCount() const { return m_attributes->size(); }
	};
}
