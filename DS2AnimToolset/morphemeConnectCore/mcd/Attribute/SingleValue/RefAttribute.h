#pragma once
#include "NMDBExtensions/Pointer.inl"
#include "mcd/Attribute/SingleValue/StringAttribute.h"
#include "mcd/Attribute/SingleValue/BoolAttribute.h"
#include "mcd/Attribute/SingleValue/IntAttribute.h"

namespace mcd
{
	class RefAttribute : public Attribute
	{
		std::unique_ptr<db::Pointer<db::Node>> m_ref;
		std::unique_ptr<db::StringAttribute> m_refStr;
		std::unique_ptr<db::IntAttribute> m_refKind;
		std::unique_ptr<db::BoolAttribute> m_isWeakRef;
	public:
		enum RefKind
		{
			kRefKindNone,
			kRefKindTransitionSubState,
			kRefKindConditionSubState,
			kRefKindAllStateMachines,
			kRefKindAnimationSet,
			kRefKindMessagePreset,
			kNumRefKinds
		};

		RefAttribute(db::Attribute* parent, const std::string& name, int refKind, bool isWeakRef);
		virtual ~RefAttribute() override {}
		virtual bool isValueEqualTo(Attribute* attr) override;

		void setRef(db::Node* value);
		void setRefByPath(const std::string& path);
		db::Node* getRef() const { return m_ref->getValue(); }

		void setWeak(bool isWeakRef);
		bool isWeak() const { return m_isWeakRef->getValue(); }

		void setRefKind(int refKind);
		int getRefKind() const { return m_refKind->getValue(); }

		std::string getRefKindString() const { return m_refStr->getValue(); }

		static const char* refKindAsString(int refKind);
		static int stringAsRefKind(const char* refKindStr);
	};
}
