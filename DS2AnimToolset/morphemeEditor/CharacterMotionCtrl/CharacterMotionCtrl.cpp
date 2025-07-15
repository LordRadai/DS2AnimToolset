#include "extern.h"
#include "framework.h"
#include "CharacterMotionCtrl.h"
#include "MorphemeSystem/MorphemeUtils/MorphemeUtils.h"

namespace
{
    bool compareAnimObjs(AnimObject* first, AnimObject* second)
    {
        return std::string(first->getAnimName()) < std::string(second->getAnimName());
    }
}

bool CharacterMotionCtrlBase::initialize(const char* filename, bool doSimulateNetwork)
{
    MorphemeCharacterDef* characterDef = MorphemeSystem::createCharacterDef(filename);

    if (!characterDef)
        throw("Failed to create MorphemeCharacterDef instance (%s)", filename);

    this->m_pMorphemeCharacter = MorphemeCharacter::create(characterDef);

    if (!this->m_pMorphemeCharacter)
        throw("Failed to create MorphemeCharacter instance (%s)", filename);

	this->getMorphemeCharacter()->setDoSimulateNetwork(doSimulateNetwork);

    return true;
}

void CharacterMotionCtrlBase::destroy()
{
	MorphemeCharacter::destroy(this->m_pMorphemeCharacter);
}

void CharacterMotionCtrlBase::update(float dt)
{
    if (this->m_pMorphemeCharacter && this->m_pMorphemeCharacter->getDoSimulateNetwork())
        this->m_pMorphemeCharacter->update(dt);
}

bool CharacterMotionCtrlAnimPreview::initialize(const char* filename, bool doSimulateNetwork)
{
    if (!CharacterMotionCtrlBase::initialize(filename, doSimulateNetwork))
        return false;

	MorphemeCharacterDef* characterDef = this->m_pMorphemeCharacter->getCharacterDef();

    std::wstring animFolder = std::filesystem::path(filename).parent_path().c_str();
	const uint32_t numAnimSets = characterDef->getNetworkDef()->getNumAnimSets();
    const uint32_t animCount = characterDef->getAnimFileLookUp()->getNumAnims();

	this->m_animations.reserve(numAnimSets);

    for (uint32_t animSetIdx = 0; animSetIdx < numAnimSets; animSetIdx++)
    {
		this->m_animations.push_back(std::vector<AnimObject*>());

        g_appLog->debugMessage(MsgLevel_Debug, "\Adding animations for animSet %d:\n", animSetIdx);

        for (uint32_t i = 0; i < animCount; i++)
        {
            std::wstring animFileName = RString::toWide(characterDef->getAnimFileLookUp()->getFilename(i));
            std::wstring animFilePath = animFolder + L"\\" + animFileName;

            this->addAnimation(RString::toNarrow(animFilePath).c_str(), animSetIdx);
        }
    }

    this->sortAnimations();

    return true;
}

void CharacterMotionCtrlAnimPreview::destroy()
{
    CharacterMotionCtrlBase::destroy();

    for (size_t i = 0; i < this->m_animations.size(); i++)
        this->m_animations[i].clear();

    this->m_animations.clear();
}

AnimObject* CharacterMotionCtrlAnimPreview::getAnimation(int animSetIdx, int idx)
{
    if (idx < this->m_animations[animSetIdx].size())
        return m_animations[animSetIdx][idx];

    return nullptr;
}

AnimObject* CharacterMotionCtrlAnimPreview::getAnimationById(int animSetIdx, int id)
{
    for (size_t i = 0; i < this->m_animations[animSetIdx].size(); i++)
    {
        if (this->m_animations[animSetIdx][i]->getAnimID() == id)
            return this->m_animations[animSetIdx][i];
    }

    return nullptr;
}

void CharacterMotionCtrlAnimPreview::addAnimation(const char* filename, int animSetIdx)
{
    g_appLog->debugMessage(MsgLevel_Debug, "\tAdding animation \"%s\"\n", filename);

    const int idx = m_animations[animSetIdx].size();

	MorphemeCharacterDef* pCharacterDef = this->m_pMorphemeCharacter->getCharacterDef();
	MR::NetworkDef* pNetDef = pCharacterDef->getNetworkDef();

    this->m_animations[animSetIdx].push_back(AnimObject::createFromMorphemeAssets(pCharacterDef, pNetDef->getRig(animSetIdx), MorphemeUtils::getRigToAnimMapByAnimID(pNetDef, idx, animSetIdx), filename, idx));
}

void CharacterMotionCtrlAnimPreview::sortAnimations()
{
    for (uint32_t i = 0; i < this->m_animations.size(); i++)
        std::sort(this->m_animations[i].begin(), this->m_animations[i].end(), compareAnimObjs);
}