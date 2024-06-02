#include "AnimPlayer.h"
#include "utils/NMDX/NMDX.h"
#include "utils/RString/RString.h"
#include "utils/RLog/RLog.h"
#include "extern.h"

int GetMorphemeRigBoneIndexByFlverBoneIndex(MR::AnimRigDef* pRig, FlverModel* pFlverModel, int idx)
{
	if (idx == -1)
		return -1;

	std::string boneName = RString::ToNarrow(pFlverModel->m_flver->bones[idx].name);

	int boneIdx = pRig->getBoneIndexFromName(boneName.c_str());

	g_appLog->DebugMessage(MsgLevel_Debug, "Bone %s: (to=%d, from=%d)\n", boneName.c_str(), boneIdx, idx);

	return boneIdx;
}

int GetFlverBoneIDByMorphemeBoneID(MR::AnimRigDef* pRig, FlverModel* pFlverModel, int idx)
{
	std::string boneName = pRig->getBoneName(idx);
	int flverIdx = pFlverModel->GetBoneIndexFromName(boneName.c_str());

	return flverIdx;
}

AnimPlayer::AnimPlayer()
{
	this->m_anim = nullptr;
	this->m_time = 0.f;
	this->m_pause = true;
	this->m_loop = true;
	this->m_model = nullptr;
}

AnimPlayer::~AnimPlayer()
{
}

void AnimPlayer::Clear()
{
	if (this->m_model)
		delete this->m_model;

	this->m_model = nullptr;

	this->Reset();
}

void AnimPlayer::Update(float dt)
{
	MR::AnimationSourceHandle* animHandle = nullptr;

	if (this->m_anim)
		animHandle = this->m_anim->GetHandle();

	if (this->m_model != nullptr)
	{
		this->m_model->Animate(animHandle, this->m_flverToMorphemeBoneMap);
		this->m_model->UpdateModel();
	}

	if (animHandle == nullptr)
		return;

	if (!this->m_pause)
	{
		this->m_time += (this->m_playSpeed * dt);

		if (this->m_time > animHandle->getDuration())
		{
			if (this->m_loop)
				this->m_time = 0.f;
			else
				this->m_time = animHandle->getDuration();
		}
	}

	animHandle->setTime(this->m_time);
}

void AnimPlayer::SetAnimation(AnimSourceInterface* anim)
{
	this->Reset();

	this->m_anim = anim;
}

void AnimPlayer::Reset()
{
	this->SetModel(this->m_model);
	this->m_anim = nullptr;
	this->m_time = 0.f;
}

void AnimPlayer::TogglePause()
{
	this->m_pause = !this->m_pause;
}

void AnimPlayer::SetPause(bool status)
{
	this->m_pause = status;
}

void AnimPlayer::SetTime(float time)
{
	if (this->m_anim == nullptr)
		return;

	this->m_time = time;
	this->m_anim->GetHandle()->setTime(this->m_time);
}

void AnimPlayer::SetModel(FlverModel* model)
{
	this->m_model = model;

	if (model != nullptr)
		this->m_model->GetModelData();
}

void AnimPlayer::SetPlaySpeed(float speed)
{
	this->m_playSpeed = speed;
}

void AnimPlayer::StepPlay(float step)
{
	if (this->m_anim == nullptr)
		return;

	this->m_time += step;

	if (this->m_time < 0)
		this->m_time = 0;

	if (this->m_time > this->m_anim->GetHandle()->getDuration())
		this->m_time = this->m_anim->GetHandle()->getDuration();

	this->m_anim->GetHandle()->setTime(this->m_time);
}

AnimSourceInterface* AnimPlayer::GetAnimation()
{
	return this->m_anim;
}

bool AnimPlayer::IsPlaybackPaused()
{
	return this->m_pause;
}

bool AnimPlayer::IsPlaybackLoop()
{
	return this->m_loop;
}

float AnimPlayer::GetTime()
{
	return this->m_time;
}

FlverModel* AnimPlayer::GetModel()
{
	return this->m_model;
}

std::vector<int> AnimPlayer::GetFlverToMorphemeBoneMap()
{
	return this->m_flverToMorphemeBoneMap;
}

float AnimPlayer::GetPlaySpeed()
{
	return this->m_playSpeed;
}

//Creates an anim map from the flver model bone to the morpheme rig and saves it in m_morphemeToFlverRigMap
void AnimPlayer::CreateFlverToMorphemeBoneMap(MR::AnimRigDef* pMorphemeRig)
{
	this->m_flverToMorphemeBoneMap.clear();
	this->m_flverToMorphemeBoneMap.reserve(this->m_model->m_flver->header.boneCount);

	for (int i = 0; i < this->m_model->m_flver->header.boneCount; i++)
		this->m_flverToMorphemeBoneMap.push_back(GetMorphemeRigBoneIndexByFlverBoneIndex(pMorphemeRig, this->m_model, i));
}

int AnimPlayer::GetFlverBoneIndexByMorphemeBoneIndex(int idx)
{
	for (size_t i = 0; i < this->m_flverToMorphemeBoneMap.size(); i++)
	{
		if (this->m_flverToMorphemeBoneMap[i] == idx)
			return i;
	}
}