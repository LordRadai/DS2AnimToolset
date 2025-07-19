#include "AnimationSet.h"

namespace db
{
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