#include "RefAttribute.h"

namespace mcd
{
    RefAttribute::RefAttribute(db::Attribute* parent, const std::string& name, db::Node* value, int refKind, bool isWeakRef) :
        Attribute(parent, name, "ref"),
        m_value(this, "value", value),
        m_refStr(new StringAttribute(this, "RefKindStr", refKindAsString(refKind))),
        m_refKind(new IntAttribute(this, "RefKind", refKind)),
        m_isWeakRef(new BoolAttribute(this, "IsWeakRef", isWeakRef))
    {
        this->addAttribute(&m_value);
        setWeak(isWeakRef);

	}

    bool RefAttribute::isValueEqualTo(Attribute* attrib)
    {
        if (!attrib->isOfType<RefAttribute>())
            return false;

        RefAttribute* otherPtrAttrib = dynamic_cast<RefAttribute*>(attrib);

        if (otherPtrAttrib->m_value.getValue() == this->m_value.getValue())
            return true;

        return false;
    }

    void RefAttribute::setRef(db::Node* node)
    {
        m_value.setValue(node);
    }

    void RefAttribute::setRefByPath(const std::string& path)
    {

    }

    void RefAttribute::setWeak(bool weak)
    {
        removeAttribute(m_isWeakRef);

        m_isWeakRef->setValue(weak);

        if (weak)
            addAttribute(m_isWeakRef);
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
        case kRefKindTransitionSubState:
            return "transitionSubState";
        case kRefKindConditionSubState:
            return "conditionSubState";
        case kRefKindAllStateMachines:
            return "allStateMachines";
        case kRefKindAnimationSet:
            return "AnimationSet";
        case kRefKindMessagePreset:
            return "messagePreset";
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
