#include "FBXTranslator.h"
#include "utils/NMDX/NMDX.h"
#include "morpheme/AnimSource/mrAnimSourceNSA.h"
#include "extern.h"
#include "RCore.h"

namespace
{
	inline FbxVector4 getFbxVector4(Vector3 vec)
	{
		return FbxVector4(vec.x, vec.y, vec.z, 0.f);
	}

	inline FbxVector4 getFbxVector4(Vector4 vec)
	{
		return FbxVector4(vec.x, vec.y, vec.z, vec.w);
	}

	//Converts a DirectX::XMMATRIX to an FbxAMatrix
	FbxAMatrix createFbxMatrixFromDXMatrix(DirectX::XMMATRIX m)
	{
		FbxAMatrix fbxMatrix;

		fbxMatrix.mData[0][0] = m.r[0].m128_f32[0]; fbxMatrix.mData[0][1] = m.r[0].m128_f32[1]; fbxMatrix.mData[0][2] = m.r[0].m128_f32[2]; fbxMatrix.mData[0][3] = m.r[0].m128_f32[3];
		fbxMatrix.mData[1][0] = m.r[1].m128_f32[0]; fbxMatrix.mData[1][1] = m.r[1].m128_f32[1]; fbxMatrix.mData[1][2] = m.r[1].m128_f32[2]; fbxMatrix.mData[1][3] = m.r[1].m128_f32[3];
		fbxMatrix.mData[2][0] = m.r[2].m128_f32[0]; fbxMatrix.mData[2][1] = m.r[2].m128_f32[1]; fbxMatrix.mData[2][2] = m.r[2].m128_f32[2]; fbxMatrix.mData[2][3] = m.r[2].m128_f32[3];
		fbxMatrix.mData[3][0] = m.r[3].m128_f32[0]; fbxMatrix.mData[3][1] = m.r[3].m128_f32[1]; fbxMatrix.mData[3][2] = m.r[3].m128_f32[2]; fbxMatrix.mData[3][3] = m.r[3].m128_f32[3];

		return fbxMatrix;
	}

	//Converts an FbxMatrix into an FbxAMatrix
	FbxAMatrix convertFbxMatrixToAMatrix(FbxMatrix m)
	{
		FbxVector4 translation, rotation, scale, shearing;
		double sign;

		m.GetElements(translation, rotation, shearing, scale, sign);

		FbxAMatrix fbxAMatrix;

		fbxAMatrix.SetTRS(translation, rotation, scale);

		return fbxAMatrix;
	}

	std::vector<FbxVector4> getVertices(FlverModel* model, int meshIdx)
	{
		std::vector<FlverModel::SkinnedVertex> skinnedVertices = model->getBindPoseSkinnedVertices(meshIdx);

		std::vector<FbxVector4> pointList;
		pointList.reserve(skinnedVertices.size());

		for (size_t i = 0; i < skinnedVertices.size(); i++)
			pointList.push_back(getFbxVector4(Vector3::Transform(skinnedVertices[i].vertexData.position, Matrix::CreateRotationX(DirectX::XM_PIDIV2))));

		return pointList;
	}

	std::vector<FbxVector4> getNormals(FlverModel* model, int meshIdx)
	{
		std::vector<FlverModel::SkinnedVertex> skinnedVertices = model->getBindPoseSkinnedVertices(meshIdx);

		std::vector<FbxVector4> normalList;
		normalList.reserve(skinnedVertices.size());

		for (size_t i = 0; i < skinnedVertices.size(); i++)
			normalList.push_back(getFbxVector4(Vector3::TransformNormal(skinnedVertices[i].vertexData.normal, Matrix::CreateRotationX(DirectX::XM_PIDIV2))));

		return normalList;
	}

	//Gets FLVER bone local transform
	FbxAMatrix getBoneLocalTransform(cfr::FLVER2::Bone bone)
	{
		DirectX::XMMATRIX world = DirectX::XMMatrixScaling(bone.scale.x, bone.scale.y, bone.scale.z);
		world *= DirectX::XMMatrixRotationX(bone.rot.x);
		world *= DirectX::XMMatrixRotationZ(bone.rot.z);
		world *= DirectX::XMMatrixRotationY(bone.rot.y);
		world *= DirectX::XMMatrixTranslation(bone.translation.x, bone.translation.y, bone.translation.z);

		return createFbxMatrixFromDXMatrix(world);
	}

	FbxNode* createFlverBoneNode(FbxScene* pScene, FbxPose* pBindPoses, cfr::FLVER2::Bone bone, int id)
	{
		FbxNode* pBoneNode = FbxNode::Create(pScene, RString::toNarrow(bone.name).c_str());

		FbxAMatrix boneTransform = getBoneLocalTransform(bone);

		pBoneNode->LclTranslation.Set(boneTransform.GetT());
		pBoneNode->LclRotation.Set(boneTransform.GetR());
		pBoneNode->LclScaling.Set(boneTransform.GetS());

		FbxSkeleton* pBone = FbxSkeleton::Create(pScene, RString::toNarrow(bone.name).c_str());
		pBone->SetSkeletonType(FbxSkeleton::eLimbNode);
		pBone->Color.Set(FbxDouble3(0, 0.769, 0.769));
		pBone->LimbLength = 0.1;

		pBoneNode->SetNodeAttribute(pBone);
		pBindPoses->Add(pBoneNode, boneTransform);

		return pBoneNode;
	}

	FbxNode* createMorphemeBoneNode(FbxScene* pScene, FbxPose* pBindPoses, MR::AnimRigDef* pRig, int id)
	{
		FbxNode* pBoneNode = FbxNode::Create(pScene, pRig->getBoneName(id));

		Matrix bonePose = utils::NMDX::getTransformMatrix(*pRig->getBindPoseBoneQuat(id), *pRig->getBindPoseBonePos(id));
		FbxAMatrix boneTransform = createFbxMatrixFromDXMatrix(bonePose);

		pBoneNode->LclTranslation.Set(boneTransform.GetT());
		pBoneNode->LclRotation.Set(boneTransform.GetR());
		pBoneNode->LclScaling.Set(boneTransform.GetS());

		FbxSkeleton* pBone = FbxSkeleton::Create(pScene, pRig->getBoneName(id));
		pBone->SetSkeletonType(FbxSkeleton::eLimbNode);
		pBone->Color.Set(FbxDouble3(1.0, 0.66, 0.0));
		pBone->LimbLength = 0.1;

		pBoneNode->SetNodeAttribute(pBone);
		pBindPoses->Add(pBoneNode, boneTransform);

		return pBoneNode;
	}

	//Creates an FbxNode* object called Skeleton and rotates it by 180 degrees
	FbxNode* createDummyRootBoneNode(FbxScene* pScene)
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

	inline double getTimeByAnimFrame(float animLenght, float fps, int frameNum)
	{
		return (animLenght / fps) * frameNum;
	}

	FbxAMatrix convertToFbxAMatrix(const DirectX::SimpleMath::Matrix& dxMatrix)
	{
		FbxAMatrix fbxMatrix;

		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				fbxMatrix[i][j] = dxMatrix.m[i][j];
			}
		}

		return fbxMatrix;
	}
}

namespace FT
{
	FbxNode* FbxFileTranslator::createModelFbxMesh(FbxScene* pScene, FlverModel* pFlverModel, std::vector<FbxNode*> skeletonNodes, int idx)
	{
		if (idx > pFlverModel->getNumFlverMeshes())
			return nullptr;

		std::string mesh_node_name = "mesh[" + std::to_string(idx) + "]";
		FbxNode* pMeshNode = FbxNode::Create(pScene, mesh_node_name.c_str());

		FbxMesh* pMesh = FbxMesh::Create(pScene, std::string(mesh_node_name + "_mesh").c_str());
		pMesh->CreateLayer();

		// Add vertices
		std::vector<FbxVector4> vertices = getVertices(pFlverModel, idx);
		std::vector<FbxVector4> indices = getNormals(pFlverModel, idx);

		pMesh->InitControlPoints(vertices.size());

		for (int i = 0; i < vertices.size(); i++)
			pMesh->SetControlPointAt(vertices[i], i);

		// Add normals
		FbxLayerElementNormal* pLayerNormal = (FbxLayerElementNormal*)pMesh->GetLayer(0)->CreateLayerElementOfType(FbxLayerElement::eNormal);
		pLayerNormal->SetMappingMode(FbxLayerElement::eByControlPoint);
		pLayerNormal->SetReferenceMode(FbxLayerElement::eDirect);

		for (int i = 0; i < indices.size(); i++)
			pLayerNormal->GetDirectArray().Add(indices[i]);

		// Create polygons
		for (int i = 0; i < vertices.size(); i += 3)
		{
			pMesh->BeginPolygon();

			pMesh->AddPolygon(i);
			pMesh->AddPolygon(i + 1);
			pMesh->AddPolygon(i + 2);

			pMesh->EndPolygon();
		}

		FbxSkin* pSkin = nullptr;

		std::vector<FlverModel::SkinnedVertex> skinnedVertices = pFlverModel->getBindPoseSkinnedVertices(idx);

		//Add bone weights
		if (skinnedVertices.size() > 0)
			pSkin = FbxSkin::Create(pScene, std::string(mesh_node_name + "_skin").c_str());

		if (pSkin)
		{
			for (size_t i = 0; i < pFlverModel->getNumMorphemeBones(); i++)
			{
				FbxNode* pBoneNode = skeletonNodes[i];

				FbxCluster* pCluster = FbxCluster::Create(pScene, (mesh_node_name + "_" + pFlverModel->getFlverBoneName(i) + "_cluster").c_str());
				pCluster->SetLink(pBoneNode);
				pCluster->SetLinkMode(FbxCluster::eTotalOne);
				pCluster->SetTransformLinkMatrix(skeletonNodes[i]->EvaluateGlobalTransform());

				for (int vertexIndex = 0; vertexIndex < skinnedVertices.size(); vertexIndex++)
				{
					int* indices = skinnedVertices[vertexIndex].boneIndices;
					float* weights = skinnedVertices[vertexIndex].boneWeights;

					for (int wt = 0; wt < 4; wt++)
					{
						const int flverBoneID = indices[wt];

						if (flverBoneID != -1)
						{
							const int boneID = pFlverModel->getMorphemeBoneIdByFlverBoneId(flverBoneID);

							if ((boneID != -1) && (boneID == i))
							{
								switch (wt)
								{
								case 0:
									pCluster->AddControlPointIndex(vertexIndex, weights[0]);
									break;
								case 1:
									pCluster->AddControlPointIndex(vertexIndex, weights[1]);
									break;
								case 2:
									pCluster->AddControlPointIndex(vertexIndex, weights[2]);
									break;
								case 3:
									pCluster->AddControlPointIndex(vertexIndex, weights[3]);
									break;
								}
							}
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

	std::vector<FbxNode*> FbxFileTranslator::createFbxFlverSkeleton(FbxScene* pScene, FlverModel* pFlverModel, FbxPose* pBindPoses)
	{
		std::vector<FbxNode*> boneNodes;
		boneNodes.reserve(pFlverModel->getNumFlverBones());

		for (int i = 0; i < pFlverModel->getNumFlverBones(); i++)
		{
			cfr::FLVER2::Bone bone = pFlverModel->getFlverBone(i);

			boneNodes.push_back(createFlverBoneNode(pScene, pBindPoses, bone, i));
		}

		for (int i = 0; i < pFlverModel->getNumFlverBones(); i++)
		{
			cfr::FLVER2::Bone bone = pFlverModel->getFlverBone(i);

			if (bone.parentIndex != -1)
				boneNodes[bone.parentIndex]->AddChild(boneNodes[i]);
			else
				pScene->GetRootNode()->AddChild(boneNodes[i]);
		}

		return boneNodes;
	}

	std::vector<FbxNode*> FbxFileTranslator::createFbxMorphemeSkeleton(FbxScene* pScene, MR::AnimRigDef* pRig, FbxPose* pBindPoses)
	{
		std::vector<FbxNode*> pMorphemeBoneList;
		pMorphemeBoneList.reserve(pRig->getNumBones());

		for (int i = 0; i < pRig->getNumBones(); i++)
		{
			FbxNode* pMorphemeBoneNode = createMorphemeBoneNode(pScene, pBindPoses, pRig, i);

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

	bool FbxFileTranslator::createFbxTake(FbxScene* pScene, std::vector<FbxNode*> pSkeleton, AnimObject* pAnim, std::string name, int fps)
	{
		MR::AnimationSourceHandle* animHandle = pAnim->getHandle();

		if (animHandle == nullptr)
			return false;

		const MR::AnimRigDef* rig = animHandle->getRig();
		MR::AnimSourceNSA* animSourceNSA = (MR::AnimSourceNSA*)animHandle->getAnimation();

		if (animSourceNSA->getType() != ANIM_TYPE_NSA)
		{
			g_appLog->alertMessage(MsgLevel_Error, "Unsupported animation format");
			return false;
		}

		const char* cStrName = name.c_str();

		FbxAnimStack* pAnimStack = FbxAnimStack::Create(pScene, cStrName);
		FbxAnimLayer* pAnimBaseLayer = FbxAnimLayer::Create(pScene, cStrName);

		pAnimStack->AddMember(pAnimBaseLayer);

		FbxTime start;
		start.SetSecondDouble(0.0);

		float animDuration = animSourceNSA->getDuration(animSourceNSA);

		FbxTime end;
		end.SetSecondDouble(animDuration);

		FbxTimeSpan timeSpan = FbxTimeSpan(start, end);
		pAnimStack->SetLocalTimeSpan(timeSpan);

		const int keyframeCount = fps * animDuration;
		const int boneCount = rig->getNumBones();

		const int trajectoryBoneIndex = rig->getTrajectoryBoneIndex();
		const int rootBoneIndex = rig->getCharacterRootBoneIndex();

		for (int boneIndex = 1; boneIndex < boneCount; boneIndex++)
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

				const float animTime = getTimeByAnimFrame(animDuration, fps, frame);
				FbxAMatrix transform = convertToFbxAMatrix(pAnim->getTransformAtTime(animTime, boneIndex));

				if (boneIndex == rootBoneIndex)
				{
					const int parentIdx = rig->getParentBoneIndex(rootBoneIndex);
					transform = convertToFbxAMatrix(pAnim->getTransformAtTime(animTime, boneIndex) * pAnim->getTransformAtTime(animTime, parentIdx) * pAnim->getTransformAtTime(animTime, trajectoryBoneIndex));
				}

				FbxTime keyTime;
				keyTime.SetFrame(RMath::timeToFrame(animTime, fps));

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

			FbxAnimCurveFilterGimbleKiller gimblerFilter;
			FbxAnimCurve* rotationCurves[] = { curveRX, curveRY, curveRZ };
			gimblerFilter.Apply(rotationCurves, 3);
		}

		return true;
	}

	bool FbxFileTranslator::createFbxModel(FbxScene* pScene, FlverModel* pFlverModel, std::string modelName, FbxPose* pBindPoses, std::vector<FbxNode*> pBoneList, std::filesystem::path export_path)
	{
		std::string model_node_name = modelName + "_model";

		FbxNode* pModelNode = FbxNode::Create(pScene, model_node_name.c_str());
		pScene->GetRootNode()->AddChild(pModelNode);

		std::vector<FbxNode*> pMeshNodesList;

		pMeshNodesList.reserve(pFlverModel->getNumMeshes());

		for (int i = 0; i < pFlverModel->getNumMeshes(); i++)
		{
			FbxNode* pMeshNode = nullptr;

			pMeshNode = createModelFbxMesh(pScene, pFlverModel, pBoneList, i);

			if (pMeshNode)
			{
				pModelNode->AddChild(pMeshNode);
				pBindPoses->Add(pMeshNode, FbxAMatrix());

				pMeshNodesList.push_back(pMeshNode);
			}
			else
				g_appLog->debugMessage(MsgLevel_Error, "Failed to create MeshNode object for mesh %d (file=%s)\n", i, export_path.filename().c_str());
		}

		return true;
	}

	FbxNode* FbxFileTranslator::createLightNode(FbxScene* pScene, FbxVector4 position, const char* name)
	{
		// Create a light
		FbxLight* lLight = FbxLight::Create(pScene, name);
		lLight->LightType.Set(FbxLight::ePoint); // Point light

		char nodeName[256];
		sprintf_s(nodeName, "%s_node", name);

		FbxNode* lLightNode = FbxNode::Create(pScene, nodeName);
		lLightNode->SetNodeAttribute(lLight);

		lLightNode->LclTranslation.Set(position);

		return lLightNode;
	}

	bool FbxFileTranslator::exportModel(Character* character)
	{
		bool status = true;

		g_appLog->debugMessage(MsgLevel_Info, "Exporting model to FBX for character %s\n", character->getCharacterName().c_str());

		FbxExporter* pExporter = FbxExporter::Create(g_pFbxManager, "Flver Exporter");
		pExporter->SetFileExportVersion(FBX_2014_00_COMPATIBLE);

		std::string modelOutPath = RString::toNarrow(character->getCharacterName()) + ".fbx";

		try
		{
			if (!pExporter->Initialize(modelOutPath.c_str()), g_pFbxManager->GetIOPluginRegistry()->GetNativeWriterFormat(), g_pFbxManager->GetIOSettings())
				return false;
		}
		catch (const std::exception& e)
		{
			g_appLog->panicMessage(e.what());
		}

		FbxScene* pScene = FbxScene::Create(g_pFbxManager, RString::toNarrow(character->getCharacterName()).c_str());
		pScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::eMax);
		pScene->GetGlobalSettings().SetSystemUnit(FbxSystemUnit::m);

		createLightNode(pScene, FbxVector4(1000.f, 0.f, 0.f), "Forward");
		createLightNode(pScene, FbxVector4(-1000.f, 0.f, 0.f), "Backward");

		createLightNode(pScene, FbxVector4(0.f, 1000.f, 0.f), "Up");
		createLightNode(pScene, FbxVector4(0.f, -1000.f, 0.f), "Down");

		createLightNode(pScene, FbxVector4(0.f, 0.f, 1000.f), "Left");
		createLightNode(pScene, FbxVector4(0.f, 0.f, -1000.f), "Right");

		FbxPose* pBindPoses = FbxPose::Create(g_pFbxManager, "BindPoses");
		pBindPoses->SetIsBindPose(true);

		std::vector<FbxNode*> morphemeRig = createFbxMorphemeSkeleton(pScene, character->getMorphemeCharacter()->getNetwork()->getRig(0), pBindPoses);

		FlverModel* model = character->getCharacterModelCtrl()->getModel();

		if (!createFbxModel(pScene, model, RString::toNarrow(character->getCharacterName()), pBindPoses, morphemeRig, modelOutPath))
		{
			g_appLog->debugMessage(MsgLevel_Error, "Failed to create FBX model/skeleton (chr=%ws)\n", character->getCharacterName().c_str());
			status = false;
		}

		pScene->AddPose(pBindPoses);

		//Shutdown
		pExporter->Export(pScene);
		pScene->Destroy(true);
		pExporter->Destroy(true);
	}

	bool FbxFileTranslator::exportAnimation(Character* character, std::wstring path, int animSetIdx, int animIdx, int fps, bool includeMeshes)
	{
		bool status = true;

		MorphemeCharacterDef* characterDef = character->getMorphemeCharacterDef();
		AnimObject* anim = characterDef->getAnimation(0, animIdx);

		int animId = anim->getAnimID();

		if (characterDef == nullptr)
			throw("characterDef was nullptr\n");

		std::string animName = RString::toNarrow(path) + RString::removeExtension(characterDef->getAnimFileLookUp()->getSourceFilename(animId));

		g_appLog->debugMessage(MsgLevel_Info, "\tExporting animation \"%s\" to FBX (%ws)\n", animName.c_str(), character->getCharacterName().c_str());

		FbxExporter* pExporter = FbxExporter::Create(g_pFbxManager, "Flver Exporter");
		pExporter->SetFileExportVersion(FBX_2014_00_COMPATIBLE);

		std::string outPath = animName + ".fbx";

		try
		{
			if (!pExporter->Initialize(outPath.c_str()), g_pFbxManager->GetIOPluginRegistry()->GetNativeWriterFormat(), g_pFbxManager->GetIOSettings())
				return false;
		}
		catch (const std::exception& e)
		{
			g_appLog->panicMessage(e.what());
		}

		FbxScene* pScene = FbxScene::Create(g_pFbxManager, animName.c_str());
		pScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::eMax);
		pScene->GetGlobalSettings().SetSystemUnit(FbxSystemUnit::m);

		createLightNode(pScene, FbxVector4(1000.f, 0.f, 0.f), "Forward");
		createLightNode(pScene, FbxVector4(-1000.f, 0.f, 0.f), "Backward");

		createLightNode(pScene, FbxVector4(0.f, 1000.f, 0.f), "Up");
		createLightNode(pScene, FbxVector4(0.f, -1000.f, 0.f), "Down");

		createLightNode(pScene, FbxVector4(0.f, 0.f, 1000.f), "Left");
		createLightNode(pScene, FbxVector4(0.f, 0.f, -1000.f), "Right");

		FbxPose* pBindPoses = FbxPose::Create(g_pFbxManager, "BindPoses");
		pBindPoses->SetIsBindPose(true);

		std::vector<FbxNode*> morphemeRig = createFbxMorphemeSkeleton(pScene, character->getMorphemeCharacter()->getNetwork()->getRig(0), pBindPoses);

		pScene->AddPose(pBindPoses);

		if (includeMeshes)
		{
			FlverModel* model = character->getCharacterModelCtrl()->getModel();

			if (!createFbxModel(pScene, model, RString::toNarrow(character->getCharacterName()), pBindPoses, morphemeRig, outPath))
			{
				g_appLog->debugMessage(MsgLevel_Error, "Failed to create FBX model/skeleton (%ws)\n", character->getCharacterName().c_str());
				status = false;
			}
		}

		if (!createFbxTake(pScene, morphemeRig, characterDef->getAnimationById(animSetIdx, animId), characterDef->getAnimFileLookUp()->getTakeName(animId), fps))
		{
			g_appLog->debugMessage(MsgLevel_Error, "Failed to create FBX take (%ws)\n", character->getCharacterName().c_str());
			status = false;
		}

		//Shutdown
		pExporter->Export(pScene);
		pScene->Destroy(true);
		pExporter->Destroy(true);

		return status;
	}
}