#pragma once
#include "AnimSourceInterface/AnimSourceInterface.h"
#include "FlverModel/FlverModel.h"

enum PartType
{
	Parts_Head,
	Parts_Face,
	Parts_Body,
	Parts_Arm,
	Parts_Leg,
	Parts_WeaponLeft,
	Parts_WeaponRight,
};

struct ModelParts
{
	FlverModel* m_model;

	FlverModel* m_head;
	FlverModel* m_face;
	FlverModel* m_body;
	FlverModel* m_arm;
	FlverModel* m_leg;

	FlverModel* m_weaponLeft;
	FlverModel* m_weaponRight;
};

class AnimPlayer
{
public:
	AnimPlayer();
	~AnimPlayer();
	
	void Clear();
	void Update(float dt);
	void Reset();
	void TogglePause();

	void SetAnimation(AnimSourceInterface* anim);
	void SetPause(bool status);
	void SetTime(float time);
	void SetModel(FlverModel* model);
	void SetModelPart(PartType partType, FlverModel* model);
	void SetPlaySpeed(float speed);
	void StepPlay(float step);

	AnimSourceInterface* GetAnimation();
	bool IsPlaybackPaused();
	bool IsPlaybackLoop();
	float GetTime();
	FlverModel* GetModel();
	FlverModel* GetModelPart(PartType partType);
	float GetPlaySpeed();

private:
	AnimSourceInterface* m_anim;
	float m_time;
	bool m_pause;
	bool m_loop;
	ModelParts m_modelParts;
	float m_playSpeed = 1.f;
};