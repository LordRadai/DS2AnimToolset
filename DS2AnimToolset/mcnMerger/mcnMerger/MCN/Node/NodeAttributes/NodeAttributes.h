#pragma once
#include <string>
#include <vector>

namespace MCN
{
	namespace Node
	{
		class NodeAttribute
		{
		protected:
			std::string m_name;
			std::string m_type;
		};

		class BoolAttribute : public NodeAttribute
		{
			bool m_value;

		public:
			BoolAttribute(const std::string& name, bool value)
			{
				m_name = name;
				m_type = "node";
				m_value = value;
			}

			bool getValue() const { return m_value; }
			void setValue(bool value) { m_value = value; }
		};

		class IntAttribute : public NodeAttribute
		{
			int m_value;

		public:
			IntAttribute(const std::string& name, const std::string& type, int value)
			{
				m_name = name;
				m_type = type;
				m_value = value;
			}

			int getValue() const { return m_value; }
			void setValue(int value) { m_value = value; }
		};

		class FloatAttribute : public NodeAttribute
		{
			float m_value;
		public:
			FloatAttribute(const std::string& name, const std::string& type, float value)
			{
				m_name = name;
				m_type = type;
				m_value = value;
			}

			float getValue() const { return m_value; }
			void setValue(float value) { m_value = value; }
		};

		class AttributePlaceholder : public NodeAttribute
		{
			std::vector<NodeAttribute> m_attributes;

		public:
			AttributePlaceholder(const std::string& name, const std::string& type)
			{
				m_name = name;
				m_type = type;
			}
			void addAttribute(const NodeAttribute& attribute)
			{
				m_attributes.push_back(attribute);
			}

			const NodeAttribute* getAttribute(size_t index) const
			{
				if (index < m_attributes.size())
					return &m_attributes[index];

				return nullptr;
			}

			size_t getNumAttributes() const { return m_attributes.size(); }
		};

		class AnimationTakeAttribute : public NodeAttribute
		{
			std::string m_take = "";
			std::string m_filename = "";
			std::string m_format = "";
			std::string m_syncTrack = "";

		public:
			AnimationTakeAttribute(const std::string& take, const std::string& filename, const std::string& format, const std::string& syncTrack)
			{
				m_name = "AnimationTake";
				m_type = "node";
				m_take = take;
				m_filename = filename;
				m_format = format;
				m_syncTrack = syncTrack;
			}

			const std::string& getTake() const { return m_take; }
			const std::string& getFilename() const { return m_filename; }
			const std::string& getFormat() const { return m_format; }
			const std::string& getSyncTrack() const { return m_syncTrack; }
		};

		class AnimationSetAttribute : public NodeAttribute
		{
			std::vector<AttributePlaceholder> m_attributePlaceholders;

		public:
			AnimationSetAttribute(const std::string& name)
			{
				m_name = name;
				m_type = "node";
			}

			void addAttributePlaceholder(const AttributePlaceholder& placeholder)
			{
				m_attributePlaceholders.push_back(placeholder);
			}

			const AttributePlaceholder* getAttributePlaceholder(size_t index) const;
		};
	}
}