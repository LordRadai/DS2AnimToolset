#pragma once
#include "../Attribute/MMAttribute.h"
#include "../Pin/MMDataPin.h"
#include "../Pin/MMFunctionalPin.h"

namespace mcd
{
	class MorphemeDB;
	class BlendTree;
	class BlendTreeNode;
}

namespace mcc
{
	class MMNode : public MMManifestItemBase
	{
		std::vector<MMAttribute*> m_attributes;
		std::vector<MMDataPin*> m_dataPins;
		std::vector<MMFunctionalPin*> m_functionalPins;
		std::vector<std::string> m_pinOrder;

		void setupNewBlendTreeNode(mcd::BlendTreeNode* node, mcd::BlendTree* parent);
		std::string getNodeDefaultName(mcd::BlendTreeNode* node, mcd::BlendTree* parent) const;
	public:
		MMNode() {}
		MMNode(const nlohmann::json& json) { fromJson(json); }

		virtual ~MMNode() override;
		virtual void fromJson(const nlohmann::json& json) override;

		virtual mcd::BlendTreeNode* createDatabaseNode(mcd::BlendTree* parent, mcd::MorphemeDB* morphemeDB);

		int getId() const { return m_jsonData["id"]; }
		std::string getGroup() const { return m_jsonData["group"]; }
		std::string getDisplayName() const { return m_jsonData["displayName"]; }
		std::string getImage() const { return m_jsonData["image"]; }

		MMAttribute* getAttribute(uint32_t index);
		MMAttribute* findAttribute(const std::string& name);
		uint32_t getNumAttributes() const { return static_cast<uint32_t>(m_attributes.size()); }

		MMDataPin* getDataPin(uint32_t index);
		MMDataPin* findDataPin(const std::string& name);
		uint32_t getNumDataPins() const { return static_cast<uint32_t>(m_dataPins.size()); }

		MMFunctionalPin* getFunctionalPin(uint32_t index);
		MMFunctionalPin* findFunctionalPin(const std::string& name);
		uint32_t getNumFunctionalPins() const { return static_cast<uint32_t>(m_functionalPins.size()); }

		void sortPins();
	};
}
