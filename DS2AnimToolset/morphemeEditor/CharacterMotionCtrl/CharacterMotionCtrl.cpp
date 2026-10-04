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

    this->m_pMorphemeCharacter = MorphemeCharacter::create(characterDef, doSimulateNetwork);

    if (!this->m_pMorphemeCharacter)
        throw("Failed to create MorphemeCharacter instance (%s)", filename);

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

bool CharacterMotionCtrlBase::sendRequest(uint32_t requestID, bool status)
{
	MR::Network* network = this->getNetwork();

    if (!network || requestID == -1)
        return false;

	MR::Message requestMessage(requestID, MESSAGE_TYPE_REQUEST, status, nullptr, 0);

	return network->broadcastMessage(requestMessage) > 0;
}

std::vector<MR::NodeID> CharacterMotionCtrlBase::getActiveNodeIDs() const
{
    std::vector<MR::NodeID> activeNodes;

    MR::Network* network = this->getNetwork();

    if (!network)
        return activeNodes;

	const uint32_t numActiveNodes = network->getActiveNodeCount();
	MR::NodeID* activeNodeIDs = new MR::NodeID[numActiveNodes];

    network->getActiveNodes(activeNodeIDs, numActiveNodes);

    for (uint32_t i = 0; i < numActiveNodes; i++)
		activeNodes.push_back(activeNodeIDs[i]);

	delete[] activeNodeIDs;

    return activeNodes;
}

bool CharacterMotionCtrlBase::canSendMessage(uint32_t messageID) const
{
    if (messageID == -1)
        return false;

	MR::Network* network = this->getNetwork();
	MR::NetworkDef* networkDef = this->getNetworkDef();

    const MR::MessageDistributor* msgDist = networkDef->getMessageDistributor(messageID);
    const std::string messageName = networkDef->getMessageNameFromMessageID(msgDist->m_messageID);

    bool canSendMessage = false;

    for (size_t msgDistNodeIdx = 0; msgDistNodeIdx < msgDist->m_numNodeIDs; msgDistNodeIdx++)
    {
        const MR::NodeID nodeID = msgDist->m_nodeIDs[msgDistNodeIdx];

        if (network->nodeIsActive(nodeID))
			return true;
    }

    return false;
}

void CharacterMotionCtrlBase::setControlParamFloat(MR::NodeID cpID, float value)
{
    MR::Network* network = this->getNetwork();

    if (!network)
        return;

	MR::AttribDataFloat* cpData = (MR::AttribDataFloat*)network->getControlParameter(cpID);

    NMP_ASSERT(cpData);

    cpData->m_value = value;

	network->setControlParameter(cpID, cpData);
}

void CharacterMotionCtrlBase::setControlParamInt(MR::NodeID cpID, int value)
{
    MR::Network* network = this->getNetwork();

    if (!network)
        return;

    MR::AttribDataInt* cpData = (MR::AttribDataInt*)network->getControlParameter(cpID);

    NMP_ASSERT(cpData);

    cpData->m_value = value;

    network->setControlParameter(cpID, cpData);
}

void CharacterMotionCtrlBase::setControlParamUInt(MR::NodeID cpID, uint32_t value)
{
    MR::Network* network = this->getNetwork();

    if (!network)
        return;

    MR::AttribDataUInt* cpData = (MR::AttribDataUInt*)network->getControlParameter(cpID);

	NMP_ASSERT(cpData);

    cpData->m_value = value;

    network->setControlParameter(cpID, cpData);
}

void CharacterMotionCtrlBase::setControlParamBool(MR::NodeID cpID, bool value)
{
    MR::Network* network = this->getNetwork();

    if (!network)
        return;

    MR::AttribDataBool* cpData = (MR::AttribDataBool*)network->getControlParameter(cpID);

    NMP_ASSERT(cpData);

    cpData->m_value = value;

    network->setControlParameter(cpID, cpData);
}

void CharacterMotionCtrlBase::setControlParamVector3(MR::NodeID cpID, const NMP::Vector3& value)
{
    MR::Network* network = this->getNetwork();

    if (!network)
        return;

    MR::AttribDataVector3* cpData = (MR::AttribDataVector3*)network->getControlParameter(cpID);

    NMP_ASSERT(cpData);

    cpData->m_value = value;

    network->setControlParameter(cpID, cpData);
}

void CharacterMotionCtrlBase::setControlParamVector4(MR::NodeID cpID, const NMP::Quat& value)
{
    MR::Network* network = this->getNetwork();

    if (!network)
        return;

    MR::AttribDataVector4* cpData = (MR::AttribDataVector4*)network->getControlParameter(cpID);

    NMP_ASSERT(cpData);

    cpData->m_value = value;

    network->setControlParameter(cpID, cpData);
}

float CharacterMotionCtrlBase::getControlParamFloat(MR::NodeID cpID) const
{
    MR::Network* network = this->getNetwork();

    if (!network)
        return 0.0f;

    MR::AttribDataFloat* cpData = (MR::AttribDataFloat*)network->getControlParameter(cpID);

    NMP_ASSERT(cpData);

    return cpData->m_value;
}

int CharacterMotionCtrlBase::getControlParamInt(MR::NodeID cpID) const
{
    MR::Network* network = this->getNetwork();

    if (!network)
        return 0;

    MR::AttribDataInt* cpData = (MR::AttribDataInt*)network->getControlParameter(cpID);

    NMP_ASSERT(cpData);

    return cpData->m_value;
}

uint32_t CharacterMotionCtrlBase::getControlParamUInt(MR::NodeID cpID) const
{
    MR::Network* network = this->getNetwork();

    if (!network)
        return 0;

    MR::AttribDataUInt* cpData = (MR::AttribDataUInt*)network->getControlParameter(cpID);

    NMP_ASSERT(cpData);

    return cpData->m_value;
}

bool CharacterMotionCtrlBase::getControlParamBool(MR::NodeID cpID) const
{
    MR::Network* network = this->getNetwork();

    if (!network)
        return false;

    MR::AttribDataBool* cpData = (MR::AttribDataBool*)network->getControlParameter(cpID);

    NMP_ASSERT(cpData);

    return cpData->m_value;
}

NMP::Vector3 CharacterMotionCtrlBase::getControlParamVector3(MR::NodeID cpID) const
{
    MR::Network* network = this->getNetwork();

    if (!network)
        return NMP::Vector3Zero();

    MR::AttribDataVector3* cpData = (MR::AttribDataVector3*)network->getControlParameter(cpID);

    NMP_ASSERT(cpData);

    return cpData->m_value;
}

NMP::Quat CharacterMotionCtrlBase::getControlParamVector4(MR::NodeID cpID) const
{
    MR::Network* network = this->getNetwork();

    if (!network)
        return NMP::Quat(0.f, 0.f, 0.f, 0.f);

    MR::AttribDataVector4* cpData = (MR::AttribDataVector4*)network->getControlParameter(cpID);

    NMP_ASSERT(cpData);

    return cpData->m_value;
}

NMP::Quat CharacterMotionCtrlBase::getRotationChange()
{
    MR::Network* network = this->getNetwork();

    if (!network)
        return NMP::QuatIdentity();

    return network->getOrientationChange();
}

NMP::Vector3 CharacterMotionCtrlBase::getTranslationChange()
{
    MR::Network* network = this->getNetwork();

    if (!network)
        return NMP::Vector3Zero();

    return network->getTranslationChange();
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
