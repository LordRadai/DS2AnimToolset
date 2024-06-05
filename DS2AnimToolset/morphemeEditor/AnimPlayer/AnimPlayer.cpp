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

	if (this->m_modelParts.m_head)
		delete this->m_modelParts.m_head;

	if (this->m_modelParts.m_face)
		delete this->m_modelParts.m_face;

	if (this->m_modelParts.m_body)
		delete this->m_modelParts.m_body;

	if (this->m_modelParts.m_arm)
		delete this->m_modelParts.m_arm;

	if (this->m_modelParts.m_leg)
		delete this->m_modelParts.m_leg;

	if (this->m_modelParts.m_weaponRight)
		delete this->m_modelParts.m_weaponRight;

	if (this->m_modelParts.m_weaponLeft)
		delete this->m_modelParts.m_weaponLeft;

	if (this->m_modelParts.m_faceGen.m_fgFace)
		delete this->m_modelParts.m_faceGen.m_fgFace;

	if (this->m_modelParts.m_faceGen.m_fgHead)
		delete this->m_modelParts.m_faceGen.m_fgHead;

	if (this->m_modelParts.m_faceGen.m_fgEyes)
		delete this->m_modelParts.m_faceGen.m_fgEyes;

	if (this->m_modelParts.m_faceGen.m_fgEyeBrows)
		delete this->m_modelParts.m_faceGen.m_fgEyeBrows;

	if (this->m_modelParts.m_faceGen.m_fgBeard)
		delete this->m_modelParts.m_faceGen.m_fgBeard;

	if (this->m_modelParts.m_faceGen.m_fgHair)
		delete this->m_modelParts.m_faceGen.m_fgHair;

	this->m_modelParts.m_model = nullptr;
	this->m_modelParts.m_head = nullptr;
	this->m_modelParts.m_face = nullptr;
	this->m_modelParts.m_body = nullptr;
	this->m_modelParts.m_arm = nullptr;
	this->m_modelParts.m_leg = nullptr;
	this->m_modelParts.m_weaponRight = nullptr;
	this->m_modelParts.m_weaponLeft = nullptr;
	this->m_modelParts.m_faceGen.m_fgFace = nullptr;
	this->m_modelParts.m_faceGen.m_fgHead = nullptr;
	this->m_modelParts.m_faceGen.m_fgEyes = nullptr;
	this->m_modelParts.m_faceGen.m_fgEyeBrows = nullptr;
	this->m_modelParts.m_faceGen.m_fgBeard = nullptr;
	this->m_modelParts.m_faceGen.m_fgHair = nullptr;

	this->Reset();
}

void AnimPlayer::Update(float dt)
{
	MR::AnimationSourceHandle* animHandle = nullptr;

	if (this->m_anim)
		animHandle = this->m_anim->GetHandle();

	if (this->m_modelParts.m_model)
	{
		this->m_modelParts.m_model->UpdateModel();
		this->m_modelParts.m_model->Animate(animHandle);
	}

	if (this->m_modelParts.m_head)
	{
		this->m_modelParts.m_head->UpdateModel();
		this->m_modelParts.m_head->Animate(animHandle);
	}

	if (this->m_modelParts.m_face)
	{
		this->m_modelParts.m_face->UpdateModel();
		this->m_modelParts.m_face->Animate(animHandle);
	}

	if (this->m_modelParts.m_body)
	{
		this->m_modelParts.m_body->UpdateModel();
		this->m_modelParts.m_body->Animate(animHandle);
	}

	if (this->m_modelParts.m_arm)
	{
		this->m_modelParts.m_arm->UpdateModel();
		this->m_modelParts.m_arm->Animate(animHandle);
	}

	if (this->m_modelParts.m_leg)
	{
		this->m_modelParts.m_leg->UpdateModel();
		this->m_modelParts.m_leg->Animate(animHandle);
	}

	if (this->m_modelParts.m_weaponLeft)
	{
		this->m_modelParts.m_weaponLeft->UpdateModel();
		this->m_modelParts.m_weaponLeft->m_position = Matrix::CreateRotationY(-DirectX::XM_PIDIV2) * Matrix::CreateRotationX(-DirectX::XM_PIDIV2) * this->m_modelParts.m_model->GetDummyPolygonTransform(21) * this->m_modelParts.m_model->m_position;
	}

	if (this->m_modelParts.m_weaponRight)
	{
		this->m_modelParts.m_weaponRight->UpdateModel();
		this->m_modelParts.m_weaponRight->m_position = Matrix::CreateRotationY(-DirectX::XM_PIDIV2) * Matrix::CreateRotationX(-DirectX::XM_PIDIV2) * this->m_modelParts.m_model->GetDummyPolygonTransform(20) * this->m_modelParts.m_model->m_position;
	}

	if (this->m_modelParts.m_faceGen.m_fgFace)
	{
		this->m_modelParts.m_faceGen.m_fgFace->UpdateModel();
		this->m_modelParts.m_faceGen.m_fgFace->Animate(animHandle);
	}

	if (this->m_modelParts.m_faceGen.m_fgHead)
	{
		this->m_modelParts.m_faceGen.m_fgHead->UpdateModel();
		this->m_modelParts.m_faceGen.m_fgHead->Animate(animHandle);
	}

	if (this->m_modelParts.m_faceGen.m_fgEyes)
	{
		this->m_modelParts.m_faceGen.m_fgEyes->UpdateModel();
		this->m_modelParts.m_faceGen.m_fgEyes->Animate(animHandle);
	}

	if (this->m_modelParts.m_faceGen.m_fgEyeBrows)
	{
		this->m_modelParts.m_faceGen.m_fgEyeBrows->UpdateModel();
		this->m_modelParts.m_faceGen.m_fgEyeBrows->Animate(animHandle);
	}

	if (this->m_modelParts.m_faceGen.m_fgBeard)
	{
		this->m_modelParts.m_faceGen.m_fgBeard->UpdateModel();
		this->m_modelParts.m_faceGen.m_fgBeard->Animate(animHandle);
	}

	if (this->m_modelParts.m_faceGen.m_fgHair)
	{
		this->m_modelParts.m_faceGen.m_fgHair->UpdateModel();
		this->m_modelParts.m_faceGen.m_fgHair->Animate(animHandle);
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
	if (model)
		model->GetModelData();

	if (this->m_modelParts.m_model)
		delete this->m_modelParts.m_model;

	this->m_modelParts.m_model = model;
}

void AnimPlayer::SetModelPart(PartType partType, FlverModel* model)
{
	if (model)
		model->GetModelData();

	switch (partType)
	{
	case Parts_Head:
		if (this->m_modelParts.m_head)
			delete this->m_modelParts.m_head;

		this->m_modelParts.m_head = model;
		break;
	case Parts_Face:
		if (this->m_modelParts.m_face)
			delete this->m_modelParts.m_face;

		this->m_modelParts.m_face = model;
		break;
	case Parts_Body:
		if (this->m_modelParts.m_body)
			delete this->m_modelParts.m_body;

		this->m_modelParts.m_body = model;
		break;
	case Parts_Arm:
		if (this->m_modelParts.m_arm)
			delete this->m_modelParts.m_arm;

		this->m_modelParts.m_arm = model;
		break;
	case Parts_Leg:
		if (this->m_modelParts.m_leg)
			delete this->m_modelParts.m_leg;

		this->m_modelParts.m_leg = model;
		break;
	case Parts_WeaponLeft:
		if (this->m_modelParts.m_weaponLeft)
			delete this->m_modelParts.m_weaponLeft;

		this->m_modelParts.m_weaponLeft = model;
		break;
	case Parts_WeaponRight:
		if (this->m_modelParts.m_weaponRight)
			delete this->m_modelParts.m_weaponRight;

		this->m_modelParts.m_weaponRight = model;
		break;
	default:
		g_appLog->DebugMessage(MsgLevel_Error, "Invalid part type %d\n", partType);
		break;
	}
}

void AnimPlayer::SetModelPartFacegen(FgPartType fgType, FlverModel* model)
{
	if (model)
		model->GetModelData();

	switch (fgType)
	{
	case FaceGen_Face:
		if (this->m_modelParts.m_faceGen.m_fgFace)
			delete this->m_modelParts.m_faceGen.m_fgFace;

		this->m_modelParts.m_faceGen.m_fgFace = model;
		break;
	case FaceGen_Head:
		if (this->m_modelParts.m_faceGen.m_fgHead)
			delete this->m_modelParts.m_faceGen.m_fgHead;

		this->m_modelParts.m_faceGen.m_fgHead = model;
		break;
	case FaceGen_Eyes:
		if (this->m_modelParts.m_faceGen.m_fgEyes)
			delete this->m_modelParts.m_faceGen.m_fgEyes;

		this->m_modelParts.m_faceGen.m_fgEyes = model;
		break;
	case FaceGen_EyeBrows:
		if (this->m_modelParts.m_faceGen.m_fgEyeBrows)
			delete this->m_modelParts.m_faceGen.m_fgEyeBrows;

		this->m_modelParts.m_faceGen.m_fgEyeBrows = model;
		break;
	case FaceGen_Beard:
		if (this->m_modelParts.m_faceGen.m_fgBeard)
			delete this->m_modelParts.m_faceGen.m_fgBeard;

		this->m_modelParts.m_faceGen.m_fgBeard = model;
		break;
	case FaceGen_Hair:
		if (this->m_modelParts.m_faceGen.m_fgHair)
			delete this->m_modelParts.m_faceGen.m_fgHair;

		this->m_modelParts.m_faceGen.m_fgHair = model;
		break;
	default:
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
	if (partType >= Parts_MaxValue)
	{
		g_appLog->DebugMessage(MsgLevel_Error, "Requesting facegen outside of the maximum allowed bounadry (request=%d, max=%d)\n", partType, Parts_MaxValue);
		return nullptr;
	}

	switch (partType)
	{
	case Parts_Head:
		return this->m_modelParts.m_head;
	case Parts_Face:
		return this->m_modelParts.m_face;
	case Parts_Body:
		return this->m_modelParts.m_body;
	case Parts_Arm:
		return this->m_modelParts.m_arm;
	case Parts_Leg:
		return this->m_modelParts.m_leg;
	case Parts_WeaponLeft:
		return this->m_modelParts.m_weaponLeft;
	case Parts_WeaponRight:
		return this->m_modelParts.m_weaponRight;
	default:
		return nullptr;
	}
}

FlverModel* AnimPlayer::GetModelPartFacegen(FgPartType fgType)
{
	if (fgType >= FaceGen_MaxValue)
	{
		g_appLog->DebugMessage(MsgLevel_Error, "Requesting facegen outside of the maximum allowed bounadry (request=%d, max=%d)\n", fgType, FaceGen_MaxValue);
		return nullptr;
	}

	switch (fgType)
	{
	case FaceGen_Face:
		return this->m_modelParts.m_faceGen.m_fgFace;
	case FaceGen_Head:
		return this->m_modelParts.m_faceGen.m_fgHead;
		break;
	case FaceGen_Eyes:
		return this->m_modelParts.m_faceGen.m_fgEyes;
		break;
	case FaceGen_EyeBrows:
		return this->m_modelParts.m_faceGen.m_fgEyeBrows;
		break;
	case FaceGen_Beard:
		return this->m_modelParts.m_faceGen.m_fgBeard;
		break;
	case FaceGen_Hair:
		return this->m_modelParts.m_faceGen.m_fgHair;
		break;
	default:
		return nullptr;
	}
}

float AnimPlayer::GetPlaySpeed()
{
	return this->m_playSpeed;
}