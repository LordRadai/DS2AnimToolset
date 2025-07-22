#pragma once
#include "RCore.h"
#include "../Skin/Skin.h"
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMVector3.h"
#include "../Node/Node.h"
#include "../NodeContainer/NodeContainer.h"

namespace db
{
	class AnimationSet : public Node
	{
		std::string m_rig = "";
		Skin* m_assetManagerSkin = nullptr;
		NodeContainer m_skins;
		std::string m_format = "nsa";
		std::vector<std::string> m_channelNames;
		int m_networkFollowJoint = 0;
		int m_assetManagerFollowJoint = 0;
		std::string m_template = "";
		NMP::Matrix34 m_retargetSrcStartPointLocation;

	public:
		AnimationSet(Node* parent, std::string name) : Node(parent, "AnimationSet", name), m_skins(this, "Skins"), m_retargetSrcStartPointLocation(NMP::Matrix34Identity()) {};

		virtual ~AnimationSet() {};
		virtual bool isValid() const;
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		std::string getRig() const { return m_rig; }
		void setRig(const std::string& rig) { m_rig = rig; }

		Skin* getAssetManagerSkin() const { return m_assetManagerSkin; }
		void setAssetManagerSkin(Skin* skin) { m_assetManagerSkin = skin; }

		Skin* getSkin(int index) const;
		void addSkin(std::string name, std::string filepath) { m_skins.addNode(new Skin(this, name, filepath)); }
		void removeSkin(int index);

		std::string getFormat() const { return m_format; }
		void setFormat(std::string format) { m_format = format; }

		std::string getChannelName(int index) const;
		void addChannelName(const std::string& name) { m_channelNames.push_back(name); }
		void removeChannelName(int index);

		int getNetworkFollowJoint() const { return m_networkFollowJoint; }
		void setNetworkFollowJoint(int joint) { m_networkFollowJoint = joint; }

		int getAssetManagerFollowJoint() const { return m_assetManagerFollowJoint; }
		void setAssetManagerFollowJoint(int joint) { m_assetManagerFollowJoint = joint; }

		std::string getTemplate() const { return m_template; }
		void setTemplate(const std::string& templ) { m_template = templ; }

		NMP::Matrix34* getRetargetSrcStartPointLocation() const { return const_cast<NMP::Matrix34*>(&m_retargetSrcStartPointLocation); }
		void setRetargetSrcStartPointLocation(const NMP::Vector3 rx, const NMP::Vector3 ry, const NMP::Vector3 rz, const NMP::Vector3 position);
	};
}