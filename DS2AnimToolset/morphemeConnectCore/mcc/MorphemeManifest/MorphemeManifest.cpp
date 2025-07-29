#include "MorphemeManifest.h"
#include <filesystem>
#include <fstream>

namespace mcc
{
	void MorphemeManifest::registerNode(std::string manifestPath)
	{
		if (!std::filesystem::exists(manifestPath))
			throw std::runtime_error("Manifest file does not exist: " + manifestPath);

		std::ifstream file(manifestPath);
		nlohmann::json jsonData = nlohmann::json::parse(file);

		file.close();

		if (!jsonData.is_object())
			throw std::runtime_error("Invalid JSON format for MorphemeManifest node registration");

		MMNode node(jsonData);
		node.setName(std::filesystem::path(manifestPath).filename().replace_extension("").string());

		m_registeredNodes.push_back(node);
	}

	void MorphemeManifest::unregisterNode(uint32_t index)
	{
		if (index >= m_registeredNodes.size())
			throw std::out_of_range("Index out of range for registered nodes");

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

		MMCondition condition(jsonData);
		condition.setName(std::filesystem::path(manifestPath).filename().replace_extension("").string());

		m_conditions.push_back(condition);
	}

	void MorphemeManifest::unregisterCondition(uint32_t index)
	{
		if (index >= m_conditions.size())
			throw std::out_of_range("Index out of range for registered conditions");

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

		MMTransition transition(jsonData);
		transition.setName(std::filesystem::path(manifestPath).filename().replace_extension("").string());

		m_transitions.push_back(transition);
	}

	void MorphemeManifest::unregisterTransition(uint32_t index)
	{
		if (index >= m_transitions.size())
			throw std::out_of_range("Index out of range for registered transitions");

		m_transitions.erase(m_transitions.begin() + index);
	}

	void MorphemeManifest::shutdown()
	{
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

	MMNode* MorphemeManifest::getNodeManifest(uint32_t index)
	{
		if (index >= m_registeredNodes.size())
			return nullptr;

		return &m_registeredNodes[index];
	}

	MMNode* MorphemeManifest::findNodeManifest(uint32_t id)
	{
		for (auto& node : m_registeredNodes)
		{
			if (node.getId() == id)
				return &node;
		}

		return nullptr;
	}

	MMCondition* MorphemeManifest::getConditionManifest(uint32_t index)
	{
		if (index >= m_conditions.size())
			return nullptr;

		return &m_conditions[index];
	}

	MMCondition* MorphemeManifest::findConditionManifest(const std::string& name)
	{
		for (auto& condition : m_conditions)
		{
			if (condition.getName() == name)
				return &condition;
		}

		return nullptr;
	}

	MMTransition* MorphemeManifest::getTransitionManifest(uint32_t index)
	{
		if (index >= m_transitions.size())
			return nullptr;

		return &m_transitions[index];
	}

	MMTransition* MorphemeManifest::findTransitionManifest(const std::string& name)
	{
		for (auto& transition : m_transitions)
		{
			if (transition.getName() == name)
				return &transition;
		}

		return nullptr;
	}
}
