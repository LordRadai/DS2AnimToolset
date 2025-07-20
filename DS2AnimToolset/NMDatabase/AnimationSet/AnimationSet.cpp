#include "AnimationSet.h"
#include "../mcnSerializer/mcnSerializer.h"

namespace db
{
	bool AnimationSet::isValid() const
	{
		if (!Node::isValid())
			return false;

		if (m_rig.empty())
			return false;

		if (m_skins.getNumNodes() == 0)
			return false;

		if (m_format.empty())
			return false;

		if (m_networkFollowJoint < 0)
			return false;

		if (m_assetManagerFollowJoint < 0)
			return false;

		for (size_t i = 0; i < m_skins.getNumNodes(); i++)
		{
			Skin* skin = static_cast<Skin*>(m_skins.getNode(i));

			if (skin == nullptr || !skin->isValid())
				return false;
		}

		return true;
	}

	tinyxml2::XMLElement* AnimationSet::serialize(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLElement* element = Node::serialize(parent);
		element->SetName("AnimationSet");

		mcnSerializer::createStringElement(element, "Rig", m_rig);
		mcnSerializer::createStringElement(element, "AssetManagerSkin", m_assetManagerSkin);

		for (size_t i = 0; i < m_skins.getNumNodes(); i++)
			m_skins.getNode(i)->serialize(element);

		mcnSerializer::createStringElement(element, "Format", m_format);
		mcnSerializer::createStringArrayElement(element, "RigChannelNames", m_channelNames);
		mcnSerializer::createStringElement(element, "NetworkFollowJoint", getChannelName(m_networkFollowJoint));
		mcnSerializer::createStringElement(element, "AssetManagerFollowJoint", getChannelName(m_assetManagerFollowJoint));
		mcnSerializer::createStringElement(element, "Template", m_template);
		mcnSerializer::createMatrix34Element(element, "RetargetSrcStartPointLocation", m_retargetSrcStartPointLocation);
	}

	void AnimationSet::setRetargetSrcStartPointLocation(const NMP::Vector3 rx, const NMP::Vector3 ry, const NMP::Vector3 rz, const NMP::Vector3 position)
	{
		this->m_retargetSrcStartPointLocation.r[0] = rx;
		this->m_retargetSrcStartPointLocation.r[1] = ry;
		this->m_retargetSrcStartPointLocation.r[2] = rz;
		this->m_retargetSrcStartPointLocation.r[3] = position;
	}

	Skin* AnimationSet::getSkin(int index) const
	{
		if (index < 0 || index >= m_skins.getNumNodes())
			throw std::out_of_range("Index out of range");

		return static_cast<Skin*>(m_skins.getNode(index));
	}

	void AnimationSet::removeSkin(int index)
	{
		if (index < 0 || index >= m_skins.getNumNodes())
			throw std::out_of_range("Index out of range");

		m_skins.removeNode(index);
	}

	std::string AnimationSet::getChannelName(int index) const
	{
		if (index < 0 || index >= m_channelNames.size())
			throw std::out_of_range("Index out of range");

		return m_channelNames[index];
	}

	void AnimationSet::removeChannelName(int index)
	{
		if (index < 0 || index >= m_channelNames.size())
			throw std::out_of_range("Index out of range");

		m_channelNames.erase(m_channelNames.begin() + index);
	}
}