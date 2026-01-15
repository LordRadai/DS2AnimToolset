#include "RefAttribute.h"

namespace mcd
{
    RefAttribute::RefAttribute(db::Attribute* parent, const std::string& name, int refKind, bool isWeakRef) :
        Attribute(parent, name, "RefAttribute"),
        m_ref(std::make_unique<db::Pointer<db::Node>>(this, "Ref", nullptr)),
        m_refStr(std::make_unique<db::StringAttribute>(this, "RefStr", "")),
        m_refKind(std::make_unique<db::IntAttribute>(this, "RefKind", refKind)),
        m_isWeakRef(std::make_unique<db::BoolAttribute>(this, "IsWeakRef", isWeakRef))
    {
        addAttribute(m_refKind.get());
		setWeak(isWeakRef);
	}

    bool RefAttribute::isValueEqualTo(Attribute* attrib)
    {
        if (!attrib->isOfType<RefAttribute>())
            return false;

        RefAttribute* otherPtrAttrib = dynamic_cast<RefAttribute*>(attrib);

        if (otherPtrAttrib->m_ref->getValue() == this->m_ref->getValue())
            return true;

        return false;
    }

    void RefAttribute::setRef(db::Node* node)
    {
        m_ref->setValue(node);
    }

    void RefAttribute::setRefByPath(const std::string& path)
    {
		throw std::runtime_error("RefAttribute::setRefByPath() - Not implemented");
    }

    void RefAttribute::setWeak(bool weak)
    {
        removeAttribute(m_isWeakRef.get());

        m_isWeakRef->setValue(weak);

        if (weak)
            addAttribute(m_isWeakRef.get());
    }

    void RefAttribute::setRefKind(int refKind)
    {
        m_refKind->setValue(refKind);
        m_refStr->setValue(refKindAsString(refKind));
    }

	const char* RefAttribute::refKindAsString(int refKind)
	{
        switch (refKind)
        {
        case kRefKindTransitionSubState:    return "transitionSubState";
        case kRefKindConditionSubState:     return "conditionSubState";
        case kRefKindAllStateMachines:      return "allStateMachines";
        case kRefKindAnimationSet:          return "AnimationSet";
        case kRefKindMessagePreset:         return "messagePreset";
        default:
            return "";
        }
	}

    int RefAttribute::stringAsRefKind(const char* refKindStr)
    {
        if (strcmp(refKindStr, "transitionSubState") == 0)
			return kRefKindTransitionSubState;
        else if (strcmp(refKindStr, "conditionSubState") == 0)
            return kRefKindConditionSubState;
		else if (strcmp(refKindStr, "allStateMachines") == 0)
            return kRefKindAllStateMachines;
        else if (strcmp(refKindStr, "AnimationSet") == 0)
            return kRefKindAnimationSet;
        else if (strcmp(refKindStr, "messagePreset") == 0)
            return kRefKindMessagePreset;
        else
			return kRefKindNone;
    }
}
