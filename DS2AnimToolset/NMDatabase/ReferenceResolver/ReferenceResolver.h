#pragma once
#include "Attribute/SingleValue/PointerAttribute.h"

namespace db
{
	class ReferenceResolver
	{
		struct PathReference
		{
			std::string path;
			PointerAttribute* attribute;

			PathReference(const std::string& p, PointerAttribute* attr) : path(p), attribute(attr) {}
		};

		struct RuntimeIDReference
		{
			uint32_t runtimeID;
			PointerAttribute* attribute;

			RuntimeIDReference(uint32_t id, PointerAttribute* attr) : runtimeID(id), attribute(attr) {}
		};

		std::vector<PathReference*> m_unresolvedPathReferences;
		std::vector<RuntimeIDReference*> m_unresolvedRuntimeIDReferences;
	public:

		/**
		 * \brief Adds a path reference to be resolved.
		 * 
		 * \param attribute The PointerAttribute that holds the unresolved reference.
		 * \param path The path string that needs to be resolved.
		 */
		void addUnresolvedReference(PointerAttribute* attribute, const std::string path) { m_unresolvedPathReferences.push_back(new PathReference(path, attribute)); }

		/**
		 * \brief Adds a runtime ID reference to be resolved.
		 * 
		 * \param attribute The PointerAttribute that holds the unresolved reference.
		 * \param runtimeID The runtime ID that needs to be resolved.
		 */
		void addUnresolvedReference(PointerAttribute* attribute, const uint32_t runtimeID) { m_unresolvedRuntimeIDReferences.push_back(new RuntimeIDReference(runtimeID, attribute)); }

		/**
		 * \brief Resolves all unresolved path references using the given root attribute.
		 * 
		 * \param root The root attribute to start resolving from.
		 * \return True if all references were resolved successfully, false otherwise.
		 */
		bool resolvePathReferences(db::Attribute* root);

		/**
		 * \brief Resolves all unresolved runtime ID references using the given root attribute.
		 * 
		 * \param root The root attribute to start resolving from.
		 * \return True if all references were resolved successfully, false otherwise.
		 */
		bool resolveRuntimeIDReferences(db::Attribute* root);
	};
}