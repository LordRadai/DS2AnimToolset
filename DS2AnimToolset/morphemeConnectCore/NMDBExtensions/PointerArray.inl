#pragma once
#include "Pointer.inl"

namespace db
{
	template<typename T>
	class PointerArray : public db::PointerArrayAttribute
	{
	public:
		PointerArray(db::Attribute* parent, const std::string& name)
			: db::PointerArrayAttribute(parent, name)
		{}

		void addElement(T* value)
		{
			auto ptrAttr = new db::Pointer<T>(this, "elem", value);
			this->addAttribute(ptrAttr);
		}

		void removeElement(int index)
		{
			auto attr = this->getAttribute(index);
			this->removeAttribute(attr);
		}

		T* getElement(int index) const
		{
			auto attr = this->getAttribute(index);
			auto ptrAttr = dynamic_cast<db::Pointer<T>*>(attr);
			if (ptrAttr)
				return ptrAttr->getValue();
			return nullptr;
		}

		uint32_t size() const
		{
			return this->getNumAttributes();
		}
	};
}
