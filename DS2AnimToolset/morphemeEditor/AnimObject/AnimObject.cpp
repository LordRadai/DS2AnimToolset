#include "AnimObject.h"
#include <string>
#include <filesystem>
#include "utils/NMDX/NMDX.h"
#include "RCore.h"
#include "MorphemeSystem/MorphemeCharacterDef/MorphemeCharacterDef.h"
#include "MorphemeSystem/MorphemeExport/MorphemeExport.h"

AnimObject::AnimObject()
{
}

AnimObject::AnimObject(int id)
{	
    this->m_id = id;
}

AnimObject::~AnimObject()
{
}

AnimObject* AnimObject::createFromMorphemeAssets(MorphemeCharacterDef* owner, MR::AnimRigDef* rig, MR::RigToAnimMap* rigToAnimMap, const char* filename, int id)
{
    AnimObject* animObj = new AnimObject(id);

    void* animData = nullptr;
    int64_t animSize = 0;
    int64_t fileSize = NMP::NMFile::allocAndLoad(filename, &animData, &animSize);
    MR::AnimationSourceHandle* animHandle = new MR::AnimationSourceHandle;

    std::filesystem::path name = std::filesystem::path(filename).filename();

    bool found = false;

    //Look for the NSA anim file in the same folder as the NMB file first
    if (fileSize > -1)
    {
        animHandle->openAnimation((unsigned char*)animData, fileSize, "nsa");
        found = true;
    }

    //If it's not found, look in the extended anim folder instead
    if (!found)
    {
        std::filesystem::path parent_path = std::filesystem::path(filename).parent_path().parent_path().string() + "\\c0001\\";

        if (std::filesystem::exists(parent_path))
        {
            for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(parent_path))
            {
                std::string filepath = dirEntry.path().string() + "\\" + name.string();

                fileSize = NMP::NMFile::allocAndLoad(filepath.c_str(), &animData, &animSize);

                if (fileSize > -1)
                {
                    animHandle->openAnimation((unsigned char*)animData, fileSize, "nsa");
                    found = true;
                    break;
                }
            }
        }
    }

    if (!found)
    {
        delete animHandle;
        animHandle = nullptr;
    }

    if (animHandle)
    {
        animHandle->setRigToAnimMap(rigToAnimMap);
        animHandle->setRig(rig);
        animHandle->setTime(0.f);
    }

    //Create markup from EventTracks for the current animation
    wchar_t out_path[255];
    swprintf_s(out_path, L"Export\\c%04d\\morphemeMarkup\\", owner->getCharacterId());

    std::filesystem::path takeListPath = std::filesystem::path(out_path).string() + RString::removeExtension(owner->getAnimFileLookUp()->getSourceFilename(id)) + ".xmd.xml";

    animObj->m_animHandle = animHandle;
    animObj->m_takeList = MorphemeExport::exportAnimMarkup(owner, id, takeListPath.c_str());
    animObj->m_id = id;

    animObj->m_animFileName = std::string(filename);
    animObj->m_animName = std::string(name.string());

    return animObj;
}

Matrix AnimObject::getTransformAtTime(float time, int channelId)
{
    MR::AnimationSourceHandle* animHandle = this->m_animHandle;
    animHandle->setTime(time);

    Matrix transform = NMDX::getWorldMatrix(animHandle->getChannelData()[channelId].m_quat, animHandle->getChannelData()[channelId].m_pos);

    if ((channelId == animHandle->getRig()->getTrajectoryBoneIndex()) || (channelId == animHandle->getRig()->getCharacterRootBoneIndex()))
    {
        NMP::Vector3 trajPos;
        NMP::Quat trajRot;
        animHandle->getTrajectory(trajRot, trajPos);

        transform *= NMDX::getWorldMatrix(trajRot, trajPos);
    }

    return transform;
}

Vector3 AnimObject::getTransformPosAtTime(float time, int channelId)
{
    MR::AnimationSourceHandle* animHandle = this->m_animHandle;
    animHandle->setTime(time);

    if (channelId == animHandle->getRig()->getTrajectoryBoneIndex())
    {
        NMP::Vector3 trajPos;
        NMP::Quat trajRot;
        animHandle->getTrajectory(trajRot, trajPos);

        return NMDX::getDxVector(trajPos);
    }

    return NMDX::getDxVector(animHandle->getChannelData()[channelId].m_pos);
}

Quaternion AnimObject::getTransformQuatAtTime(float time, int channelId)
{
    MR::AnimationSourceHandle* animHandle = this->m_animHandle;
    animHandle->setTime(time);

    if (channelId == animHandle->getRig()->getTrajectoryBoneIndex())
    {
        NMP::Vector3 trajPos;
        NMP::Quat trajRot;
        animHandle->getTrajectory(trajRot, trajPos);

        return NMDX::getDxQuat(trajRot);
    }

    return NMDX::getDxQuat(animHandle->getChannelData()[channelId].m_quat);
}

void AnimObject::setAnimTime(float time)
{
    if (this->m_animHandle)
        this->m_animHandle->setTime(time);
}