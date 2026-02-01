#pragma once
#include "ManifestItem/Node/MMNode.h"
#include "ManifestItem/Node/MMStateMachine.h"
#include "ManifestItem/Node/MMStateMachineNode.h"
#include "ManifestItem/Condition/MMCondition.h"
#include "ManifestItem/Transition/MMTransition.h"

namespace NodeEditor
{
	namespace Manifest
	{
		class Manifest
		{
			std::vector<MMStateMachine*> m_registeredStateMachines;
			std::vector<MMStateMachineNode*> m_registeredStateMachineNodes;
			std::vector<MMNode*> m_registeredNodes;
			std::vector<MMCondition*> m_registeredConditions;
			std::vector<MMTransition*> m_regosteredTransitions;

		public:
			Manifest() = default;

			bool init();
			void shutdown();

			void registerStateMachine(std::string manifestPath);
			void unregisterStateMachine(uint32_t index);

			void registerStateMachineNode(std::string manifestPath);
			void unregisterStateMachineNode(uint32_t index);

			void registerNode(std::string manifestPath);
			void unregisterNode(uint32_t index);

			void registerCondition(std::string manifestPath);
			void unregisterCondition(uint32_t index);

			void registerTransition(std::string manifestPath);
			void unregisterTransition(uint32_t index);

			MMStateMachine* getStateMachineManifest(uint32_t index);
			MMStateMachine* findStateMachineManifest(uint32_t id);
			MMStateMachine* findStateMachineManifest(const std::string& name);

			MMStateMachineNode* getStateMachineNodeManifest(uint32_t index);
			MMStateMachineNode* findStateMachineNodeManifest(uint32_t id);
			MMStateMachineNode* findStateMachineNodeManifest(const std::string& name);

			MMNode* getNodeManifest(uint32_t index);
			MMNode* findNodeManifest(uint32_t id);
			MMNode* findNodeManifest(const std::string& name);

			MMCondition* getConditionManifest(uint32_t index);
			MMCondition* findConditionManifest(uint32_t id);
			MMCondition* findConditionManifest(const std::string& name);

			MMTransition* getTransitionManifest(uint32_t index);
			MMTransition* findTransitionManifest(uint32_t id);
			MMTransition* findTransitionManifest(const std::string& name);

			uint32_t getNumRegisteredStateMachines() const { return static_cast<uint32_t>(m_registeredStateMachines.size()); }
			uint32_t getNumRegisteredStateMachineNodes() const { return static_cast<uint32_t>(m_registeredStateMachineNodes.size()); }
			uint32_t getNumRegisteredNodes() const { return static_cast<uint32_t>(m_registeredNodes.size()); }
			uint32_t getNumRegisteredConditions() const { return static_cast<uint32_t>(m_registeredConditions.size()); }
			uint32_t getNumRegisteredTransitions() const { return static_cast<uint32_t>(m_regosteredTransitions.size()); }

			void printRegisteredManifests();
		};
	}
}