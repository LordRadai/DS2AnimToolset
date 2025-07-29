#pragma once
#include "MMManifestItem/Node/MMNode.h"
#include "MMManifestItem/Condition/MMCondition.h"
#include "MMManifestItem/Transition/MMTransition.h"

namespace mcc
{
	class MorphemeManifest
	{
		std::vector<MMNode> m_registeredNodes;
		std::vector<MMCondition> m_conditions;
		std::vector<MMTransition> m_transitions;

	public:
		MorphemeManifest() = default;

		void shutdown();

		void registerNode(std::string manifestPath);
		void unregisterNode(uint32_t index);

		void registerCondition(std::string manifestPath);
		void unregisterCondition(uint32_t index);

		void registerTransition(std::string manifestPath);
		void unregisterTransition(uint32_t index);

		MMNode* getNodeManifest(uint32_t index);
		MMNode* findNodeManifest(uint32_t id);

		MMCondition* getConditionManifest(uint32_t index);
		MMCondition* findConditionManifest(const std::string& name);

		MMTransition* getTransitionManifest(uint32_t index);
		MMTransition* findTransitionManifest(const std::string& name);
	};
}
