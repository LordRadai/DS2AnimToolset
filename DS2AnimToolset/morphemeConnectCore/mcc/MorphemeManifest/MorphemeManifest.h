#pragma once
#include "MMManifestItem/MMNode.h"

namespace mcc
{
	class MorphemeManifest
	{
		std::vector<MMNode> m_registeredNodes;

	public:
		MorphemeManifest() = default;

		void shutdown();

		void registerNode(std::string manifestPath);
		void unregisterNode(uint32_t index);

		MMNode* getManifest(uint32_t index);
		MMNode* findManifest(uint32_t id);
	};
}
