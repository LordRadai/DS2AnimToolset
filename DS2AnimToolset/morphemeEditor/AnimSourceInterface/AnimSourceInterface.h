#pragma once
#include "morpheme/mrAnimationSourceHandle.h"
#include "export/include/export/mcExportXml.h"
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class CharacterDef;

class AnimSourceInterface
{
public:
	AnimSourceInterface();
	AnimSourceInterface(CharacterDef* owner, MR::AnimRigDef* rig, MR::RigToAnimMap* rigToAnimMap, const char* filename, int id);
	~AnimSourceInterface();

	static bool lessThan(AnimSourceInterface* a, AnimSourceInterface* b);

	int GetID() const { return this->m_id; }
	const char* GetAnimName() const { return this->m_animName; };
	MR::AnimationSourceHandle* GetHandle() const { return this->m_animHandle; }
	ME::TakeListXML* GetTakeList() const { return this->m_takeList; }

	Matrix GetTransformAtTime(float time, int channelId);
	Vector3 GetTransformPosAtTime(float time, int channelId);
	Quaternion GetTransformQuatAtTime(float time, int channelId);

	void SetTakeList(ME::TakeListXML* takeList) { this->m_takeList = takeList; }
private:
	MR::AnimationSourceHandle* m_animHandle;
	ME::TakeListXML* m_takeList;
	int m_id;
	char m_animName[255];
};