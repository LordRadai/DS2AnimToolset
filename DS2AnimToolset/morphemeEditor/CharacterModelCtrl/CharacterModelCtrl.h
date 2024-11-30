#pragma once
#include "FlverModel/FlverModel.h"
#include "morpheme/mrAnimationSourceHandle.h"

enum PartType
{
	kPartsHead,
	kPartsFace,
	kPartsBody,
	kPartsArm,
	kPartsLeg,
	kPartsWeaponLeft,
	kPartsWeaponRight,
	kNumPartTypes,		//This must always be the last value
	kPartsInvalid = -1,
};

enum FgPartType
{
	kFgFace,
	kFgHead,
	kFgEyes,
	kFgEyeBrows,
	kFgBeard,
	kFgHair,
	kNumFgTypes,	//This must always be the last value
	kFgInvalid = -1,
};

struct ModelFaceGen
{
	FlverModel* m_fgFace;
	FlverModel* m_fgHead;
	FlverModel* m_fgEyes;
	FlverModel* m_fgEyeBrows;
	FlverModel* m_fgBeard;
	FlverModel* m_fgHair;
};

class CharacterModelCtrl
{
public:
	CharacterModelCtrl();
	~CharacterModelCtrl();

	void destroy();

	void setModel(FlverModel* model);
	void setModelPart(PartType partType, FlverModel* model);
	void setModelFg(FgPartType fgType, FlverModel* model);

	FlverModel* getModel();
	FlverModel* getModelPart(PartType partType);
	FlverModel* getModelFg(FgPartType fgType);

	void setDisplayMode(DisplayMode mode);
	void setDrawDummies(bool show);
	void setDrawBones(bool show);
	void setDrawMeshes(bool show);
	void setDrawBoundingBox(bool show);
	void setScale(float scale);

	float getScale();

	void update(float dt);
	void animate(MR::AnimationSourceHandle* animHandle);
private:
	FlverModel* m_model;

	FlverModel* m_head;
	FlverModel* m_face;
	FlverModel* m_body;
	FlverModel* m_arm;
	FlverModel* m_leg;

	FlverModel* m_weaponLeft;
	FlverModel* m_weaponRight;

	ModelFaceGen m_faceGen;
};