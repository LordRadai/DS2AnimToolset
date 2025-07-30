#include "Pin.h"

namespace mcd
{
	void Pin::setReference(bool isReference)
	{
		removeAttribute(m_reference.get());

		if (isReference)
			addAttribute(m_reference.get());

		m_reference->setValue(isReference);
	}
}
