#pragma once
#include <vector>
#include "AnimObject/AnimObject.h"
#include "MorphemeSystem/MorphemeCharacter/MorphemeCharacter.h"

class CharacterMotionCtrlBase
{
protected:
	MorphemeCharacter* m_pMorphemeCharacter; // Pointer to the character definition this motion belongs to
public:
	CharacterMotionCtrlBase() {}

	virtual ~CharacterMotionCtrlBase() {};
	virtual bool initialize(const char* filename, bool doSimulateNetwork);
	virtual void finalize();
	virtual void update(float dt);

    MorphemeCharacter* getMorphemeCharacter() const { return this->m_pMorphemeCharacter; }
	MorphemeCharacterDef* getMorphemeCharacterDef() const { return this->m_pMorphemeCharacter->getCharacterDef(); }
	MR::Network* getNetwork() const { return this->m_pMorphemeCharacter->getNetwork(); }
	MR::NetworkDef* getNetworkDef() const { return this->m_pMorphemeCharacter->getCharacterDef()->getNetworkDef(); }
	MR::AnimRigDef* getAnimRigDef() const { return this->getNetwork()->getNetworkDef()->getRig(this->getActiveAnimSetIndex()); }
	const uint32_t getActiveAnimSetIndex() const { return this->getNetwork()->getActiveAnimSetIndex(); }
};

class CharacterMotionCtrlAnimPreview : public CharacterMotionCtrlBase
{
	std::vector<std::vector<AnimObject*>> m_animations;

public:
	CharacterMotionCtrlAnimPreview() : CharacterMotionCtrlBase() {}

	bool initialize(const char* filename, bool doSimulateNetwork) override;
	void finalize() override;

	int getNumAnimSets() const { return static_cast<int>(this->m_animations.size()); }
	int getNumAnimsInAnimSet(int animSetIdx) const { return static_cast<int>(this->m_animations[animSetIdx].size()); }

	AnimObject* getAnimation(int animSetIdx, int idx);
	AnimObject* getAnimationById(int animSetIdx, int id);
    void addAnimation(const char* filename, int animSetIdx);
	void sortAnimations();
};