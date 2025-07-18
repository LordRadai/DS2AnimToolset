#pragma once
#include <string>
#include <vector>
#include "NodeAttributes/NodeAttributes.h"
#include "NodePins/NodePins.h"

namespace MCN
{
	namespace Node
	{
		class mcnNode
		{
		protected:
			float m_xpos = 0.0f;
			float m_ypos = 0.0f;
			float m_width = 0.0f;
			float m_height = 0.0f;
			std::string m_nodeType = "";
			int m_manifestVersion = 0;
			std::vector<NodeAttribute> m_attributes;
			std::vector<FunctionalPin> m_functionalPins;
			std::vector<DataPin> m_dataPins;

		public:
			mcnNode() = default;
			mcnNode(float xpos, float ypos, float width, float height)
				: m_xpos(xpos), m_ypos(ypos), m_width(width), m_height(height) {}

			void setPosition(float x, float y) { m_xpos = x; m_ypos = y; }
			void setSize(float width, float height) { m_width = width; m_height = height; }
			void setNodeType(const std::string& type) { m_nodeType = type; }
			void setManifestVersion(int version) { m_manifestVersion = version; }

			void addAttribute(const NodeAttribute& attribute) { m_attributes.push_back(attribute); }
			void addFunctionalPin(const FunctionalPin& pin) { m_functionalPins.push_back(pin); }
			void addDataPin(const DataPin& pin) { m_dataPins.push_back(pin); }

			const NodeAttribute* getAttribute(size_t index) const;
			const FunctionalPin* getFunctionalPin(size_t index) const;
			const DataPin* getDataPin(size_t index) const;

			const size_t getNumAttributes() const { return m_attributes.size(); }
			const size_t getNumFunctionalPins() const { return m_functionalPins.size(); }
			const size_t getNumDataPins() const { return m_dataPins.size(); }
		};
	}
}