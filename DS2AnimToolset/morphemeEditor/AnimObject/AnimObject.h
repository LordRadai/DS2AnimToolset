#pragma once
#include "morpheme/mrAnimationSourceHandle.h"
#include "export/include/export/mcExportXml.h"
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class MorphemeCharacterDef;

class AnimObject
{
public:
	AnimObject() {}
	AnimObject(int id);
	~AnimObject() {}

	static AnimObject* createFromMorphemeAssets(MorphemeCharacterDef* owner, MR::AnimRigDef* rig, MR::RigToAnimMap* rigToAnimMap, const char* filename, int id, std::vector<ME::EventTrackExport*>& exportedTracks);

	inline bool operator <(const AnimObject& other)
	{
		return std::string(this->m_animName) < std::string(other.m_animName);
	}

	int getAnimID() const { return this->m_id; }
	const char* getAnimFileName() const { return this->m_animFileName.c_str(); };
	const char* getAnimName() const { return this->m_animName.c_str(); };
	MR::AnimationSourceHandle* getHandle() const { return this->m_animHandle; }
	ME::TakeListXML* getTakeList() const { return this->m_takeList; }
	float getAnimLenght() const { return this->m_animHandle->getDuration(); }

	Matrix getTransformAtTime(float time, int channelId);
	Vector3 getTransformPosAtTime(float time, int channelId);
	Quaternion getTransformQuatAtTime(float time, int channelId);

	void setTakeList(ME::TakeListXML* takeList) { this->m_takeList = takeList; }

	void setAnimTime(float time);

	bool isLoaded() const { return this->m_animHandle != nullptr; }
private:
	MR::AnimationSourceHandle* m_animHandle = nullptr;
	ME::TakeListXML* m_takeList = nullptr;
	int m_id = -1;
	std::string m_animFileName = "";
	std::string m_animName = "";
};