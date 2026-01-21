#include "MorphemeManifest.h"
#include <filesystem>
#include <fstream>

namespace mcc
{
	void MorphemeManifest::registerStateMachine(std::string manifestPath)
	{
		if (!std::filesystem::exists(manifestPath))
			throw std::runtime_error("Manifest file does not exist: " + manifestPath);

		std::ifstream file(manifestPath);
		nlohmann::json jsonData = nlohmann::json::parse(file);

		file.close();

		if (!jsonData.is_object())
			throw std::runtime_error("Invalid JSON format for MorphemeManifest state machine registration");

		MMStateMachine* stateMachine = new MMStateMachine(jsonData);
		stateMachine->setName(std::filesystem::path(manifestPath).filename().replace_extension("").string());

		m_registeredStateMachines.push_back(stateMachine);
	}

	void MorphemeManifest::unregisterStateMachine(uint32_t index)
	{
		if (index >= m_registeredStateMachines.size())
			throw std::out_of_range("Index out of range for registered state machines");

		delete m_registeredStateMachines[index];
		m_registeredStateMachines.erase(m_registeredStateMachines.begin() + index);
	}

	void MorphemeManifest::registerStateMachineNode(std::string manifestPath)
	{
		if (!std::filesystem::exists(manifestPath))
			throw std::runtime_error("Manifest file does not exist: " + manifestPath);

		std::ifstream file(manifestPath);
		nlohmann::json jsonData = nlohmann::json::parse(file);

		file.close();

		if (!jsonData.is_object())
			throw std::runtime_error("Invalid JSON format for MorphemeManifest state machine node registration");

		MMStateMachineNode* stateMachineNode = new MMStateMachineNode(jsonData);
		stateMachineNode->setName(std::filesystem::path(manifestPath).filename().replace_extension("").string());

		m_registeredStateMachineNodes.push_back(stateMachineNode);
	}

	void MorphemeManifest::unregisterStateMachineNode(uint32_t index)
	{
		if (index >= m_registeredStateMachineNodes.size())
			throw std::out_of_range("Index out of range for registered state machine nodes");

		delete m_registeredStateMachineNodes[index];

		m_registeredStateMachineNodes.erase(m_registeredStateMachineNodes.begin() + index);
	}

	void MorphemeManifest::registerNode(std::string manifestPath)
	{
		if (!std::filesystem::exists(manifestPath))
			throw std::runtime_error("Manifest file does not exist: " + manifestPath);

		std::ifstream file(manifestPath);
		nlohmann::json jsonData = nlohmann::json::parse(file);

		file.close();

		if (!jsonData.is_object())
			throw std::runtime_error("Invalid JSON format for MorphemeManifest node registration");

		MMNode* node = new MMNode(jsonData);
		node->setName(std::filesystem::path(manifestPath).filename().replace_extension("").string());

		m_registeredNodes.push_back(node);
	}

	void MorphemeManifest::unregisterNode(uint32_t index)
	{
		if (index >= m_registeredNodes.size())
			throw std::out_of_range("Index out of range for registered nodes");

		delete m_registeredNodes[index];
		m_registeredNodes.erase(m_registeredNodes.begin() + index);
	}

	void MorphemeManifest::registerCondition(std::string manifestPath)
	{
		if (!std::filesystem::exists(manifestPath))
			throw std::runtime_error("Manifest file does not exist: " + manifestPath);

		std::ifstream file(manifestPath);
		nlohmann::json jsonData = nlohmann::json::parse(file);

		file.close();

		if (!jsonData.is_object())
			throw std::runtime_error("Invalid JSON format for MorphemeManifest condition registration");

		MMCondition* condition = new MMCondition(jsonData);
		condition->setName(std::filesystem::path(manifestPath).filename().replace_extension("").string());

		m_registeredConditions.push_back(condition);
	}

	void MorphemeManifest::unregisterCondition(uint32_t index)
	{
		if (index >= m_registeredConditions.size())
			throw std::out_of_range("Index out of range for registered conditions");

		delete m_registeredConditions[index];
		m_registeredConditions.erase(m_registeredConditions.begin() + index);
	}

	void MorphemeManifest::registerTransition(std::string manifestPath)
	{
		if (!std::filesystem::exists(manifestPath))
			throw std::runtime_error("Manifest file does not exist: " + manifestPath);

		std::ifstream file(manifestPath);

		nlohmann::json jsonData = nlohmann::json::parse(file);
		file.close();
		if (!jsonData.is_object())
			throw std::runtime_error("Invalid JSON format for MorphemeManifest transition registration");

		MMTransition* transition = new MMTransition(jsonData);
		transition->setName(std::filesystem::path(manifestPath).filename().replace_extension("").string());

		m_regosteredTransitions.push_back(transition);
	}

	void MorphemeManifest::unregisterTransition(uint32_t index)
	{
		if (index >= m_regosteredTransitions.size())
			throw std::out_of_range("Index out of range for registered transitions");

		delete m_regosteredTransitions[index];
		m_regosteredTransitions.erase(m_regosteredTransitions.begin() + index);
	}

	bool MorphemeManifest::init()
	{
		if (!std::filesystem::exists("Data\\manifest\\"))
		{
			throw std::runtime_error("Manifest directory does not exist: Data\\manifest\\");
			return false;
		}

		if (!std::filesystem::exists("Data\\manifest\\nodes\\"))
		{
			throw std::runtime_error("Manifest node directory does not exist: Data\\manifest\\nodes\\");
			return false;
		}

		registerStateMachine("Data\\manifest\\nodes\\animation\\StateMachine.json");
		registerStateMachineNode("Data\\manifest\\nodes\\animation\\StateMachine.json");

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
			throw std::runtime_error("Manifest condition directory does not exist: Data\\manifest\\conditions\\");
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
			throw std::runtime_error("Manifest transition directory does not exist: Data\\manifest\\transitions\\");
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

	void MorphemeManifest::shutdown()
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

	MMStateMachine* MorphemeManifest::getStateMachineManifest(uint32_t index)
	{
		if (index >= m_registeredStateMachines.size())
			return nullptr;

		return m_registeredStateMachines[index];
	}

	MMStateMachine* MorphemeManifest::findStateMachineManifest(uint32_t id)
	{
		for (auto& stateMachine : m_registeredStateMachines)
		{
			if (stateMachine->getID() == id)
				return stateMachine;
		}

		return nullptr;
	}

	MMStateMachine* MorphemeManifest::findStateMachineManifest(const std::string& name)
	{
		for (auto& stateMachine : m_registeredStateMachines)
		{
			if (stateMachine->getName() == name)
				return stateMachine;
		}

		return nullptr;
	}

	MMStateMachineNode* MorphemeManifest::getStateMachineNodeManifest(uint32_t index)
	{
		if (index >= m_registeredStateMachineNodes.size())
			return nullptr;

		return m_registeredStateMachineNodes[index];
	}

	MMStateMachineNode* MorphemeManifest::findStateMachineNodeManifest(uint32_t id)
	{
		for (auto& stateMachineNode : m_registeredStateMachineNodes)
		{
			if (stateMachineNode->getID() == id)
				return stateMachineNode;
		}

		return nullptr;
	}

	MMStateMachineNode* MorphemeManifest::findStateMachineNodeManifest(const std::string& name)
	{
		for (auto& stateMachineNode : m_registeredStateMachineNodes)
		{
			if (stateMachineNode->getName() == name)
				return stateMachineNode;
		}

		return nullptr;
	}

	MMNode* MorphemeManifest::getNodeManifest(uint32_t index)
	{
		if (index >= m_registeredNodes.size())
			return nullptr;

		return m_registeredNodes[index];
	}

	MMNode* MorphemeManifest::findNodeManifest(uint32_t id)
	{
		for (auto& node : m_registeredNodes)
		{
			if (node->getID() == id)
				return node;
		}

		return nullptr;
	}

	MMNode* MorphemeManifest::findNodeManifest(const std::string& name)
	{
		for (auto& node : m_registeredNodes)
		{
			if (node->getName() == name)
				return node;
		}

		return nullptr;
	}

	MMCondition* MorphemeManifest::getConditionManifest(uint32_t index)
	{
		if (index >= m_registeredConditions.size())
			return nullptr;

		return m_registeredConditions[index];
	}

	MMCondition* MorphemeManifest::findConditionManifest(uint32_t id)
	{
		for (auto& condition : m_registeredConditions)
		{
			if (condition->getID() == id)
				return condition;
		}

		return nullptr;
	}

	MMCondition* MorphemeManifest::findConditionManifest(const std::string& name)
	{
		for (auto& condition : m_registeredConditions)
		{
			if (condition->getName() == name)
				return condition;
		}

		return nullptr;
	}

	MMTransition* MorphemeManifest::getTransitionManifest(uint32_t index)
	{
		if (index >= m_regosteredTransitions.size())
			return nullptr;

		return m_regosteredTransitions[index];
	}

	MMTransition* MorphemeManifest::findTransitionManifest(uint32_t id)
	{
		for (auto& transition : m_regosteredTransitions)
		{
			if (transition->getAnimID() == id)
				return transition;
		}

		return nullptr;
	}

	MMTransition* MorphemeManifest::findTransitionManifest(const std::string& name)
	{
		for (auto& transition : m_regosteredTransitions)
		{
			if (transition->getName() == name)
				return transition;
		}

		return nullptr;
	}

	void MorphemeManifest::printRegisteredManifests()
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
