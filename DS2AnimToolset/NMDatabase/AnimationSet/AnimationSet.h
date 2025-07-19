#pragma once
#include "RCore.h"
#include "../Skin/Skin.h"
#include <SimpleMath.h>

namespace db
{
	class AnimationSet
	{
		enum class Format
		{
			kNsa,
			kMba,
			kQsa,
			kNumFormats
		};

		std::string m_rig = "";
		std::string m_assetManagerSkin = "";
		std::vector<Skin> m_skins;
		Format m_format = kNsa;
		std::vector<std::string> m_channelNames;
		int m_networkFollowJoint = 0;
		int m_assetManagerFollowJoint = 0;
		std::string m_template = "";
		DirectX::SimpleMath::Vector3 m_retargetSrcStartPointLocation[4];

	public:
		AnimationSet() {};
		~AnimationSet() {};

		std::string getRig() const { return m_rig; }
		void setRig(const std::string& rig) { m_rig = rig; }

		std::string getAssetManagerSkin() const { return m_assetManagerSkin; }
		void setAssetManagerSkin(const std::string& skin) { m_assetManagerSkin = skin; }

		Skin getSkin(int index) const 
		{ 
			if (index < 0 || index >= m_skins.size())
				throw std::out_of_range("Index out of range");
			return m_skins[index]; 
		}

		void addSkin(const Skin& skin) { m_skins.push_back(skin); }
		void removeSkin(int index) 
		{ 
			if (index < 0 || index >= m_skins.size())
				throw std::out_of_range("Index out of range");
			m_skins.erase(m_skins.begin() + index); 
		}

		Format getFormat() const { return m_format; }
		void setFormat(Format format) { m_format = format; }

		std::string getChannelName(int index) const 
		{ 
			if (index < 0 || index >= m_channelNames.size())
				throw std::out_of_range("Index out of range");
			return m_channelNames[index]; 
		}

		void addChannelName(const std::string& name) { m_channelNames.push_back(name); }

		int getNetworkFollowJoint() const { return m_networkFollowJoint; }
		void setNetworkFollowJoint(int joint) { m_networkFollowJoint = joint; }

		int getAssetManagerFollowJoint() const { return m_assetManagerFollowJoint; }
		void setAssetManagerFollowJoint(int joint) { m_assetManagerFollowJoint = joint; }

		std::string getTemplate() const { return m_template; }
		void setTemplate(const std::string& templ) { m_template = templ; }

		DirectX::SimpleMath::Vector3* getRetargetSrcStartPointLocation() const { return const_cast<DirectX::SimpleMath::Vector3*>(m_retargetSrcStartPointLocation); }
		void setRetargetSrcStartPointLocation(const DirectX::SimpleMath::Vector3 rx, const DirectX::SimpleMath::Vector3 ry, const DirectX::SimpleMath::Vector3 rz, const DirectX::SimpleMath::Vector3 position) 
		{ 
			this->m_retargetSrcStartPointLocation[0] = rx;	
			this->m_retargetSrcStartPointLocation[1] = ry;
			this->m_retargetSrcStartPointLocation[2] = rz;
			this->m_retargetSrcStartPointLocation[3] = position;
		}
	};
}