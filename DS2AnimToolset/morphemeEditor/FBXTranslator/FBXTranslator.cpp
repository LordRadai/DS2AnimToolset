#include "FBXTranslator.h"
#include "RString/RString.h"
#include "utils/MorphemeToDirectX.h"
#include "morpheme/AnimSource/mrAnimSourceNSA.h"
#include "RLog/RLog.h"
#include "extern.h"

//Converts a DirectX::XMMATRIX to an FbxAMatrix
FbxAMatrix CreateFbxMatrixFromDXMatrix(DirectX::XMMATRIX m)
{
	FbxAMatrix fbxMatrix;

	fbxMatrix.mData[0][0] = m.r[0].m128_f32[0]; fbxMatrix.mData[0][1] = m.r[0].m128_f32[1]; fbxMatrix.mData[0][2] = m.r[0].m128_f32[2]; fbxMatrix.mData[0][3] = m.r[0].m128_f32[3];
	fbxMatrix.mData[1][0] = m.r[1].m128_f32[0]; fbxMatrix.mData[1][1] = m.r[1].m128_f32[1]; fbxMatrix.mData[1][2] = m.r[1].m128_f32[2]; fbxMatrix.mData[1][3] = m.r[1].m128_f32[3];
	fbxMatrix.mData[2][0] = m.r[2].m128_f32[0]; fbxMatrix.mData[2][1] = m.r[2].m128_f32[1]; fbxMatrix.mData[2][2] = m.r[2].m128_f32[2]; fbxMatrix.mData[2][3] = m.r[2].m128_f32[3];
	fbxMatrix.mData[3][0] = m.r[3].m128_f32[0]; fbxMatrix.mData[3][1] = m.r[3].m128_f32[1]; fbxMatrix.mData[3][2] = m.r[3].m128_f32[2]; fbxMatrix.mData[3][3] = m.r[3].m128_f32[3];

	return fbxMatrix;
}

//Converts an FbxMatrix into an FbxAMatrix
FbxAMatrix ConvertFbxMatrixToAMatrix(FbxMatrix m)
{
	FbxVector4 translation, rotation, scale, shearing;
	double sign;

	m.GetElements(translation, rotation, shearing, scale, sign);

	FbxAMatrix fbxAMatrix;

	fbxAMatrix.SetTRS(translation, rotation, scale);

	return fbxAMatrix;
}

//Gets FLVER bone local transform
FbxAMatrix GetBoneLocalTransform(cfr::FLVER2::Bone bone)
{
	DirectX::XMMATRIX world = DirectX::XMMatrixScaling(bone.scale.x, bone.scale.y, bone.scale.z);
	world *= DirectX::XMMatrixRotationX(bone.rot.x);
	world *= DirectX::XMMatrixRotationZ(bone.rot.z);
	world *= DirectX::XMMatrixRotationY(bone.rot.y);
	world *= DirectX::XMMatrixTranslation(bone.translation.x, bone.translation.y, bone.translation.z);

	return CreateFbxMatrixFromDXMatrix(world);
}

FbxNode* CreateFlverBoneNode(FbxScene* pScene, FbxPose* pBindPoses, cfr::FLVER2::Bone bone, int id)
{
	FbxNode* pBoneNode = FbxNode::Create(pScene, RString::ToNarrow(bone.name).c_str());

	FbxAMatrix boneTransform = GetBoneLocalTransform(bone);

	pBoneNode->LclTranslation.Set(boneTransform.GetT());
	pBoneNode->LclRotation.Set(boneTransform.GetR());
	pBoneNode->LclScaling.Set(boneTransform.GetS());

	FbxSkeleton* pBone = FbxSkeleton::Create(pScene, RString::ToNarrow(bone.name).c_str());
	pBone->SetSkeletonType(FbxSkeleton::eLimbNode);
	pBone->Color.Set(FbxDouble3(0, 0.769, 0.769));
	pBone->LimbLength = 0.1;

	pBoneNode->SetNodeAttribute(pBone);
	pBindPoses->Add(pBoneNode, boneTransform);

	return pBoneNode;
}

FbxNode* CreateMorphemeBoneNode(FbxScene* pScene, FbxPose* pBindPoses, MR::AnimRigDef* pRig, int id)
{
	FbxNode* pBoneNode = FbxNode::Create(pScene, pRig->getBoneName(id));

	Matrix bonePose = NMDX::GetWorldMatrix(*pRig->getBindPoseBoneQuat(id), *pRig->getBindPoseBonePos(id));
	FbxAMatrix boneTransform = CreateFbxMatrixFromDXMatrix(bonePose);

	pBoneNode->LclTranslation.Set(boneTransform.GetT());
	pBoneNode->LclRotation.Set(boneTransform.GetR());
	pBoneNode->LclScaling.Set(FbxDouble3(1.0, 1.0, 1.0));

	FbxSkeleton* pBone = FbxSkeleton::Create(pScene, pRig->getBoneName(id));
	pBone->SetSkeletonType(FbxSkeleton::eLimbNode);
	pBone->Color.Set(FbxDouble3(1.0, 0.66, 0.0));
	pBone->LimbLength = 0.1;

	pBoneNode->SetNodeAttribute(pBone);
	pBindPoses->Add(pBoneNode, boneTransform);

	return pBoneNode;
}

//Creates an FbxNode* object called Skeleton and rotates it by 180 degrees
FbxNode* CreateDummyRootBoneNode(FbxScene* pScene)
{
	FbxNode* pBoneNode = FbxNode::Create(pScene, "Skeleton");

	pBoneNode->LclTranslation.Set(FbxVector4(0.0, 0.0, 0.0));
	pBoneNode->LclRotation.Set(FbxVector4(0.0, 180.0, 0.0));
	pBoneNode->LclScaling.Set(FbxVector4(1.0, 1.0, 1.0));

	FbxSkeleton* pBone = FbxSkeleton::Create(pScene, "Skeleton");
	pBone->SetSkeletonType(FbxSkeleton::eRoot);
	pBone->Color.Set(FbxDouble3(0, 0.769, 0.769));
	pBone->LimbLength = 0.1;

	pBoneNode->SetNodeAttribute(pBone);

	return pBoneNode;
}

inline double GetTimeByAnimFrame(float animLenght, float fps, int frameNum)
{
	return (animLenght / fps) * frameNum;
}

FbxAMatrix ConvertToFbxAMatrix(const DirectX::SimpleMath::Matrix& dxMatrix)
{
	FbxAMatrix fbxMatrix;

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			fbxMatrix[i][j] = dxMatrix.m[i][j];
		}
	}

	return fbxMatrix;
}

//Creates an FbxNode object containing vertices, normals and bone weights
FbxNode* FBXTranslator::CreateModelFbxMesh(FbxScene* pScene, FlverModel* pFlverModel, std::vector<FbxNode*> skeletonNodes, int idx)
{
	if (pFlverModel->m_flver == nullptr)
		return nullptr;

	if (idx > pFlverModel->m_flver->header.meshCount)
		return nullptr;

	std::string mesh_node_name = "mesh[" + std::to_string(idx) + "]";
	FbxNode* pMeshNode = FbxNode::Create(pScene, mesh_node_name.c_str());

	FbxMesh* pMesh = FbxMesh::Create(pScene, std::string(mesh_node_name + "_mesh").c_str());
	pMesh->CreateLayer();

	//Add vertices
	std::vector<FbxVector4> vertices = pFlverModel->GetModelMeshVertices(idx, true);

	pMesh->InitControlPoints(vertices.size());

	for (int i = 0; i < vertices.size(); i++)
		pMesh->SetControlPointAt(vertices[i], i);

	//Add normals
	std::vector<FbxVector4> normals = pFlverModel->GetModelMeshNormals(idx, true);

	FbxLayerElementNormal* pLayerNormal = (FbxLayerElementNormal*)pMesh->GetLayer(0)->CreateLayerElementOfType(FbxLayerElement::eNormal);
	pLayerNormal->SetMappingMode(FbxLayerElement::eByControlPoint);
	pLayerNormal->SetReferenceMode(FbxLayerElement::eDirect);

	for (int i = 0; i < normals.size(); i++)
		pLayerNormal->GetDirectArray().Add(normals[i]);

	//Create polygons
	for (int i = 0; i < vertices.size(); i += 3)
	{
		pMesh->BeginPolygon();

		pMesh->AddPolygon(i);
		pMesh->AddPolygon(i + 1);
		pMesh->AddPolygon(i + 2);

		pMesh->EndPolygon();
	}

	FbxSkin* pSkin = nullptr;

	//Add bone weights
	std::vector<FbxVector4> bone_weights = pFlverModel->GetModelMeshBoneWeights(idx);
	std::vector<int*> bone_indices;

	bone_indices.reserve(bone_weights.size());
	pFlverModel->GetModelMeshBoneIndices(bone_indices, idx);

	if (bone_weights.size() > 0)
		pSkin = FbxSkin::Create(pScene, std::string(mesh_node_name + "_skin").c_str());

	if (pSkin != nullptr)
	{
		for (int i = 0; i < pFlverModel->m_flver->meshes[idx].header.boneCount; i++)
		{
			int boneIndex = pFlverModel->m_flver->meshes[idx].boneIndices[i];

			if (boneIndex == -1)
				continue;

			FbxNode* pBoneNode = skeletonNodes[boneIndex];

			FbxCluster* pCluster = FbxCluster::Create(pScene, (mesh_node_name + "_" + RString::ToNarrow(pFlverModel->m_flver->bones[boneIndex].name) + "_cluster").c_str());
			pCluster->SetLink(pBoneNode);
			pCluster->SetLinkMode(FbxCluster::eTotalOne);

			pCluster->SetTransformLinkMatrix(skeletonNodes[boneIndex]->EvaluateGlobalTransform());

			for (int vertexIndex = 0; vertexIndex < vertices.size(); vertexIndex++)
			{
				int* indices = bone_indices[vertexIndex];
				FbxVector4 weights = bone_weights[vertexIndex];

				for (int wt = 0; wt < 4; wt++)
				{
					switch (wt)
					{
					case 0:
						if (indices[0] == boneIndex)
							pCluster->AddControlPointIndex(vertexIndex, weights[0]);
						break;
					case 1:
						if (indices[1] == boneIndex)
							pCluster->AddControlPointIndex(vertexIndex, weights[1]);
						break;
					case 2:
						if (indices[2] == boneIndex)
							pCluster->AddControlPointIndex(vertexIndex, weights[2]);
						break;
					case 3:
						if (indices[3] == boneIndex)
							pCluster->AddControlPointIndex(vertexIndex, weights[3]);
						break;
					}
				}
			}

			pSkin->AddCluster(pCluster);
		}
	}

	pMesh->AddDeformer(pSkin);
	pMesh->BuildMeshEdgeArray();
	pMeshNode->SetNodeAttribute(pMesh);

	return pMeshNode;
}

FbxNode* FBXTranslator::CreateModelFbxMesh(FbxScene* pScene, FlverModel* pFlverModel, std::vector<FbxNode*> skeletonNodes, int idx, std::vector<int> morphemeToFlverBoneMap)
{
	if (pFlverModel->m_flver == nullptr)
		return nullptr;

	if (idx > pFlverModel->m_flver->header.meshCount)
		return nullptr;

	std::string mesh_node_name = "mesh[" + std::to_string(idx) + "]";
	FbxNode* pMeshNode = FbxNode::Create(pScene, mesh_node_name.c_str());

	FbxMesh* pMesh = FbxMesh::Create(pScene, std::string(mesh_node_name + "_mesh").c_str());
	pMesh->CreateLayer();

	//Add vertices
	std::vector<FbxVector4> vertices = pFlverModel->GetModelMeshVertices(idx, true);

	pMesh->InitControlPoints(vertices.size());

	for (int i = 0; i < vertices.size(); i++)
		pMesh->SetControlPointAt(vertices[i], i);

	//Add normals
	std::vector<FbxVector4> normals = pFlverModel->GetModelMeshNormals(idx, true);

	FbxLayerElementNormal* pLayerNormal = (FbxLayerElementNormal*)pMesh->GetLayer(0)->CreateLayerElementOfType(FbxLayerElement::eNormal);
	pLayerNormal->SetMappingMode(FbxLayerElement::eByControlPoint);
	pLayerNormal->SetReferenceMode(FbxLayerElement::eDirect);

	for (int i = 0; i < normals.size(); i++)
		pLayerNormal->GetDirectArray().Add(normals[i]);

	//Create polygons
	for (int i = 0; i < vertices.size(); i += 3)
	{
		pMesh->BeginPolygon();

		pMesh->AddPolygon(i);
		pMesh->AddPolygon(i + 1);
		pMesh->AddPolygon(i + 2);

		pMesh->EndPolygon();
	}

	pMesh->BuildMeshEdgeArray();
	pMeshNode->SetNodeAttribute(pMesh);

	return pMeshNode;
}

//The skeleton has a fake root node. Take this into account when exporting animations
std::vector<FbxNode*> FBXTranslator::CreateFbxFlverSkeleton(FbxScene* pScene, FlverModel* pFlverModel, FbxPose* pBindPoses)
{
	std::vector<FbxNode*> boneNodes;
	boneNodes.reserve(pFlverModel->m_flver->header.boneCount);

	for (int i = 0; i < pFlverModel->m_flver->header.boneCount; i++)
	{
		cfr::FLVER2::Bone bone = pFlverModel->m_flver->bones[i];

		boneNodes.push_back(CreateFlverBoneNode(pScene, pBindPoses, bone, i));
	}

	for (int i = 0; i < pFlverModel->m_flver->header.boneCount; i++)
	{
		cfr::FLVER2::Bone bone = pFlverModel->m_flver->bones[i];

		if (bone.parentIndex != -1)
			boneNodes[bone.parentIndex]->AddChild(boneNodes[i]);
		else
			pScene->GetRootNode()->AddChild(boneNodes[i]);
	}

	return boneNodes;
}

std::vector<FbxNode*> FBXTranslator::CreateFbxMorphemeSkeleton(FbxScene* pScene, MR::AnimRigDef* pRig, FbxPose* pBindPoses)
{
	std::vector<FbxNode*> pMorphemeBoneList;
	pMorphemeBoneList.reserve(pRig->getNumBones());

	for (int i = 0; i < pRig->getNumBones(); i++)
	{
		FbxNode* pMorphemeBoneNode = CreateMorphemeBoneNode(pScene, pBindPoses, pRig, i);

		pMorphemeBoneList.push_back(pMorphemeBoneNode);
	}

	for (int i = 0; i < pRig->getNumBones(); i++)
	{
		int boneParentIdx = pRig->getParentBoneIndex(i);

		if (boneParentIdx != -1)
			pMorphemeBoneList[boneParentIdx]->AddChild(pMorphemeBoneList[i]);
		else
			pScene->GetRootNode()->AddChild(pMorphemeBoneList[i]);
	}

	return pMorphemeBoneList;
}

//Creates FBX animation take from an NSA file input
bool FBXTranslator::CreateFbxTake(FbxScene* pScene, std::vector<FbxNode*> pSkeleton, AnimSourceInterface* pAnim, std::string name, std::vector<int> flverToMorphemeBoneMap)
{
	MR::AnimationSourceHandle* animHandle = pAnim->GetHandle();

	if (animHandle == nullptr)
		return false;

	MR::AnimSourceNSA* animSourceNSA = (MR::AnimSourceNSA*)animHandle->getAnimation();

	if (animSourceNSA->getType() != ANIM_TYPE_NSA)
	{
		RDebug::SystemAlert(g_logLevel, MsgLevel_Error, "Application.cpp", "Unsupported animation format");
		return false;
	}

	const char* cStrName = name.c_str();

	FbxAnimStack* pAnimStack = FbxAnimStack::Create(pScene, cStrName);
	FbxAnimLayer* pAnimBaseLayer = FbxAnimLayer::Create(pScene, cStrName);

	pAnimStack->AddMember(pAnimBaseLayer);

	FbxTime start;
	start.SetSecondDouble(0.0);

	float animSampleRate = 60;
	float animDuration = animSourceNSA->getDuration(animSourceNSA);

	FbxTime end;
	end.SetSecondDouble(animDuration);

	FbxTimeSpan timeSpan = FbxTimeSpan(start, end);
	pAnimStack->SetLocalTimeSpan(timeSpan);

	int keyframeCount = animSampleRate * animDuration;
	int boneCount = animHandle->getChannelCount();

	for (int boneIndex = 0; boneIndex < boneCount; boneIndex++)
	{
		int flverBoneIdx = flverToMorphemeBoneMap[boneIndex];

		if (flverBoneIdx == -1)
			continue;

		FbxNode* pBone = pSkeleton[flverBoneIdx];

		FbxAnimCurve* curveTX = pBone->LclTranslation.GetCurve(pAnimBaseLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
		FbxAnimCurve* curveTY = pBone->LclTranslation.GetCurve(pAnimBaseLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
		FbxAnimCurve* curveTZ = pBone->LclTranslation.GetCurve(pAnimBaseLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);

		FbxAnimCurve* curveRX = pBone->LclRotation.GetCurve(pAnimBaseLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
		FbxAnimCurve* curveRY = pBone->LclRotation.GetCurve(pAnimBaseLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
		FbxAnimCurve* curveRZ = pBone->LclRotation.GetCurve(pAnimBaseLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);

		curveTX->KeyModifyBegin();
		curveTY->KeyModifyBegin();
		curveTZ->KeyModifyBegin();

		curveRX->KeyModifyBegin();
		curveRY->KeyModifyBegin();
		curveRZ->KeyModifyBegin();

		for (int frame = 0; frame < keyframeCount; frame++)
		{
			int keyIndex;

			float animTime = GetTimeByAnimFrame(animDuration, animSampleRate, frame);
			FbxAMatrix transform = ConvertToFbxAMatrix(pAnim->GetTransformAtTime(animTime, boneIndex));

			FbxTime keyTime;
			keyTime.SetSecondDouble(animTime);

			FbxVector4 translation = transform.GetT();

			keyIndex = curveTX->KeyAdd(keyTime);
			curveTX->KeySetValue(keyIndex, translation[0]);
			curveTX->KeySetInterpolation(keyIndex, FbxAnimCurveDef::eInterpolationLinear);

			keyIndex = curveTY->KeyAdd(keyTime);
			curveTY->KeySetValue(keyIndex, translation[1]);
			curveTY->KeySetInterpolation(keyIndex, FbxAnimCurveDef::eInterpolationLinear);

			keyIndex = curveTZ->KeyAdd(keyTime);
			curveTZ->KeySetValue(keyIndex, translation[2]);
			curveTZ->KeySetInterpolation(keyIndex, FbxAnimCurveDef::eInterpolationLinear);

			FbxVector4 rotation = transform.GetR();

			keyIndex = curveRX->KeyAdd(keyTime);
			curveRX->KeySetValue(keyIndex, rotation[0]);
			curveRX->KeySetInterpolation(keyIndex, FbxAnimCurveDef::eInterpolationLinear);

			keyIndex = curveRY->KeyAdd(keyTime);
			curveRY->KeySetValue(keyIndex, rotation[1]);
			curveRY->KeySetInterpolation(keyIndex, FbxAnimCurveDef::eInterpolationLinear);

			keyIndex = curveRZ->KeyAdd(keyTime);
			curveRZ->KeySetValue(keyIndex, rotation[2]);
			curveRZ->KeySetInterpolation(keyIndex, FbxAnimCurveDef::eInterpolationLinear);
		}

		curveTX->KeyModifyEnd();
		curveTY->KeyModifyEnd();
		curveTZ->KeyModifyEnd();

		curveRX->KeyModifyEnd();
		curveRY->KeyModifyEnd();
		curveRZ->KeyModifyEnd();
	}

	return true;
}

bool FBXTranslator::CreateFbxTake(FbxScene* pScene, std::vector<FbxNode*> pSkeleton, AnimSourceInterface* pAnim, std::string name)
{
	MR::AnimationSourceHandle* animHandle = pAnim->GetHandle();

	if (animHandle == nullptr)
		return false;

	MR::AnimSourceNSA* animSourceNSA = (MR::AnimSourceNSA*)animHandle->getAnimation();

	if (animSourceNSA->getType() != ANIM_TYPE_NSA)
	{
		RDebug::SystemAlert(g_logLevel, MsgLevel_Error, "Application.cpp", "Unsupported animation format");
		return false;
	}

	const char* cStrName = name.c_str();

	FbxAnimStack* pAnimStack = FbxAnimStack::Create(pScene, cStrName);
	FbxAnimLayer* pAnimBaseLayer = FbxAnimLayer::Create(pScene, cStrName);

	pAnimStack->AddMember(pAnimBaseLayer);

	FbxTime start;
	start.SetSecondDouble(0.0);

	float animSampleRate = 60;
	float animDuration = animSourceNSA->getDuration(animSourceNSA);

	FbxTime end;
	end.SetSecondDouble(animDuration);

	FbxTimeSpan timeSpan = FbxTimeSpan(start, end);
	pAnimStack->SetLocalTimeSpan(timeSpan);

	int keyframeCount = animSampleRate * animDuration;
	int boneCount = animHandle->getChannelCount();

	for (int boneIndex = 0; boneIndex < boneCount; boneIndex++)
	{
		FbxNode* pBone = pSkeleton[boneIndex];

		FbxAnimCurve* curveTX = pBone->LclTranslation.GetCurve(pAnimBaseLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
		FbxAnimCurve* curveTY = pBone->LclTranslation.GetCurve(pAnimBaseLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
		FbxAnimCurve* curveTZ = pBone->LclTranslation.GetCurve(pAnimBaseLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);

		FbxAnimCurve* curveRX = pBone->LclRotation.GetCurve(pAnimBaseLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
		FbxAnimCurve* curveRY = pBone->LclRotation.GetCurve(pAnimBaseLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
		FbxAnimCurve* curveRZ = pBone->LclRotation.GetCurve(pAnimBaseLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);

		curveTX->KeyModifyBegin();
		curveTY->KeyModifyBegin();
		curveTZ->KeyModifyBegin();

		curveRX->KeyModifyBegin();
		curveRY->KeyModifyBegin();
		curveRZ->KeyModifyBegin();

		for (int frame = 0; frame < keyframeCount; frame++)
		{
			int keyIndex;

			float animTime = GetTimeByAnimFrame(animDuration, animSampleRate, frame);
			FbxAMatrix transform = ConvertToFbxAMatrix(pAnim->GetTransformAtTime(animTime, boneIndex));

			FbxTime keyTime;
			keyTime.SetSecondDouble(animTime);

			FbxVector4 translation = transform.GetT();

			keyIndex = curveTX->KeyAdd(keyTime);
			curveTX->KeySetValue(keyIndex, translation[0]);
			curveTX->KeySetInterpolation(keyIndex, FbxAnimCurveDef::eInterpolationLinear);

			keyIndex = curveTY->KeyAdd(keyTime);
			curveTY->KeySetValue(keyIndex, translation[1]);
			curveTY->KeySetInterpolation(keyIndex, FbxAnimCurveDef::eInterpolationLinear);

			keyIndex = curveTZ->KeyAdd(keyTime);
			curveTZ->KeySetValue(keyIndex, translation[2]);
			curveTZ->KeySetInterpolation(keyIndex, FbxAnimCurveDef::eInterpolationLinear);

			FbxVector4 rotation = transform.GetR();

			keyIndex = curveRX->KeyAdd(keyTime);
			curveRX->KeySetValue(keyIndex, rotation[0]);
			curveRX->KeySetInterpolation(keyIndex, FbxAnimCurveDef::eInterpolationLinear);

			keyIndex = curveRY->KeyAdd(keyTime);
			curveRY->KeySetValue(keyIndex, rotation[1]);
			curveRY->KeySetInterpolation(keyIndex, FbxAnimCurveDef::eInterpolationLinear);

			keyIndex = curveRZ->KeyAdd(keyTime);
			curveRZ->KeySetValue(keyIndex, rotation[2]);
			curveRZ->KeySetInterpolation(keyIndex, FbxAnimCurveDef::eInterpolationLinear);
		}

		curveTX->KeyModifyEnd();
		curveTY->KeyModifyEnd();
		curveTZ->KeyModifyEnd();

		curveRX->KeyModifyEnd();
		curveRY->KeyModifyEnd();
		curveRZ->KeyModifyEnd();
	}

	return true;
}

//Adds flver meshes to the scene
bool FBXTranslator::CreateFbxModel(FbxScene* pScene, FlverModel* pFlverModel, int chrId, FbxPose* pBindPoses, std::vector<FbxNode*> pBoneList, std::filesystem::path export_path)
{
	std::string model_node_name = "c" + std::to_string(chrId) + "_model";

	FbxNode* pModelNode = FbxNode::Create(pScene, model_node_name.c_str());
	pScene->GetRootNode()->AddChild(pModelNode);

	std::vector<FbxNode*> pMeshNodesList;

	pMeshNodesList.reserve(pFlverModel->m_flver->header.meshCount);

	for (int i = 0; i < pFlverModel->m_flver->header.meshCount; i++)
	{
		FbxNode* pMeshNode = nullptr;

		pMeshNode = FBXTranslator::CreateModelFbxMesh(pScene, pFlverModel, pBoneList, i);

		if (pMeshNode != nullptr)
		{
			pModelNode->AddChild(pMeshNode);
			pBindPoses->Add(pMeshNode, FbxAMatrix());

			pMeshNodesList.push_back(pMeshNode);
		}
		else
			RDebug::DebuggerOut(g_logLevel, MsgLevel_Error, "Failed to create MeshNode object for mesh %d (file=%s)\n", i, export_path.filename().c_str());
	}

	return true;
}

bool FBXTranslator::CreateFbxModel(FbxScene* pScene, FlverModel* pFlverModel, int chrId, FbxPose* pBindPoses, std::vector<FbxNode*> pBoneList, std::filesystem::path export_path, std::vector<int> morphemeToFlverBoneMap)
{
	std::string model_node_name = "c" + std::to_string(chrId) + "_model";

	FbxNode* pModelNode = FbxNode::Create(pScene, model_node_name.c_str());
	pScene->GetRootNode()->AddChild(pModelNode);

	std::vector<FbxNode*> pMeshNodesList;

	pMeshNodesList.reserve(pFlverModel->m_flver->header.meshCount);

	for (int i = 0; i < pFlverModel->m_flver->header.meshCount; i++)
	{
		FbxNode* pMeshNode = nullptr;

		pMeshNode = FBXTranslator::CreateModelFbxMesh(pScene, pFlverModel, pBoneList, i, morphemeToFlverBoneMap);

		if (pMeshNode != nullptr)
		{
			pModelNode->AddChild(pMeshNode);
			pBindPoses->Add(pMeshNode, FbxAMatrix());

			pMeshNodesList.push_back(pMeshNode);
		}
		else
			RDebug::DebuggerOut(g_logLevel, MsgLevel_Error, "Failed to create MeshNode object for mesh %d (file=%s)\n", i, export_path.filename().c_str());
	}

	return true;
}