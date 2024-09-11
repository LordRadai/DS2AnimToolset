#include "CharacterModelCtrl.h"
#include "framework.h"
#include "extern.h"

CharacterModelCtrl::CharacterModelCtrl()
{
}

CharacterModelCtrl::~CharacterModelCtrl()
{
}

void CharacterModelCtrl::destroy()
{
	if (this->m_model)
		this->m_model->destroy();

	if (this->m_head)
		this->m_head->destroy();

	if (this->m_face)
		this->m_face->destroy();

	if (this->m_body)
		this->m_body->destroy();

	if (this->m_arm)
		this->m_arm->destroy();

	if (this->m_leg)
		this->m_leg->destroy();

	if (this->m_weaponRight)
		this->m_weaponRight->destroy();

	if (this->m_weaponLeft)
		this->m_weaponLeft->destroy();

	if (this->m_faceGen.m_fgFace)
		this->m_faceGen.m_fgFace->destroy();

	if (this->m_faceGen.m_fgHead)
		this->m_faceGen.m_fgHead->destroy();

	if (this->m_faceGen.m_fgEyes)
		this->m_faceGen.m_fgEyes->destroy();

	if (this->m_faceGen.m_fgEyeBrows)
		this->m_faceGen.m_fgEyeBrows->destroy();

	if (this->m_faceGen.m_fgBeard)
		this->m_faceGen.m_fgBeard->destroy();

	if (this->m_faceGen.m_fgHair)
		this->m_faceGen.m_fgHair->destroy();

	this->m_model = nullptr;
	this->m_head = nullptr;
	this->m_face = nullptr;
	this->m_body = nullptr;
	this->m_arm = nullptr;
	this->m_leg = nullptr;
	this->m_weaponRight = nullptr;
	this->m_weaponLeft = nullptr;
	this->m_faceGen.m_fgFace = nullptr;
	this->m_faceGen.m_fgHead = nullptr;
	this->m_faceGen.m_fgEyes = nullptr;
	this->m_faceGen.m_fgEyeBrows = nullptr;
	this->m_faceGen.m_fgBeard = nullptr;
	this->m_faceGen.m_fgHair = nullptr;

	delete this;
}

void CharacterModelCtrl::setModel(FlverModel* model)
{
	if (model)
		model->initModelData();

	if (this->m_model)
		this->m_model->destroy();

	this->m_model = model;
}

void CharacterModelCtrl::setModelPart(PartType partType, FlverModel* model)
{
	switch (partType)
	{
	case kPartsHead:
		if (this->m_head)
			this->m_head->destroy();

		this->m_head = model;
		break;
	case kPartsFace:
		if (this->m_face)
			this->m_face->destroy();

		this->m_face = model;
		break;
	case kPartsBody:
		if (this->m_body)
			this->m_body->destroy();

		this->m_body = model;
		break;
	case kPartsArm:
		if (this->m_arm)
			this->m_arm->destroy();

		this->m_arm = model;
		break;
	case kPartsLeg:
		if (this->m_leg)
			this->m_leg->destroy();

		this->m_leg = model;
		break;
	case kPartsWeaponLeft:
		if (this->m_weaponLeft)
			this->m_weaponLeft->destroy();

		this->m_weaponLeft = model;
		break;
	case kPartsWeaponRight:
		if (this->m_weaponRight)
			this->m_weaponRight->destroy();

		this->m_weaponRight = model;
		break;
	default:
		g_appLog->debugMessage(MsgLevel_Error, "Invalid part type %d\n", partType);
		break;
	}
}

void CharacterModelCtrl::setModelFg(FgPartType fgType, FlverModel* model)
{
	switch (fgType)
	{
	case kFgFace:
		if (this->m_faceGen.m_fgFace)
			this->m_faceGen.m_fgFace->destroy();

		this->m_faceGen.m_fgFace = model;
		break;
	case kFgHead:
		if (this->m_faceGen.m_fgHead)
			this->m_faceGen.m_fgHead->destroy();

		this->m_faceGen.m_fgHead = model;
		break;
	case kFgEyes:
		if (this->m_faceGen.m_fgEyes)
			this->m_faceGen.m_fgEyes->destroy();

		this->m_faceGen.m_fgEyes = model;
		break;
	case kFgEyeBrows:
		if (this->m_faceGen.m_fgEyeBrows)
			this->m_faceGen.m_fgEyeBrows->destroy();

		this->m_faceGen.m_fgEyeBrows = model;
		break;
	case kFgBeard:
		if (this->m_faceGen.m_fgBeard)
			this->m_faceGen.m_fgBeard->destroy();

		this->m_faceGen.m_fgBeard = model;
		break;
	case kFgHair:
		if (this->m_faceGen.m_fgHair)
			this->m_faceGen.m_fgHair->destroy();

		this->m_faceGen.m_fgHair = model;
		break;
	default:
		break;
	}
}


FlverModel* CharacterModelCtrl::getModel()
{
	return this->m_model;
}

FlverModel* CharacterModelCtrl::getModelPart(PartType partType)
{
	if (partType >= kNumPartTypes)
	{
		g_appLog->debugMessage(MsgLevel_Error, "Requesting facegen outside of the maximum allowed bounadry (request=%d, max=%d)\n", partType, kNumPartTypes);
		return nullptr;
	}

	switch (partType)
	{
	case kPartsHead:
		return this->m_head;
	case kPartsFace:
		return this->m_face;
	case kPartsBody:
		return this->m_body;
	case kPartsArm:
		return this->m_arm;
	case kPartsLeg:
		return this->m_leg;
	case kPartsWeaponLeft:
		return this->m_weaponLeft;
	case kPartsWeaponRight:
		return this->m_weaponRight;
	default:
		return nullptr;
	}
}

FlverModel* CharacterModelCtrl::getModelFg(FgPartType fgType)
{
	if (fgType >= kNumFgTypes)
	{
		g_appLog->debugMessage(MsgLevel_Error, "Requesting facegen outside of the maximum allowed bounadry (request=%d, max=%d)\n", fgType, kNumFgTypes);
		return nullptr;
	}

	switch (fgType)
	{
	case kFgFace:
		return this->m_faceGen.m_fgFace;
	case kFgHead:
		return this->m_faceGen.m_fgHead;
		break;
	case kFgEyes:
		return this->m_faceGen.m_fgEyes;
		break;
	case kFgEyeBrows:
		return this->m_faceGen.m_fgEyeBrows;
		break;
	case kFgBeard:
		return this->m_faceGen.m_fgBeard;
		break;
	case kFgHair:
		return this->m_faceGen.m_fgHair;
		break;
	default:
		return nullptr;
	}
}

void CharacterModelCtrl::setDisplayMode(DisplayMode mode)
{
	if (this->m_model)
		this->m_model->getSettings()->displayMode = mode;

	if (this->m_head)
		this->m_head->getSettings()->displayMode = mode;

	if (this->m_face)
		this->m_face->getSettings()->displayMode = mode;

	if (this->m_body)
		this->m_body->getSettings()->displayMode = mode;

	if (this->m_arm)
		this->m_arm->getSettings()->displayMode = mode;

	if (this->m_leg)
		this->m_leg->getSettings()->displayMode = mode;

	if (this->m_weaponRight)
		this->m_weaponRight->getSettings()->displayMode = mode;

	if (this->m_weaponLeft)
		this->m_weaponLeft->getSettings()->displayMode = mode;

	if (this->m_faceGen.m_fgFace)
		this->m_faceGen.m_fgFace->getSettings()->displayMode = mode;

	if (this->m_faceGen.m_fgHead)
		this->m_faceGen.m_fgHead->getSettings()->displayMode = mode;

	if (this->m_faceGen.m_fgEyes)
		this->m_faceGen.m_fgEyes->getSettings()->displayMode = mode;

	if (this->m_faceGen.m_fgEyeBrows)
		this->m_faceGen.m_fgEyeBrows->getSettings()->displayMode = mode;

	if (this->m_faceGen.m_fgBeard)
		this->m_faceGen.m_fgBeard->getSettings()->displayMode = mode;

	if (this->m_faceGen.m_fgHair)
		this->m_faceGen.m_fgHair->getSettings()->displayMode = mode;
}

void CharacterModelCtrl::setShowDummies(bool show)
{
	if (this->m_model)
		this->m_model->getSettings()->drawDummyPolygons = show;

	if (this->m_head)
		this->m_head->getSettings()->drawDummyPolygons = show;

	if (this->m_face)
		this->m_face->getSettings()->drawDummyPolygons = show;

	if (this->m_body)
		this->m_body->getSettings()->drawDummyPolygons = show;

	if (this->m_arm)
		this->m_arm->getSettings()->drawDummyPolygons = show;

	if (this->m_leg)
		this->m_leg->getSettings()->drawDummyPolygons = show;

	if (this->m_weaponRight)
		this->m_weaponRight->getSettings()->drawDummyPolygons = show;

	if (this->m_weaponLeft)
		this->m_weaponLeft->getSettings()->drawDummyPolygons = show;

	if (this->m_faceGen.m_fgFace)
		this->m_faceGen.m_fgFace->getSettings()->drawDummyPolygons = show;

	if (this->m_faceGen.m_fgHead)
		this->m_faceGen.m_fgHead->getSettings()->drawDummyPolygons = show;

	if (this->m_faceGen.m_fgEyes)
		this->m_faceGen.m_fgEyes->getSettings()->drawDummyPolygons = show;

	if (this->m_faceGen.m_fgEyeBrows)
		this->m_faceGen.m_fgEyeBrows->getSettings()->drawDummyPolygons = show;

	if (this->m_faceGen.m_fgBeard)
		this->m_faceGen.m_fgBeard->getSettings()->drawDummyPolygons = show;

	if (this->m_faceGen.m_fgHair)
		this->m_faceGen.m_fgHair->getSettings()->drawDummyPolygons = show;
}

void CharacterModelCtrl::setDrawBones(bool show)
{
	if (this->m_model)
		this->m_model->getSettings()->drawBones = show;

	if (this->m_head)
		this->m_head->getSettings()->drawBones = false;

	if (this->m_face)
		this->m_face->getSettings()->drawBones = false;

	if (this->m_body)
		this->m_body->getSettings()->drawBones = false;

	if (this->m_arm)
		this->m_arm->getSettings()->drawBones = false;

	if (this->m_leg)
		this->m_leg->getSettings()->drawBones = false;

	if (this->m_weaponRight)
		this->m_weaponRight->getSettings()->drawBones = false;

	if (this->m_weaponLeft)
		this->m_weaponLeft->getSettings()->drawBones = false;

	if (this->m_faceGen.m_fgFace)
		this->m_faceGen.m_fgFace->getSettings()->drawBones = false;

	if (this->m_faceGen.m_fgHead)
		this->m_faceGen.m_fgHead->getSettings()->drawBones = false;

	if (this->m_faceGen.m_fgEyes)
		this->m_faceGen.m_fgEyes->getSettings()->drawBones = false;

	if (this->m_faceGen.m_fgEyeBrows)
		this->m_faceGen.m_fgEyeBrows->getSettings()->drawBones = false;

	if (this->m_faceGen.m_fgBeard)
		this->m_faceGen.m_fgBeard->getSettings()->drawBones = false;

	if (this->m_faceGen.m_fgHair)
		this->m_faceGen.m_fgHair->getSettings()->drawBones = false;
}

void CharacterModelCtrl::setDrawBoundingBox(bool show)
{
	if (this->m_model)
		this->m_model->getSettings()->drawBoundingBox = show;

	if (this->m_head)
		this->m_head->getSettings()->drawBoundingBox = false;

	if (this->m_face)
		this->m_face->getSettings()->drawBoundingBox = false;

	if (this->m_body)
		this->m_body->getSettings()->drawBoundingBox = false;

	if (this->m_arm)
		this->m_arm->getSettings()->drawBoundingBox = false;

	if (this->m_leg)
		this->m_leg->getSettings()->drawBoundingBox = false;

	if (this->m_weaponRight)
		this->m_weaponRight->getSettings()->drawBoundingBox = false;

	if (this->m_weaponLeft)
		this->m_weaponLeft->getSettings()->drawBoundingBox = false;

	if (this->m_faceGen.m_fgFace)
		this->m_faceGen.m_fgFace->getSettings()->drawBoundingBox = false;

	if (this->m_faceGen.m_fgHead)
		this->m_faceGen.m_fgHead->getSettings()->drawBoundingBox = false;

	if (this->m_faceGen.m_fgEyes)
		this->m_faceGen.m_fgEyes->getSettings()->drawBoundingBox = false;

	if (this->m_faceGen.m_fgEyeBrows)
		this->m_faceGen.m_fgEyeBrows->getSettings()->drawBoundingBox = false;

	if (this->m_faceGen.m_fgBeard)
		this->m_faceGen.m_fgBeard->getSettings()->drawBoundingBox = false;

	if (this->m_faceGen.m_fgHair)
		this->m_faceGen.m_fgHair->getSettings()->drawBoundingBox = false;
}

void CharacterModelCtrl::setScale(float scale)
{
	if (this->m_model)
		this->m_model->setScale(scale);

	if (this->m_head)
		this->m_head->setScale(scale);

	if (this->m_face)
		this->m_face->setScale(scale);

	if (this->m_body)
		this->m_body->setScale(scale);

	if (this->m_arm)
		this->m_arm->setScale(scale);

	if (this->m_leg)
		this->m_leg->setScale(scale);

	if (this->m_weaponRight)
		this->m_weaponRight->setScale(scale);

	if (this->m_weaponLeft)
		this->m_weaponLeft->setScale(scale);

	if (this->m_faceGen.m_fgFace)
		this->m_faceGen.m_fgFace->setScale(scale);

	if (this->m_faceGen.m_fgHead)
		this->m_faceGen.m_fgHead->setScale(scale);

	if (this->m_faceGen.m_fgEyes)
		this->m_faceGen.m_fgEyes->setScale(scale);

	if (this->m_faceGen.m_fgEyeBrows)
		this->m_faceGen.m_fgEyeBrows->setScale(scale);

	if (this->m_faceGen.m_fgBeard)
		this->m_faceGen.m_fgBeard->setScale(scale);

	if (this->m_faceGen.m_fgHair)
		this->m_faceGen.m_fgHair->setScale(scale);
}

float CharacterModelCtrl::getScale()
{
	if (this->m_model)
		return this->m_model->getScale();

	return 1.f;
}

void CharacterModelCtrl::update(float dt)
{
	if (this->m_model)
		this->m_model->update(dt);

	if (this->m_head)
		this->m_head->update(dt);

	if (this->m_face)
		this->m_face->update(dt);

	if (this->m_body)
		this->m_body->update(dt);

	if (this->m_arm)
		this->m_arm->update(dt);

	if (this->m_leg)
		this->m_leg->update(dt);

	if (this->m_weaponRight)
	{
		Matrix rotation = Matrix::CreateRotationY(-DirectX::XM_PIDIV2) * Matrix::CreateRotationX(-DirectX::XM_PIDIV2);

		this->m_weaponRight->setPosition(rotation * this->m_model->getDummyPolygonTransform(20));
		this->m_weaponRight->update(dt);
	}

	if (this->m_weaponLeft)
	{
		Matrix rotation = Matrix::CreateRotationY(-DirectX::XM_PIDIV2) * Matrix::CreateRotationX(-DirectX::XM_PIDIV2);

		this->m_weaponLeft->setPosition(rotation * this->m_model->getDummyPolygonTransform(21));
		this->m_weaponLeft->update(dt);
	}

	if (this->m_faceGen.m_fgFace)
		this->m_faceGen.m_fgFace->update(dt);

	if (this->m_faceGen.m_fgHead)
		this->m_faceGen.m_fgHead->update(dt);

	if (this->m_faceGen.m_fgEyes)
		this->m_faceGen.m_fgEyes->update(dt);

	if (this->m_faceGen.m_fgEyeBrows)
		this->m_faceGen.m_fgEyeBrows->update(dt);

	if (this->m_faceGen.m_fgBeard)
		this->m_faceGen.m_fgBeard->update(dt);

	if (this->m_faceGen.m_fgHair)
		this->m_faceGen.m_fgHair->update(dt);
}

void CharacterModelCtrl::animate(MR::AnimationSourceHandle* animHandle)
{
	if (this->m_model)
		this->m_model->animate(animHandle);

	if (this->m_head)
		this->m_head->animate(animHandle);

	if (this->m_face)
		this->m_face->animate(animHandle);

	if (this->m_body)
		this->m_body->animate(animHandle);

	if (this->m_arm)
		this->m_arm->animate(animHandle);

	if (this->m_leg)
		this->m_leg->animate(animHandle);

	/*
	if (this->m_weaponRight)
		this->m_weaponRight->animate(animHandle);

	if (this->m_weaponLeft)
		this->m_weaponLeft->animate(animHandle);
	*/

	if (this->m_faceGen.m_fgFace)
		this->m_faceGen.m_fgFace->animate(animHandle);

	if (this->m_faceGen.m_fgHead)
		this->m_faceGen.m_fgHead->animate(animHandle);

	if (this->m_faceGen.m_fgEyes)
		this->m_faceGen.m_fgEyes->animate(animHandle);

	if (this->m_faceGen.m_fgEyeBrows)
		this->m_faceGen.m_fgEyeBrows->animate(animHandle);

	if (this->m_faceGen.m_fgBeard)
		this->m_faceGen.m_fgBeard->animate(animHandle);

	if (this->m_faceGen.m_fgHair)
		this->m_faceGen.m_fgHair->animate(animHandle);
}