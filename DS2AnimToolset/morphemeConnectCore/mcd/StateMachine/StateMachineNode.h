#pragma once
#include "mcd/Graph/GraphNode.h"

namespace mcd
{
	class StateMachineNode : public GraphNode
	{
		std::unique_ptr<db::IntAttribute> m_manifestVersion;
		std::unique_ptr<db::StringAttribute> m_nodeType;

	public:
		StateMachineNode(db::Attribute* parent, const std::string& name, float xpos, float ypos, float width, float height);

		virtual ~StateMachineNode() override {}

		int getManifestVersion() const { return m_manifestVersion->getValue(); }
		const std::string& getNodeType() const { return m_nodeType->getValue(); }

		void setManifestVersion(int version);
		void setNodeType(const std::string& type);
	};
}
