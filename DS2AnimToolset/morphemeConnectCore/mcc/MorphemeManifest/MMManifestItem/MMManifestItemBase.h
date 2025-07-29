#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace mcc
{
	class MMManifestItemBase
	{
	protected:
		std::string m_name;
		nlohmann::json m_jsonData;
	public:
		MMManifestItemBase() {};

		virtual ~MMManifestItemBase() {};
		virtual nlohmann::json toJson() const { return nullptr; };
		virtual void fromJson(const nlohmann::json& json) = 0;

		std::string getName() const { return m_name; }
		nlohmann::json getData() const { return m_jsonData; }

		void setName(const std::string& name) { m_name = name; }

		int getVersion() const { return m_jsonData["version"]; }
	};
}
