#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace NodeEditor
{
	namespace Manifest
	{
		class ManifestItemBase
		{
		protected:
			std::string m_name;
			nlohmann::json m_jsonData;
		public:
			ManifestItemBase() {};

			virtual ~ManifestItemBase() {};
			virtual nlohmann::json toJson() const { return nullptr; };
			virtual void fromJson(const nlohmann::json& json) = 0;

			std::string getName() const { return m_name; }
			nlohmann::json getData() const { return m_jsonData; }

			void setName(const std::string& name) { m_name = name; }

			int getVersion() const { return m_jsonData["version"]; }

			template<typename T>
			bool isOfType() const
			{
				return dynamic_cast<const T*>(this) != nullptr;
			}

			template<typename T>
			T* asType()
			{
				return dynamic_cast<T*>(this);
			}
		};
	}
}