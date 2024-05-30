#pragma once
#include "morpheme/mrAnimationSourceHandle.h"
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class AnimSourceInterface
{
public:
	AnimSourceInterface();
	AnimSourceInterface(MR::AnimRigDef* rig, MR::RigToAnimMap* rigToAnimMap, const char* filename, int id);
	~AnimSourceInterface();

	static bool lessThan(AnimSourceInterface* a, AnimSourceInterface* b);

	int GetID() const { return this->m_id; }
	const char* GetAnimName() const { return this->m_animName; };
	MR::AnimationSourceHandle* GetHandle() const { return this->m_animHandle; }

	Matrix GetTransformAtTime(float time, int channelId);
	Vector3 GetTransformPosAtTime(float time, int channelId);
	Quaternion GetTransformQuatAtTime(float time, int channelId);
private:
	MR::AnimationSourceHandle* m_animHandle;
	int m_id;
	char m_animName[255];
};