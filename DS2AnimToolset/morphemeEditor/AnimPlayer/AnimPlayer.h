#pragma once
#include "AnimObject/AnimObject.h"
#include "FlverModel/FlverModel.h"
#include "Character/Character.h"

class AnimPlayer
{
public:
	AnimPlayer();
	~AnimPlayer();
	
	void clear();
	void update(float dt);
	void reset();

	void togglePause() { this->m_pause = !this->m_pause; }

	void setCharacter(Character* character);
	void setAnim(AnimObject* anim);
	void setPause(bool status) { this->m_pause = status; }
	void setTime(float time);
	void setPlaySpeed(float speed) { this->m_playSpeed = speed; };
	void stepPlay(float step);

	Character* getCharacter() const { return this->m_character; }
	AnimObject* getAnim() const { return this->m_anim; }
	bool isPaused() const { return this->m_pause; }
	bool isLoop() const { return this->m_loop; }
	float getTime() const { return this->m_time; }
	float getPlaySpeed() const { return this->m_playSpeed; }

private:
	Character* m_character;
	AnimObject* m_anim;
	float m_time;
	bool m_pause;
	bool m_loop;
	float m_playSpeed = 1.f;
};