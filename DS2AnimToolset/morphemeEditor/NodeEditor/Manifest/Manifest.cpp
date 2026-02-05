#include "Manifest.h"
#include <filesystem>
#include <fstream>

#include "RLog/RLog.h"
#include "extern.h"

namespace NodeEditor
{
	namespace Manifest
	{
		void Manifest::registerStateMachine(std::string manifestPath)
		{
			if (!std::filesystem::exists(manifestPath))
				INVOKE_PANIC("Manifest file does not exist: " + manifestPath);

			std::ifstream file(manifestPath);
			nlohmann::json jsonData = nlohmann::json::parse(file);

			file.close();

			if (!jsonData.is_object())
				INVOKE_PANIC("Invalid JSON format for Manifest state machine registration");

			MMStateMachine* stateMachine = new MMStateMachine(jsonData);
			stateMachine->setName(std::filesystem::path(manifestPath).filename().replace_extension("").string());

			m_registeredStateMachines.push_back(stateMachine);

			g_appLog->debugMessage(MsgLevel_Info, "Registered state machine manifest: %s\n", stateMachine->getName().c_str());
		}

		void Manifest::unregisterStateMachine(uint32_t index)
		{
			if (index >= m_registeredStateMachines.size())
				INVOKE_PANIC("Index out of range for registered state machines");

			delete m_registeredStateMachines[index];
			m_registeredStateMachines.erase(m_registeredStateMachines.begin() + index);
		}

		void Manifest::registerStateMachineNode(std::string manifestPath)
		{
			if (!std::filesystem::exists(manifestPath))
				INVOKE_PANIC("Manifest file does not exist: " + manifestPath);

			std::ifstream file(manifestPath);
			nlohmann::json jsonData = nlohmann::json::parse(file);

			file.close();

			if (!jsonData.is_object())
				INVOKE_PANIC("Invalid JSON format for Manifest state machine node registration");

			MMStateMachineNode* stateMachineNode = new MMStateMachineNode(jsonData);
			stateMachineNode->setName(std::filesystem::path(manifestPath).filename().replace_extension("").string());

			m_registeredStateMachineNodes.push_back(stateMachineNode);

			g_appLog->debugMessage(MsgLevel_Info, "Registered state machine node manifest: %s\n", stateMachineNode->getName().c_str());
		}

		void Manifest::unregisterStateMachineNode(uint32_t index)
		{
			if (index >= m_registeredStateMachineNodes.size())
				INVOKE_PANIC("Index out of range for registered state machine nodes");

			delete m_registeredStateMachineNodes[index];

			m_registeredStateMachineNodes.erase(m_registeredStateMachineNodes.begin() + index);
		}

		void Manifest::registerNode(std::string manifestPath)
		{
			if (!std::filesystem::exists(manifestPath))
				INVOKE_PANIC("Manifest file does not exist: " + manifestPath);

			std::ifstream file(manifestPath);
			nlohmann::json jsonData = nlohmann::json::parse(file);

			file.close();

			if (!jsonData.is_object())
				INVOKE_PANIC("Invalid JSON format for Manifest node registration");

			MMNode* node = new MMNode(jsonData);
			node->setName(std::filesystem::path(manifestPath).filename().replace_extension("").string());

			m_registeredNodes.push_back(node);

			g_appLog->debugMessage(MsgLevel_Info, "Registered node manifest: %s\n", node->getName().c_str());
		}

		void Manifest::unregisterNode(uint32_t index)
		{
			if (index >= m_registeredNodes.size())
				INVOKE_PANIC("Index out of range for registered nodes");

			delete m_registeredNodes[index];
			m_registeredNodes.erase(m_registeredNodes.begin() + index);
		}

		void Manifest::registerCondition(std::string manifestPath)
		{
			if (!std::filesystem::exists(manifestPath))
				INVOKE_PANIC("Manifest file does not exist: " + manifestPath);

			std::ifstream file(manifestPath);
			nlohmann::json jsonData = nlohmann::json::parse(file);

			file.close();

			if (!jsonData.is_object())
				INVOKE_PANIC("Invalid JSON format for Manifest condition registration");

			MMCondition* condition = new MMCondition(jsonData);
			condition->setName(std::filesystem::path(manifestPath).filename().replace_extension("").string());

			m_registeredConditions.push_back(condition);

			g_appLog->debugMessage(MsgLevel_Info, "Registered condition manifest: %s\n", condition->getName().c_str());
		}

		void Manifest::unregisterCondition(uint32_t index)
		{
			if (index >= m_registeredConditions.size())
				INVOKE_PANIC("Index out of range for registered conditions");

			delete m_registeredConditions[index];
			m_registeredConditions.erase(m_registeredConditions.begin() + index);
		}

		void Manifest::registerTransition(std::string manifestPath)
		{
			if (!std::filesystem::exists(manifestPath))
				INVOKE_PANIC("Manifest file does not exist: " + manifestPath);

			std::ifstream file(manifestPath);

			nlohmann::json jsonData = nlohmann::json::parse(file);
			file.close();
			if (!jsonData.is_object())
				INVOKE_PANIC("Invalid JSON format for Manifest transition registration");

			MMTransition* transition = new MMTransition(jsonData);
			transition->setName(std::filesystem::path(manifestPath).filename().replace_extension("").string());

			m_regosteredTransitions.push_back(transition);

			g_appLog->debugMessage(MsgLevel_Info, "Registered transition manifest: %s\n", transition->getName().c_str());
		}

		void Manifest::unregisterTransition(uint32_t index)
		{
			if (index >= m_regosteredTransitions.size())
				INVOKE_PANIC("Index out of range for registered transitions");

			delete m_regosteredTransitions[index];
			m_regosteredTransitions.erase(m_regosteredTransitions.begin() + index);
		}

		bool Manifest::init()
		{
			if (!std::filesystem::exists("Data\\manifest\\"))
			{
				INVOKE_PANIC("Manifest directory does not exist: Data\\manifest\\");
				return false;
			}

			if (!std::filesystem::exists("Data\\manifest\\nodes\\"))
			{
				INVOKE_PANIC("Manifest node directory does not exist: Data\\manifest\\nodes\\");
				return false;
			}

			registerStateMachine("Data\\manifest\\nodes\\animation\\StateMachine.json");
			registerStateMachineNode("Data\\manifest\\nodes\\animation\\ActiveState.json");

			for (const auto& entry : std::filesystem::recursive_directory_iterator("Data\\manifest\\nodes\\"))
			{
				if (entry.is_regular_file() && entry.path().extension() == ".json" &&
					entry.path().filename().replace_extension("") != "StateMachine.json" &&
					entry.path().filename().replace_extension("") != "PhysicsStateMachine.json")
				{
					std::string manifestPath = entry.path().string();
					registerNode(manifestPath);
				}
			}

			if (!std::filesystem::exists("Data\\manifest\\conditions\\"))
			{
				INVOKE_PANIC("Manifest condition directory does not exist: Data\\manifest\\conditions\\");
				return false;
			}

			for (const auto& entry : std::filesystem::recursive_directory_iterator("Data\\manifest\\conditions\\"))
			{
				if (entry.is_regular_file() && entry.path().extension() == ".json")
				{
					std::string manifestPath = entry.path().string();
					registerCondition(manifestPath);
				}
			}

			if (!std::filesystem::exists("Data\\manifest\\transitions"))
			{
				INVOKE_PANIC("Manifest transition directory does not exist: Data\\manifest\\transitions\\");
				return false;
			}

			for (const auto& entry : std::filesystem::recursive_directory_iterator("Data\\manifest\\transitions\\"))
			{
				if (entry.is_regular_file() && entry.path().extension() == ".json")
				{
					std::string manifestPath = entry.path().string();
					registerTransition(manifestPath);
				}
			}

			return true;
		}

		void Manifest::shutdown()
		{
			while (!m_registeredStateMachines.empty())
				unregisterStateMachine(0);

			while (!m_registeredStateMachineNodes.empty())
				unregisterStateMachineNode(0);

			while (!m_registeredNodes.empty())
				unregisterNode(0);

			while (!m_registeredConditions.empty())
				unregisterCondition(0);

			while (!m_regosteredTransitions.empty())
				unregisterTransition(0);
		}

		MMStateMachine* Manifest::getStateMachineManifest(uint32_t index)
		{
			if (index >= m_registeredStateMachines.size())
				return nullptr;

			return m_registeredStateMachines[index];
		}

		MMStateMachine* Manifest::findStateMachineManifest(uint32_t id)
		{
			for (auto& stateMachine : m_registeredStateMachines)
			{
				if (stateMachine->getID() == id)
					return stateMachine;
			}

			return nullptr;
		}

		MMStateMachine* Manifest::findStateMachineManifest(const std::string& name)
		{
			for (auto& stateMachine : m_registeredStateMachines)
			{
				if (stateMachine->getName() == name)
					return stateMachine;
			}

			return nullptr;
		}

		MMStateMachineNode* Manifest::getStateMachineNodeManifest(uint32_t index)
		{
			if (index >= m_registeredStateMachineNodes.size())
				return nullptr;

			return m_registeredStateMachineNodes[index];
		}

		MMStateMachineNode* Manifest::findStateMachineNodeManifest(uint32_t id)
		{
			for (auto& stateMachineNode : m_registeredStateMachineNodes)
			{
				if (stateMachineNode->getID() == id)
					return stateMachineNode;
			}

			return nullptr;
		}

		MMStateMachineNode* Manifest::findStateMachineNodeManifest(const std::string& name)
		{
			for (auto& stateMachineNode : m_registeredStateMachineNodes)
			{
				if (stateMachineNode->getName() == name)
					return stateMachineNode;
			}

			return nullptr;
		}

		MMNode* Manifest::getNodeManifest(uint32_t index)
		{
			if (index >= m_registeredNodes.size())
				return nullptr;

			return m_registeredNodes[index];
		}

		MMNode* Manifest::findNodeManifest(uint32_t id)
		{
			for (auto& node : m_registeredNodes)
			{
				if (node->getID() == id)
					return node;
			}

			return nullptr;
		}

		MMNode* Manifest::findNodeManifest(const std::string& name)
		{
			for (auto& node : m_registeredNodes)
			{
				if (node->getName() == name)
					return node;
			}

			return nullptr;
		}

		MMCondition* Manifest::getConditionManifest(uint32_t index)
		{
			if (index >= m_registeredConditions.size())
				return nullptr;

			return m_registeredConditions[index];
		}

		MMCondition* Manifest::findConditionManifest(uint32_t id)
		{
			for (auto& condition : m_registeredConditions)
			{
				if (condition->getID() == id)
					return condition;
			}

			return nullptr;
		}

		MMCondition* Manifest::findConditionManifest(const std::string& name)
		{
			for (auto& condition : m_registeredConditions)
			{
				if (condition->getName() == name)
					return condition;
			}

			return nullptr;
		}

		MMTransition* Manifest::getTransitionManifest(uint32_t index)
		{
			if (index >= m_regosteredTransitions.size())
				return nullptr;

			return m_regosteredTransitions[index];
		}

		MMTransition* Manifest::findTransitionManifest(uint32_t id)
		{
			for (auto& transition : m_regosteredTransitions)
			{
				if (transition->getAnimID() == id)
					return transition;
			}

			return nullptr;
		}

		MMTransition* Manifest::findTransitionManifest(const std::string& name)
		{
			for (auto& transition : m_regosteredTransitions)
			{
				if (transition->getName() == name)
					return transition;
			}

			return nullptr;
		}

		void Manifest::printRegisteredManifests()
		{
			for (size_t i = 0; i < getNumRegisteredStateMachines(); i++)
			{
				MMStateMachine* stateMachine = getStateMachineManifest(static_cast<uint32_t>(i));

				const int id = stateMachine->getID();
				const std::string name = stateMachine->getName();

				printf_s("Registered State Machine: ID=%d, Name=%s\n", id, name.c_str());
			}

			for (size_t i = 0; i < getNumRegisteredStateMachineNodes(); i++)
			{
				MMStateMachineNode* stateMachineNode = getStateMachineNodeManifest(static_cast<uint32_t>(i));
				const int id = stateMachineNode->getID();
				const std::string name = stateMachineNode->getName();

				printf_s("Registered State Machine Node: ID=%d, Name=%s\n", id, name.c_str());
			}

			for (size_t i = 0; i < getNumRegisteredNodes(); i++)
			{
				MMNode* node = getNodeManifest(static_cast<uint32_t>(i));

				const int id = node->getID();
				const std::string name = node->getName();

				printf_s("Registered Node: ID=%d, Name=%s\n", id, name.c_str());
			}

			for (size_t i = 0; i < getNumRegisteredConditions(); i++)
			{
				MMCondition* condition = getConditionManifest(static_cast<uint32_t>(i));

				const int id = condition->getID();
				const std::string name = condition->getName();

				printf_s("Registered Condition: ID=%d, Name=%s\n", id, name.c_str());
			}

			for (size_t i = 0; i < getNumRegisteredTransitions(); i++)
			{
				MMTransition* transition = getTransitionManifest(static_cast<uint32_t>(i));

				const int id = transition->getAnimID();
				const std::string name = transition->getName();

				printf_s("Registered Transition: ID=%d, Name=%s\n", id, name.c_str());
			}
		}
	}
}