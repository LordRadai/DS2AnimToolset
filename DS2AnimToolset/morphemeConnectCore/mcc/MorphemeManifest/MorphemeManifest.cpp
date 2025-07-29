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

	void MorphemeManifest::shutdown()
	{
		for (size_t i = 0; i < m_registeredNodes.size(); i++)
			unregisterNode(i);
	}

	MMNode* MorphemeManifest::getManifest(uint32_t index)
	{
		if (index >= m_registeredNodes.size())
			return nullptr;

		return &m_registeredNodes[index];
	}

	MMNode* MorphemeManifest::findManifest(uint32_t id)
	{
		for (auto& node : m_registeredNodes)
		{
			if (node.getId() == id)
				return &node;
		}

		return nullptr;
	}
}
