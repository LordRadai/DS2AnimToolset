#pragma once
#include <fbxsdk.h>
#include "FlverModel/FlverModel.h"
#include "MorphemeSystem/MorphemeSystem.h"
#include "AnimObject/AnimObject.h"

namespace FBXTranslator
{
	FbxNode* createModelFbxMesh(FbxScene* pScene, FlverModel* pModel, std::vector<FbxNode*> skeletonNodes, int idx);
	std::vector<FbxNode*> createFbxFlverSkeleton(FbxScene* pScene, FlverModel* pModel, FbxPose* pBindPoses);
	std::vector<FbxNode*> createFbxMorphemeSkeleton(FbxScene* pScene, MR::AnimRigDef* pRig, FbxPose* pBindPoses);
	bool createFbxTake(FbxScene* pScene, std::vector<FbxNode*> pSkeleton, AnimObject* pAnim, std::string name, int fps);
	bool createFbxModel(FbxScene* pScene, FlverModel* pModel, std::string modelName, FbxPose* pBindPoses, std::vector<FbxNode*> pBoneList, std::filesystem::path export_path);
	FbxNode* createLightNode(FbxScene* pScene, FbxVector4 position, const char* name);
}