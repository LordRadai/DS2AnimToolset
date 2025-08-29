#pragma once
#include "NMDBExtensions/Pointer.inl"
#include "mcd/Attribute/SingleValue/StringAttribute.h"
#include "mcd/Attribute/SingleValue/BoolAttribute.h"
#include "mcd/Attribute/SingleValue/IntAttribute.h"

namespace mcd
{
	class RefAttribute : public Attribute
	{
		db::Pointer<db::Node> m_value;
		StringAttribute* m_refStr;
		IntAttribute* m_refKind;
		BoolAttribute* m_isWeakRef;
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

		RefAttribute(db::Attribute* parent, const std::string& name, db::Node* value, int refKind, bool isWeakRef);
		virtual ~RefAttribute() override {}
		virtual bool isValueEqualTo(Attribute* attr) override;

		void setRef(db::Node* value);
		void setRefByPath(const std::string& path);
		db::Node* getRef() const { return m_value.getValue(); }

		void setWeak(bool isWeakRef);
		bool isWeak() const { return m_isWeakRef->getValue(); }

		void setRefKind(int refKind);
		int getRefKind() const { return m_refKind->getValue(); }

		std::string getRefKindString() const { return m_refStr->getValue(); }

		static const char* refKindAsString(int refKind);
		static int stringAsRefKind(const char* refKindStr);
	};
}
