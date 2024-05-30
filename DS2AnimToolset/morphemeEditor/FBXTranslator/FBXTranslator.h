#pragma once
#include "FlverModel/FlverModel.h"
#include "MorphemeSystem/MorphemeSystem.h"
#include "AnimSourceInterface/AnimSourceInterface.h"

namespace FBXTranslator
{
	FbxNode* CreateModelFbxMesh(FbxScene* pScene, FlverModel* pFlverModel, std::vector<FbxNode*> skeletonNodes, int idx);
	FbxNode* CreateModelFbxMesh(FbxScene* pScene, FlverModel* pFlverModel, std::vector<FbxNode*> skeletonNodes, int idx, std::vector<int> morphemeToFlverBoneMap);

	std::vector<FbxNode*> CreateFbxFlverSkeleton(FbxScene* pScene, FlverModel* pFlverModel, FbxPose* pBindPoses);
	std::vector<FbxNode*> CreateFbxMorphemeSkeleton(FbxScene* pScene, MR::AnimRigDef* pRig, FbxPose* pBindPoses);

	bool CreateFbxTake(FbxScene* pScene, std::vector<FbxNode*> pSkeleton, AnimSourceInterface* pAnim, std::string name, std::vector<int> morphemeToFlverBoneMap);
	bool CreateFbxTake(FbxScene* pScene, std::vector<FbxNode*> pSkeleton, AnimSourceInterface* pAnim, std::string name);

	bool CreateFbxModel(FbxScene* pScene, FlverModel* pFlverModel, int chrId, FbxPose* pBindPoses, std::vector<FbxNode*> pBoneList, std::filesystem::path export_path);
	bool CreateFbxModel(FbxScene* pScene, FlverModel* pFlverModel, int chrId, FbxPose* pBindPoses, std::vector<FbxNode*> pBoneList, std::filesystem::path export_path, std::vector<int> morphemeToFlverBoneMap);
}