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
	virtual void destroy();
	virtual void update(float dt);

    MorphemeCharacter* getMorphemeCharacter() const { return this->m_pMorphemeCharacter; }
	MorphemeCharacterDef* getMorphemeCharacterDef() const { return this->m_pMorphemeCharacter->getCharacterDef(); }
	MR::Network* getNetwork() const { return this->m_pMorphemeCharacter->getNetwork(); }
	MR::NetworkDef* getNetworkDef() const { return this->m_pMorphemeCharacter->getCharacterDef()->getNetworkDef(); }
	MR::AnimRigDef* getAnimRigDef() const { return this->getNetwork()->getNetworkDef()->getRig(this->getActiveAnimSetIndex()); }
	MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* getAnimFileLookUpTable() const { return this->getMorphemeCharacterDef()->getAnimFileLookUp(); }
	const uint32_t getActiveAnimSetIndex() const { return this->getNetwork()->getActiveAnimSetIndex(); }

	bool sendRequest(uint32_t requestID, bool status = true);

	std::vector<MR::NodeID> getActiveNodeIDs() const;
	bool canSendMessage(uint32_t messageID) const;

	void setControlParamFloat(MR::NodeID cpID, float value);
	void setControlParamInt(MR::NodeID cpID, int value);
	void setControlParamUInt(MR::NodeID cpID, uint32_t value);
	void setControlParamBool(MR::NodeID cpID, bool value);
	void setControlParamVector3(MR::NodeID cpID, const NMP::Vector3& value);
	void setControlParamVector4(MR::NodeID cpID, const NMP::Quat& value);

	float getControlParamFloat(MR::NodeID cpID) const;
	int getControlParamInt(MR::NodeID cpID) const;
	uint32_t getControlParamUInt(MR::NodeID cpID) const;
	bool getControlParamBool(MR::NodeID cpID) const;
	NMP::Vector3 getControlParamVector3(MR::NodeID cpID) const;
	NMP::Quat getControlParamVector4(MR::NodeID cpID) const;

	NMP::Quat getRotationChange();
	NMP::Vector3 getTranslationChange();
};

class CharacterMotionCtrlAnimPreview : public CharacterMotionCtrlBase
{
	std::vector<std::vector<AnimObject*>> m_animations;

public:
	CharacterMotionCtrlAnimPreview() : CharacterMotionCtrlBase() {}

	bool initialize(const char* filename, bool doSimulateNetwork) override;
	void destroy() override;

	int getNumAnimSets() const { return static_cast<int>(this->m_animations.size()); }
	int getNumAnimsInAnimSet(int animSetIdx) const { return static_cast<int>(this->m_animations[animSetIdx].size()); }

	AnimObject* getAnimation(int animSetIdx, int idx);
	AnimObject* getAnimationById(int animSetIdx, int id);
    void addAnimation(const char* filename, int animSetIdx);
	void sortAnimations();
};