#pragma once
#include "NMDatabase/NMDatabase.h"
#include "mcd/Attribute/Attribute.h"
#include "mcd/Condition/Condition.h"
#include "NMDBExtensions/TypedNodeContainer.inl"
#include "NMDBExtensions/Pointer.inl"

namespace mcd
{
	class StateMachineNode;

	class TransitionEdge : public db::Node
	{
		std::unique_ptr<db::TypedNodeContainer<mcd::Condition>> m_conditions;
		std::unique_ptr<db::TypedNodeContainer<mcd::Attribute>> m_attributes;
		std::unique_ptr<db::Pointer<mcd::StateMachineNode>> m_sourceNode;
		std::unique_ptr<db::Pointer<mcd::StateMachineNode>> m_targetNode;
		std::unique_ptr<db::StringAttribute> m_edgeType;
		std::unique_ptr<db::IntAttribute> m_manifestVersion;

	public:
		TransitionEdge(db::Node* parent, const std::string& name, mcd::StateMachineNode* sourceNode, mcd::StateMachineNode* targetNode, const std::string& transitType, const int manifestVersion);

		virtual ~TransitionEdge() override {};

		mcd::StateMachineNode* getSourceNode() const { return m_sourceNode->get(); }
		mcd::StateMachineNode* getTargetNode() const { return m_targetNode->get(); }
		const std::string& getEdgeType() const { return m_edgeType->getValue(); }
		int getManifestVersion() const { return m_manifestVersion->getValue(); }

		void setEdgeType(const std::string& type) { m_edgeType->setValue(type); }
		void setManifestVersion(int version) { m_manifestVersion->setValue(version); }

		void addCondition(mcd::Condition* condition) { m_conditions->add(condition); }
		mcd::Condition* getCondition(uint32_t index) const { return m_conditions->getNode(index); }
		mcd::Condition* findCondition(const std::string& name) const { return m_conditions->find(name); }
		uint32_t getConditionCount() const { return m_conditions->size(); }

		void addAttribute(mcd::Attribute* attribute) { m_attributes->add(attribute); }
		mcd::Attribute* getAttribute(uint32_t index) const { return m_attributes->getNode(index); }
		mcd::Attribute* findAttribute(const std::string& name) const { return m_attributes->find(name); }
		uint32_t getAttributeCount() const { return m_attributes->size(); }
	};
}
