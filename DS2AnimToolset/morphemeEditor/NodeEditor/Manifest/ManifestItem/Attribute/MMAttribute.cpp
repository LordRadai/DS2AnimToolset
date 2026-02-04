#include "MMAttribute.h"

#include "NodeEditor/Editor/Attribute/SingleValue/EnumAttribute.h"
#include "NodeEditor/Editor/Attribute/SingleValue/BoolAttribute.h"
#include "NodeEditor/Editor/Attribute/SingleValue/FloatAttribute.h"
#include "NodeEditor/Editor/Attribute/SingleValue/IntAttribute.h"
#include "NodeEditor/Editor/Attribute/SingleValue/RefAttribute.h"
#include "NodeEditor/Editor/Attribute/SingleValue/StringAttribute.h"

#include "NodeEditor/Editor/Attribute/Array/BoolArrayAttribute.h"
#include "NodeEditor/Editor/Attribute/Array/FloatArrayAttribute.h"
#include "NodeEditor/Editor/Attribute/Array/IntArrayAttribute.h"
#include "NodeEditor/Editor/Attribute/Array/RefArrayAttribute.h"
#include "NodeEditor/Editor/Attribute/Array/StringArrayAttribute.h"

#include "NodeEditor/Editor/Attribute/AnimationTakeAttribute/AnimationTakeAttribute.h"
#include "NodeEditor/Editor/Attribute/ControlParameterAttribute/ControlParameterAttribute.h"
#include "NodeEditor/Editor/Attribute/RequestAttribute/RequestAttribute.h"

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

		std::vector<std::string> MMAttribute::getEnumOptions() const
		{
			std::vector<std::string> options;

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

			return type == "boolArray" || type == "floatArray" || type == "intArray" || type == "refArray";
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
			attr->setHelpText(getHelpText());

			if (attr->isOfType<EnumAttribute>())
			{
				EnumAttribute* enumAttr = attr->asType<EnumAttribute>();

				std::vector<std::string> enumOptions = getEnumOptions();

				for (size_t i = 0; i < enumOptions.size(); i++)
					enumAttr->addEnumOption(enumOptions[i]);

				enumAttr->setValue({ getIntValue() });
			}
			else if (attr->isOfType<BoolAttribute>())
			{
				BoolAttribute* boolAttr = attr->asType<BoolAttribute>();
				boolAttr->setValue({ getBoolValue() });
			}
			else if (attr->isOfType<FloatAttribute>())
			{
				FloatAttribute* floatAttr = attr->asType<FloatAttribute>();
				floatAttr->setValue({ getFloatValue() });
			}
			else if (attr->isOfType<IntAttribute>())
			{
				IntAttribute* intAttr = attr->asType<IntAttribute>();
				intAttr->setValue({ getIntValue() });
			}
			else if (attr->isOfType<StringAttribute>())
			{
				StringAttribute* stringAttr = attr->asType<StringAttribute>();
				stringAttr->setValue({ getStringValue() });
			}
			else if (attr->isOfType<RefAttribute>())
			{
				RefAttribute* refAttr = attr->asType<RefAttribute>();
				refAttr->setRefKind(getRefKind());
				refAttr->setWeakRef(isWeakRef());
			}
			else if (attr->isOfType<RefArrayAttribute>())
			{
				RefArrayAttribute* refAttr = attr->asType<RefArrayAttribute>();
				refAttr->setRefKind(getRefKind());
				refAttr->setWeakRef(isWeakRef());
			}
			else if (attr->isOfType<IntArrayAttribute>())
			{
				IntArrayAttribute* intArrayAttr = attr->asType<IntArrayAttribute>();
				nlohmann::json value = getValue();
				if (value.is_array())
				{
					std::vector<int> intValues = value.get<std::vector<int>>();
					std::vector<std::any> wrappedValues;

					for (float v : intValues)
						wrappedValues.push_back(v);

					intArrayAttr->setValue(wrappedValues);
				}
			}
			else if (attr->isOfType<BoolArrayAttribute>())
			{
				BoolArrayAttribute* boolAttr = attr->asType<BoolArrayAttribute>();
				nlohmann::json value = getValue();
				if (value.is_array())
				{
					std::vector<bool> boolValues = value.get<std::vector<bool>>();
					std::vector<std::any> wrappedValues;

					for (float v : boolValues)
						wrappedValues.push_back(v);

					boolAttr->setValue(wrappedValues);
				}
			}
			else if (attr->isOfType<FloatArrayAttribute>())
			{
				FloatArrayAttribute* floatArrayAttr = attr->asType<FloatArrayAttribute>();
				nlohmann::json value = getValue();
				if (value.is_array())
				{
					std::vector<float> floatValues = value.get<std::vector<float>>();
					std::vector<std::any> wrappedValues;

					for (float v : floatValues)
						wrappedValues.push_back(v);

					floatArrayAttr->setValue(wrappedValues);
				}
			}
			else if (attr->isOfType<StringArrayAttribute>())
			{
				StringArrayAttribute* stringArrayAttr = attr->asType<StringArrayAttribute>();
				nlohmann::json value = getValue();
				if (value.is_array())
				{
					std::vector<std::string> stringValues = value.get<std::vector<std::string>>();
					stringArrayAttr->setValue({ stringValues });
				}
			}
			else if (attr->isOfType<AnimationTakeAttribute>())
			{
				AnimationTakeAttribute* animTakeAttr = attr->asType<AnimationTakeAttribute>();
				nlohmann::json value = getValue();

				if (value.is_object())
					animTakeAttr->setValue({ value.value("filename", ""), value.value("takename", ""), std::string("Footsteps") });
			}

			return attr;
		}
	}
}