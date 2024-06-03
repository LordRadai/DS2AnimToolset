#include "AnimPlayer.h"
#include "utils/NMDX/NMDX.h"
#include "utils/RString/RString.h"
#include "utils/RLog/RLog.h"
#include "extern.h"

AnimPlayer::AnimPlayer()
{
	this->m_anim = nullptr;
	this->m_time = 0.f;
	this->m_pause = true;
	this->m_loop = true;
	this->m_modelParts.m_model = nullptr;
}

AnimPlayer::~AnimPlayer()
{
}

void AnimPlayer::Clear()
{
	if (this->m_modelParts.m_model)
		delete this->m_modelParts.m_model;

	this->m_modelParts.m_model = nullptr;

	this->Reset();
}

void AnimPlayer::Update(float dt)
{
	MR::AnimationSourceHandle* animHandle = nullptr;

	if (this->m_anim)
		animHandle = this->m_anim->GetHandle();

	if (this->m_modelParts.m_model != nullptr)
	{
		this->m_modelParts.m_model->Animate(animHandle);
		this->m_modelParts.m_model->UpdateModel();
	}

	if (this->m_modelParts.m_head != nullptr)
	{
		this->m_modelParts.m_head->Animate(animHandle);
		this->m_modelParts.m_head->UpdateModel();
	}

	if (this->m_modelParts.m_face != nullptr)
	{
		this->m_modelParts.m_face->Animate(animHandle);
		this->m_modelParts.m_face->UpdateModel();
	}

	if (this->m_modelParts.m_body != nullptr)
	{
		this->m_modelParts.m_body->Animate(animHandle);
		this->m_modelParts.m_body->UpdateModel();
	}

	if (this->m_modelParts.m_arms != nullptr)
	{
		this->m_modelParts.m_arms->Animate(animHandle);
		this->m_modelParts.m_arms->UpdateModel();
	}

	if (this->m_modelParts.m_legs != nullptr)
	{
		this->m_modelParts.m_legs->Animate(animHandle);
		this->m_modelParts.m_legs->UpdateModel();
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
	this->SetModel(this->m_modelParts.m_model);
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
	this->m_modelParts.m_model = model;

	if (model != nullptr)
		this->m_modelParts.m_model->GetModelData();
}

void AnimPlayer::SetModelPart(PartType partType, FlverModel* model)
{
	switch (partType)
	{
	case Parts_Head:
		this->m_modelParts.m_head = model;

		if (model)
			this->m_modelParts.m_head->GetModelData();
		break;
	case Parts_Face:
		this->m_modelParts.m_face = model;

		if (model)
			this->m_modelParts.m_face->GetModelData();
		break;
	case Parts_Body:
		this->m_modelParts.m_body = model;

		if (model)
			this->m_modelParts.m_body->GetModelData();
		break;
	case Parts_Arm:
		this->m_modelParts.m_arms = model;

		if (model)
			this->m_modelParts.m_arms->GetModelData();
		break;
	case Parts_Leg:
		this->m_modelParts.m_legs = model;

		if (model)
			this->m_modelParts.m_legs->GetModelData();
		break;
	default:
		g_appLog->DebugMessage(MsgLevel_Error, "Invalid part type %d\n", partType);
		break;
	}
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
	return this->m_modelParts.m_model;
}

FlverModel* AnimPlayer::GetModelPart(PartType partType)
{
	switch (partType)
	{
	case Parts_Head:
		return this->m_modelParts.m_head;
	case Parts_Face:
		return this->m_modelParts.m_face;
	case Parts_Body:
		return this->m_modelParts.m_body;
	case Parts_Arm:
		return this->m_modelParts.m_arms;
	case Parts_Leg:
		return this->m_modelParts.m_legs;
	default:
		return nullptr;
	}
}

float AnimPlayer::GetPlaySpeed()
{
	return this->m_playSpeed;
}