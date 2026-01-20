#pragma once
#include "mcd/Graph/GraphNode.h"
#include "mcd/Attribute/SingleValue/StringAttribute.h"
#include "mcd/Attribute/SingleValue/IntAttribute.h"

namespace mcd
{
	class BlendTreeNode : public GraphNode
	{
		std::unique_ptr<db::StringAttribute> m_nodeType;
		std::unique_ptr<db::IntAttribute> m_manifestVersion;
	public:
		BlendTreeNode(db::CompositeAttribute* parent, std::string name, float xPos, float yPos, float width, float height, std::string nodeType, int manifestVersion);

		virtual ~BlendTreeNode() override {}
		virtual bool isOperatorNode() override;

		const std::string& getNodeType() const { return m_nodeType->getValue(); }
		void setNodeType(const std::string& type) { m_nodeType->setValue(type); }

		int getManifestVersion() const { return m_manifestVersion->getValue(); }
		void setManifestVersion(int version) { m_manifestVersion->setValue(version); }
	};
}
