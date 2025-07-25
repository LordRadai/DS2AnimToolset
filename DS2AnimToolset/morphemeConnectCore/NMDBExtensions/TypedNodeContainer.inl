#pragma once
#include "NMDatabase/NodeContainer/NodeContainer.h"

namespace db
{
	template<typename T>
	class TypedNodeContainer : public db::NodeContainer
	{
		static_assert(std::is_base_of<db::Node, T>::value, "T must be derived from db::Node");
	public:
		TypedNodeContainer(db::Node* parent, const std::string& identifier) :
			db::NodeContainer(parent, identifier) {}

		virtual ~TypedNodeContainer() override {}

		void add(T* node) { db::NodeContainer::add(node); }
		void remove(T* attribute) { db::NodeContainer::remove(attribute); }

		T* getNode(uint32_t idx) const
		{
			return dynamic_cast<T*>(db::NodeContainer::getNode(idx));
		}

		T* find(const std::string& name) const
		{
			return dynamic_cast<T*>(db::NodeContainer::find(name));
		}

		bool hasNode(T* node) const
		{
			return db::NodeContainer::hasNode(node);
		}

		T* front() const
		{
			return dynamic_cast<T*>(db::NodeContainer::front());
		}

		T* back() const
		{
			return dynamic_cast<T*>(db::NodeContainer::back());
		}

		T* operator[](uint32_t idx) const
		{
			return getNode(idx);
		}
	};
}