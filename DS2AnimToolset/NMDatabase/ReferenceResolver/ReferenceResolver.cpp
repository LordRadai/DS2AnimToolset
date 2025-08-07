#include "ReferenceResolver.h"

namespace db
{
	bool ReferenceResolver::resolvePathReferences(db::Attribute* root)
	{
		if (!root)
			return false;

		for (size_t i = 0; i < m_unresolvedPathReferences.size(); i++)
		{
			PointerAttribute* attribute = m_unresolvedPathReferences[i]->attribute;
			const std::string& path = m_unresolvedPathReferences[i]->path;

			if (attribute == nullptr) throw std::runtime_error("ReferenceResolver::resolvePathReferences: Unresolved pointer attribute is null.");

			Attribute* referencedAttr = root->attributeFromDatabasePath(path, attribute);

			if (referencedAttr == nullptr)
				throw std::runtime_error("ReferenceResolver::resolvePathReferences: Failed to resolve path reference '" + path + "' for attribute '" + attribute->getName() + "'.");

			attribute->setValue(referencedAttr);
		}

		return true;
	}

	bool ReferenceResolver::resolveRuntimeIDReferences(db::Attribute* root)
	{
		if (!root)
			return false;

		// TODO: Implement.
		return false;
	}
}
