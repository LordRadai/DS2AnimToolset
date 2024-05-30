#include "AnimSourceInterface.h"
#include <string>
#include <filesystem>
#include "utils/MorphemeToDirectX.h"

AnimSourceInterface::AnimSourceInterface()
{
}

AnimSourceInterface::AnimSourceInterface(MR::AnimRigDef* rig, MR::RigToAnimMap* rigToAnimMap, const char* filename, int id)
{	
    void* animData = NULL;
    int64_t animSize = 0;
    int64_t fileSize = NMP::NMFile::allocAndLoad(filename, &animData, &animSize);
    MR::AnimationSourceHandle* animHandle = new MR::AnimationSourceHandle;

    std::filesystem::path name = std::filesystem::path(filename).filename();

    bool found = false;

    if (fileSize > -1)
    {
        animHandle->openAnimation((unsigned char*)animData, fileSize, "nsa");
        found = true;
    }

    while (true)
    {
        if (found)
            break;

        std::filesystem::path parent_path = std::filesystem::path(filename).parent_path().parent_path().string() + "\\c0001\\";

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

        break;
    }

    if (!found)
    {
        delete animHandle;
        animHandle = nullptr;
    }

    if (animHandle != nullptr)
    {
        animHandle->setRigToAnimMap(rigToAnimMap);
        animHandle->setRig(rig);
        animHandle->setTime(0.f);
    }

	this->m_animHandle = animHandle;
	this->m_id = id;

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

    if (channelId == animHandle->getRig()->getTrajectoryBoneIndex())
    {
        NMP::Vector3 trajPos;
        NMP::Quat trajRot;
        animHandle->getTrajectory(trajRot, trajPos);

        transform = NMDX::GetWorldMatrix(trajRot, trajPos);

        //transform *= Matrix::CreateRotationX(-DirectX::XM_PIDIV2) * Matrix::CreateRotationY(DirectX::XM_PI);
    }

    //if (channelId == animHandle->getRig()->getCharacterRootBoneIndex())
        //transform *= Matrix::CreateRotationX(-DirectX::XM_PIDIV2) * Matrix::CreateRotationY(DirectX::XM_PI);

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