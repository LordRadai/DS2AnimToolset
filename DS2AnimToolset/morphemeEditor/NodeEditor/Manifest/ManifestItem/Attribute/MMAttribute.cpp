#include "MMAttribute.h"

#include "NodeEditor/Editor/Attribute/SingleValue/EnumAttribute.h"

namespace NodeEditor
{
	namespace Manifest
	{
		MMAttribute::MMAttribute(const nlohmann::json& json, const std::string& name)
		{
			this->m_name = name;
			fromJson(json);

			if (json.contains("name"))
				this->m_name = json["name"];
		}

		void MMAttribute::fromJson(const nlohmann::json& json)
		{
			this->m_jsonData = json;
		}

		std::vector<const std::string> MMAttribute::getEnumOptions() const
		{
			std::vector<const std::string> options;

			if (m_jsonData.contains("enumOptions") && m_jsonData["enumOptions"].is_array())
			{
				for (const auto& option : m_jsonData["enumOptions"])
				{
					if (option.is_string())
						options.push_back(option.get<std::string>());
				}
			}

			return options;
		}

		bool MMAttribute::isArray() const
		{
			std::string type = getType();

			return type == "boolArray" || type == "floatArray" || type == "intArray" || type == "refArray" || type == "rigChannelName";
		}

		uint32_t MMAttribute::size() const
		{
			if (!isArray())
				throw std::runtime_error("MMAttribute::size() - Attribute is not an array");

			nlohmann::json value = getValue();

			// The manifest might not have a value list. In that case, we return 0
			if (value == nullptr)
				return 0;

			return static_cast<uint32_t>(value.size());
		}

		bool MMAttribute::getBoolValue() const
		{
			if (isArray())
				throw std::runtime_error("MMAttribute::getBoolValue() - Attribute is an array");

			nlohmann::json value = getValue();

			if (value == nullptr)
				return false;

			return value.get<bool>();
		}

		float MMAttribute::getFloatValue() const
		{
			if (isArray())
				throw std::runtime_error("MMAttribute::getFloatValue() - Attribute is an array");

			nlohmann::json value = getValue();

			if (value == nullptr)
				return 0.0f;

			return value.get<float>();
		}

		int MMAttribute::getIntValue() const
		{
			if (isArray())
				throw std::runtime_error("MMAttribute::getIntValue() - Attribute is an array");

			nlohmann::json value = getValue();

			if (value == nullptr)
				return 0;

			return value.get<int>();
		}

		std::string MMAttribute::getStringValue() const
		{
			if (isArray())
				throw std::runtime_error("MMAttribute::getStringValue() - Attribute is an array");

			nlohmann::json value = getValue();

			if (value == nullptr)
				return "";

			return value.get<std::string>();
		}

		bool MMAttribute::getBoolValue(const uint32_t index) const
		{
			if (!isArray())
				throw std::runtime_error("MMAttribute::getBoolValue(index) - Attribute is not an array");

			nlohmann::json value = getValue();

			if (value == nullptr || index >= value.size())
				throw std::out_of_range("MMAttribute::getBoolValue(index) - Index out of range");

			return value[index].get<bool>();
		}

		float MMAttribute::getFloatValue(const uint32_t index) const
		{
			if (!isArray())
				throw std::runtime_error("MMAttribute::getFloatValue(index) - Attribute is not an array");

			nlohmann::json value = getValue();

			if (value == nullptr || index >= value.size())
				throw std::out_of_range("MMAttribute::getFloatValue(index) - Index out of range");

			return value[index].get<float>();
		}

		int MMAttribute::getIntValue(const uint32_t index) const
		{
			if (!isArray())
				throw std::runtime_error("MMAttribute::getIntValue(index) - Attribute is not an array");

			nlohmann::json value = getValue();

			if (value == nullptr || index >= value.size())
				throw std::out_of_range("MMAttribute::getIntValue(index) - Index out of range");

			return value[index].get<int>();
		}

		std::string MMAttribute::getStringValue(const uint32_t index) const
		{
			if (!isArray())
				throw std::runtime_error("MMAttribute::getStringValue(index) - Attribute is not an array");

			nlohmann::json value = getValue();

			if (value == nullptr || index >= value.size())
				throw std::out_of_range("MMAttribute::getStringValue(index) - Index out of range");

			return value[index].get<std::string>();
		}

		Attribute* MMAttribute::makeAttribute(Entity* owner)
		{
			Attribute* attr = Attribute::createAttribute(owner, getDisplayName(), getType());

			if (attr->isOfType<EnumAttribute>())
			{
				EnumAttribute* enumAttr = attr->asType<EnumAttribute>();

				std::vector<const std::string> enumOptions = getEnumOptions();

				for (size_t i = 0; i < enumOptions.size(); i++)
					enumAttr->addEnumOption(enumOptions[i]);
			}
		}
	}
}