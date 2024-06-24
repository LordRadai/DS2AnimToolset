#include "AnimSourceInterface.h"
#include <string>
#include <filesystem>
#include "utils/NMDX/NMDX.h"
#include "utils/RString/RString.h"
#include "MorphemeSystem/CharacterDef/CharacterDef.h"
#include "MorphemeSystem/MorphemeExport/MorphemeExport.h"

AnimSourceInterface::AnimSourceInterface()
{
}

AnimSourceInterface::AnimSourceInterface(CharacterDef* owner, MR::AnimRigDef* rig, MR::RigToAnimMap* rigToAnimMap, const char* filename, int id)
{	
    void* animData = NULL;
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

    std::filesystem::path takeListPath = std::filesystem::path(out_path).string() + RString::RemoveExtension(owner->getAnimFileLookUp()->getSourceFilename(id)) + ".fbx.xml";

	this->m_animHandle = animHandle;
    this->m_takeList = MorphemeExport::ExportAnimXML(owner, id, takeListPath.c_str());
	this->m_id = id;

    strcpy(this->m_animFileName, filename);
    strcpy(this->m_animName, name.string().c_str());
}

AnimSourceInterface::~AnimSourceInterface()
{
}

bool AnimSourceInterface::lessThan(AnimSourceInterface* a, AnimSourceInterface* b)
{
    return std::string(a->m_animName) < std::string(b->m_animName);
}

Matrix AnimSourceInterface::GetTransformAtTime(float time, int channelId)
{
    MR::AnimationSourceHandle* animHandle = this->m_animHandle;
    animHandle->setTime(time);

    Matrix transform = NMDX::GetWorldMatrix(animHandle->getChannelData()[channelId].m_quat, animHandle->getChannelData()[channelId].m_pos);

    if ((channelId == animHandle->getRig()->getTrajectoryBoneIndex()) || (channelId == animHandle->getRig()->getCharacterRootBoneIndex()))
    {
        NMP::Vector3 trajPos;
        NMP::Quat trajRot;
        animHandle->getTrajectory(trajRot, trajPos);

        transform *= NMDX::GetWorldMatrix(trajRot, trajPos);
    }

    return transform;
}

Vector3 AnimSourceInterface::GetTransformPosAtTime(float time, int channelId)
{
    MR::AnimationSourceHandle* animHandle = this->m_animHandle;
    animHandle->setTime(time);

    if (channelId == animHandle->getRig()->getTrajectoryBoneIndex())
    {
        NMP::Vector3 trajPos;
        NMP::Quat trajRot;
        animHandle->getTrajectory(trajRot, trajPos);

        return NMDX::CreateFrom(trajPos);
    }

    return NMDX::CreateFrom(animHandle->getChannelData()[channelId].m_pos);
}

Quaternion AnimSourceInterface::GetTransformQuatAtTime(float time, int channelId)
{
    MR::AnimationSourceHandle* animHandle = this->m_animHandle;
    animHandle->setTime(time);

    if (channelId == animHandle->getRig()->getTrajectoryBoneIndex())
    {
        NMP::Vector3 trajPos;
        NMP::Quat trajRot;
        animHandle->getTrajectory(trajRot, trajPos);

        return NMDX::CreateFrom(trajRot);
    }

    return NMDX::CreateFrom(animHandle->getChannelData()[channelId].m_quat);
}

void AnimSourceInterface::SetAnimTime(float time)
{
    if (this->m_animHandle)
        this->m_animHandle->setTime(time);
}