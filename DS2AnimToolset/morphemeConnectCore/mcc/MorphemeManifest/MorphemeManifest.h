#pragma once
#include "MMManifestItem/Node/MMNode.h"
#include "MMManifestItem/Condition/MMCondition.h"
#include "MMManifestItem/Transition/MMTransition.h"
#include "MMManifestItem/StateMachine/MMStateMachine.h"

namespace mcc
{
	class MorphemeManifest
	{
		std::vector<MMStateMachine*> m_registeredStateMachines;
		std::vector<MMNode*> m_registeredNodes;
		std::vector<MMCondition*> m_conditions;
		std::vector<MMTransition*> m_transitions;

	public:
		MorphemeManifest() = default;

		void shutdown();

		void registerStateMachine(std::string manifestPath);
		void unregisterStateMachine(uint32_t index);

		void registerNode(std::string manifestPath);
		void unregisterNode(uint32_t index);

		void registerCondition(std::string manifestPath);
		void unregisterCondition(uint32_t index);

		void registerTransition(std::string manifestPath);
		void unregisterTransition(uint32_t index);

		MMStateMachine* getStateMachineManifest(uint32_t index);
		MMStateMachine* findStateMachineManifest(uint32_t id);

		MMNode* getNodeManifest(uint32_t index);
		MMNode* findNodeManifest(uint32_t id);

		MMCondition* getConditionManifest(uint32_t index);
		MMCondition* findConditionManifest(uint32_t id);

		MMTransition* getTransitionManifest(uint32_t index);
		MMTransition* findTransitionManifest(uint32_t id);
	};
}
