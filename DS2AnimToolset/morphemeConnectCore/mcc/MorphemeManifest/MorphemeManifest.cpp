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

		m_registeredStateMachines.erase(m_registeredStateMachines.begin() + index);
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

		m_conditions.push_back(condition);
	}

	void MorphemeManifest::unregisterCondition(uint32_t index)
	{
		if (index >= m_conditions.size())
			throw std::out_of_range("Index out of range for registered conditions");

		delete m_conditions[index];
		m_conditions.erase(m_conditions.begin() + index);
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

		m_transitions.push_back(transition);
	}

	void MorphemeManifest::unregisterTransition(uint32_t index)
	{
		if (index >= m_transitions.size())
			throw std::out_of_range("Index out of range for registered transitions");

		delete m_transitions[index];
		m_transitions.erase(m_transitions.begin() + index);
	}

	void MorphemeManifest::shutdown()
	{
		// Unregister all state machines
		for (size_t i = 0; i < m_registeredStateMachines.size(); i++)
			unregisterStateMachine(i);

		// Unregister all nodes
		for (size_t i = 0; i < m_registeredNodes.size(); i++)
			unregisterNode(i);

		// Unregister all conditions
		for (size_t i = 0; i < m_conditions.size(); i++)
			unregisterCondition(i);

		// Unregister all transitions
		for (size_t i = 0; i < m_transitions.size(); i++)
			unregisterTransition(i);
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

	MMCondition* MorphemeManifest::getConditionManifest(uint32_t index)
	{
		if (index >= m_conditions.size())
			return nullptr;

		return m_conditions[index];
	}

	MMCondition* MorphemeManifest::findConditionManifest(uint32_t id)
	{
		for (auto& condition : m_conditions)
		{
			if (condition->getID() == id)
				return condition;
		}

		return nullptr;
	}

	MMTransition* MorphemeManifest::getTransitionManifest(uint32_t index)
	{
		if (index >= m_transitions.size())
			return nullptr;

		return m_transitions[index];
	}

	MMTransition* MorphemeManifest::findTransitionManifest(uint32_t id)
	{
		for (auto& transition : m_transitions)
		{
			if (transition->getAnimID() == id)
				return transition;
		}

		return nullptr;
	}
}
