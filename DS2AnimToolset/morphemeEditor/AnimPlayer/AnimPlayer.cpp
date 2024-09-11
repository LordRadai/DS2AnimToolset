#include "AnimPlayer.h"
#include "utils/NMDX/NMDX.h"
#include "RCore.h"
#include "extern.h"

AnimPlayer::AnimPlayer()
{
	this->m_anim = nullptr;
	this->m_time = 0.f;
	this->m_pause = true;
	this->m_loop = true;
}

AnimPlayer::~AnimPlayer()
{
}

void AnimPlayer::clear()
{
	this->m_character = nullptr;

	this->reset();
}

void AnimPlayer::update(float dt)
{
	MR::AnimationSourceHandle* animHandle = nullptr;

	if (this->m_anim)
		animHandle = this->m_anim->getHandle();

	if (animHandle == nullptr)
	{
		this->m_pause = true;
		return;
	}

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

	if (this->m_character)
		this->m_character->getCharacterModelCtrl()->animate(animHandle);
}

void AnimPlayer::reset()
{
	this->m_anim = nullptr;
	this->m_time = 0.f;
}

void AnimPlayer::setCharacter(Character* character) 
{ 
	this->reset(); 
	this->m_character = character; 
}

void AnimPlayer::setAnim(AnimObject* anim) 
{ 
	this->reset(); 
	this->m_anim = anim; 
}

void AnimPlayer::setTime(float time)
{
	this->m_time = time;

	if (this->m_anim == nullptr)
		return;

	this->m_anim->setAnimTime(this->m_time);
}

void AnimPlayer::stepPlay(float step)
{
	if (this->m_anim == nullptr)
		return;

	this->m_time += step;

	if (this->m_time < 0)
		this->m_time = 0;

	if (this->m_time > this->m_anim->getHandle()->getDuration())
		this->m_time = this->m_anim->getHandle()->getDuration();

	this->m_anim->getHandle()->setTime(this->m_time);
}