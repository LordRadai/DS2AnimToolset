#include "XMDTranslator.h"
#include "RCore.h"
#include "extern.h"
#include "morphemeEditor.h"
#include "SimpleMath.h"
#include "morpheme/AnimSource/mrAnimSourceNSA.h"

namespace
{
	inline XMD::XQuaternion convertToXmdQuat(const NMP::Quat quat)
	{
		return XMD::XQuaternion(quat.x, quat.y, quat.z, quat.w);
	}

	inline XMD::XQuaternion convertToXmdQuat(const DirectX::SimpleMath::Quaternion quat)
	{
		return XMD::XQuaternion(quat.x, quat.y, quat.z, quat.w);
	}

	inline XMD::XVector3 convertToXmdVec3(const NMP::Vector3 vec)
	{
		return XMD::XVector3(vec.x, vec.y, vec.z);
	}

	inline XMD::XVector3 convertToXmdVec3(const DirectX::SimpleMath::Vector3 vec)
	{
		return XMD::XVector3(vec.x, vec.y, vec.z);
	}

	XMD::XBone* getBoneByName(XMD::XBoneList boneList, std::string name)
	{
		for (size_t i = 0; i < boneList.size(); i++)
		{
			if (name.compare(boneList[i]->GetName()) == 0)
				return boneList[i];
		}
	}

	XMD::XBone::XBoneList getChildJoints(XMD::XBoneList bones, MR::AnimRigDef* rig, int id)
	{
		XMD::XBone::XBoneList children;

		std::string boneName = rig->getBoneName(id);

		for (size_t i = 0; i < rig->getNumBones(); i++)
		{
			int parentIdx = rig->getParentBoneIndex(i);

			if (parentIdx == id)
			{
				std::string name = rig->getBoneName(i);

				XMD::XBone* xmdBone = getBoneByName(bones, name);

				children.push_back(xmdBone);
			}
		}

		return children;
	}
	
	XMD::XVector3Array getPoints(FlverModel* model, int meshIdx)
	{
		XMD::XVector3Array pointList;

		//Get vertices directly from the Flver data. Our processed bind pose is Y-up while Flver is z-up
		std::vector<Vector3> vertices = model->getFlverMeshVertices(meshIdx, true);

		for (size_t i = 0; i < model->getNumVerticesInMesh(meshIdx); i++)
			pointList.push_back(convertToXmdVec3(vertices[i]));

		return pointList;
	}

	XMD::XNormalList getNormals(FlverModel* model, int meshIdx)
	{
		XMD::XNormalList normalList;

		std::vector<Vector3> normals = model->getFlverMeshNormals(meshIdx, true);

		for (size_t i = 0; i < model->getNumVerticesInMesh(meshIdx); i++)
			normalList.push_back(convertToXmdVec3(normals[i]));

		return normalList;
	}

	XMD::XVector3Array getTangents(FlverModel* model, int meshIdx)
	{
		XMD::XVector3Array tangentList;

		std::vector<Vector3> tangents = model->getFlverMeshTangents(meshIdx, true);

		for (size_t i = 0; i < model->getNumVerticesInMesh(meshIdx); i++)
			tangentList.push_back(convertToXmdVec3(tangents[i]));

		return tangentList;
	}

	XMD::XVector3Array getBiTangents(FlverModel* model, int meshIdx)
	{
		XMD::XVector3Array bitangentList;

		std::vector<Vector3> bitangents = model->getFlverMeshBiTangents(meshIdx, true);

		for (size_t i = 0; i < model->getNumVerticesInMesh(meshIdx); i++)
			bitangentList.push_back(convertToXmdVec3(bitangents[i]));

		return bitangentList;
	}

	XM2::XQuaternion getBoneTransformQuatAtTime(AnimObject* animObj, float time, int boneId)
	{
		if (boneId == animObj->getHandle()->getRig()->getCharacterRootBoneIndex())
		{
			int trajectoryBoneId = animObj->getHandle()->getRig()->getTrajectoryBoneIndex();

			return convertToXmdQuat(animObj->getTransformQuatAtTime(time, trajectoryBoneId) * animObj->getTransformQuatAtTime(time, boneId));
		}

		return convertToXmdQuat(animObj->getTransformQuatAtTime(time, boneId));
	}

	XM2::XVector3 getBoneTransformPosAtTime(AnimObject* animObj, float time, int boneId)
	{
		if (boneId == animObj->getHandle()->getRig()->getCharacterRootBoneIndex())
		{
			int trajectoryBoneId = animObj->getHandle()->getRig()->getTrajectoryBoneIndex();

			return convertToXmdVec3(animObj->getTransformPosAtTime(time, trajectoryBoneId) + animObj->getTransformPosAtTime(time, boneId));
		}

		return convertToXmdVec3(animObj->getTransformPosAtTime(time, boneId));
	}

	//Returns true if the transforms are unchaning
	bool getSampledRotations(AnimObject* animObj, XM2::XQuaternionArray& rotations, int boneId, int sampleRate)
	{
		bool unchanging = true;

		int animLenFrames = RMath::timeToFrame(animObj->getAnimLenght(), sampleRate);

		rotations.reserve(animLenFrames);

		for (size_t i = 0; i < animLenFrames; i++)
		{
			float time = RMath::frameToTime(i, sampleRate);

			XMD::XQuaternion rotation = getBoneTransformQuatAtTime(animObj, time, boneId);

			rotations.push_back(rotation);

			if (i != 0)
			{
				if (rotations[i] != rotations[i - 1])
					unchanging = false;
			}
		}

		return unchanging;
	}

	//Returns true if the transforms are unchaning
	bool getSampledTranslation(AnimObject* animObj, XM2::XVector3Array& translations, int boneId, int sampleRate)
	{
		bool unchanging = true;

		int animLenFrames = RMath::timeToFrame(animObj->getAnimLenght(), sampleRate);

		translations.reserve(animLenFrames);

		for (size_t i = 0; i < animLenFrames; i++)
		{
			float time = RMath::frameToTime(i, sampleRate);

			XMD::XVector3 translation = getBoneTransformPosAtTime(animObj, time, boneId);

			translations.push_back(translation);

			if (i != 0)
			{
				if (translations[i] != translations[i - 1])
					unchanging = false;
			}
		}

		return unchanging;
	}
}

namespace XMDTranslator
{
	XMD::XModel* createModel(MR::AnimRigDef* rig, FlverModel* model, const char* originalFile, bool includeMeshes)
	{
		XMD::XModel* xmd = new XMD::XModel;
		XMD::XInfo* info = xmd->CreateInfo();

		info->SetOriginalFile(originalFile);
		info->SetSoftware(APPNAME_A);

		XMD::XBoneList joints;

		for (size_t i = 0; i < rig->getNumBones(); i++)
		{
			//CharacterWorldSpaceTM is not a bone, its a control node added on export by morpheme, so we skip it
			if (i == 0)
				continue;

			joints.push_back(XMDTranslator::createJoint(xmd, rig, i));
		}

		for (size_t i = 0; i < rig->getNumBones(); i++)
		{
			//CharacterWorldSpaceTM is not a bone, its a control node added on export by morpheme, so we skip it
			if (i == 0)
				continue;

			XMD::XBone::XBoneList children = getChildJoints(joints, rig, i);
			XMD::XBone* bone = getBoneByName(joints, rig->getBoneName(i));

			bone->SetChildren(children);
			bone->UpdateBindPose();
		}

		if (includeMeshes && (model != nullptr))
		{
			XMD::XJoint* joint = static_cast<XMD::XJoint*>(getBoneByName(joints, rig->getBoneName(rig->getTrajectoryBoneIndex())));

			if (joint != nullptr)
			{
				XMD::XLocator* locator = XMDUtils::createJointLocator(xmd, joint);
				joint->AddInstance(xmd->CreateGeometryInstance(joint, locator));
			}

			for (size_t i = 0; i < model->getNumMeshes(); i++)
				XMDTranslator::createPolygonMesh(xmd, model, i);
		}

		return xmd;
	}

	XMD::XSkinCluster* createSkin(XMD::XModel* xmd, XMD::XPolygonMesh* mesh, FlverModel* model, int meshIdx)
	{
		XMD::XSkinCluster* skin = static_cast<XMD::XSkinCluster*>(xmd->CreateNode(XMD::XFn::SkinCluster));
		MR::AnimRigDef* rig = model->getRig();

		char clusterName[256];
		sprintf_s(clusterName, "Skin");

		XMD::XBoneList bones;
		xmd->GetBones(bones);

		skin->SetName(clusterName);

		skin->SetEnvelopeWeight(1.f);
		skin->AddAffected(mesh);

		XM2::vector<XMD::XSkinnedVertex> skinnedVertices = XMDTranslator::createSkinnedVertexList(bones, skin, model, meshIdx);

		skin->SetSkinWeights(skinnedVertices);

		XMD::XDeformerList deformerList(1, skin);

		mesh->SetDeformerQueue(deformerList);

		XMDTranslator::createClusterObjSet(xmd, skin, mesh, meshIdx);

		return skin;
	}

	XMD::XSkinCluster* createSkin(XMD::XModel* xmd, XMD::XMesh* mesh, FlverModel* model, int meshIdx)
	{
		XMD::XSkinCluster* skin = static_cast<XMD::XSkinCluster*>(xmd->CreateNode(XMD::XFn::SkinCluster));
		MR::AnimRigDef* rig = model->getRig();

		char clusterName[256];
		sprintf_s(clusterName, "Skin");

		XMD::XBoneList bones;
		xmd->GetBones(bones);

		skin->SetName(clusterName);

		skin->SetEnvelopeWeight(1.f);
		skin->AddAffected(mesh);

		XM2::vector<XMD::XSkinnedVertex> skinnedVertices = XMDTranslator::createSkinnedVertexList(bones, skin, model, meshIdx);

		skin->SetSkinWeights(skinnedVertices);

		XMD::XDeformerList deformerList(1, skin);

		mesh->SetDeformerQueue(deformerList);

		XMDTranslator::createClusterObjSet(xmd, skin, mesh, meshIdx);

		return skin;
	}

	XM2::vector<XMD::XSkinnedVertex> createSkinnedVertexList(XMD::XBoneList bones, XMD::XSkinCluster* skin, FlverModel* model, int meshIdx)
	{
		MR::AnimRigDef* rig = model->getRig();

		XM2::vector<XMD::XSkinnedVertex> skinnedVertices;

		int numVertices = model->getNumVerticesInMesh(meshIdx);

		std::vector<Vector4> weights = model->getFlverMeshBoneWeights(meshIdx);
		std::vector<std::vector<int>> indices = model->getFlverMeshBoneIndices(meshIdx);

		for (size_t i = 0; i < weights.size(); i++)
		{
			XMD::XSkinnedVertex skinnedVertex;

			for (size_t j = 0; j < 4; j++)
			{
				int boneIdx = model->getMorphemeBoneIdByFlverBoneId(indices[i][j]);

				if (boneIdx != -1)
				{
					XMD::XBone* bone = getBoneByName(bones, rig->getBoneName(boneIdx));

					if (bone != nullptr)
					{
						XM2::XU32 id = skin->AddInfluence(bone);

						if (boneIdx != -1)
						{
							switch (j)
							{
							case 0:
								skinnedVertex.push_back(XMD::XSkinWeight(weights[i].x, id));
								break;
							case 1:
								skinnedVertex.push_back(XMD::XSkinWeight(weights[i].y, id));
								break;
							case 2:
								skinnedVertex.push_back(XMD::XSkinWeight(weights[i].z, id));
								break;
							case 3:
								skinnedVertex.push_back(XMD::XSkinWeight(weights[i].w, id));
								break;
							}
						}
					}
				}
			}

			skinnedVertices.push_back(skinnedVertex);
		}

		return skinnedVertices;
	}

	XMD::XBone* createBone(XMD::XModel* xmd, MR::AnimRigDef* rig, int idx)
	{
		XMD::XBone* bone = static_cast<XMD::XBone*>(xmd->CreateNode(XMD::XFn::Bone));
		bone->SetName(rig->getBoneName(idx));

		bone->SetTranslation(convertToXmdVec3(*rig->getBindPoseBonePos(idx)));
		bone->SetRotation(convertToXmdQuat(*rig->getBindPoseBoneQuat(idx)));

		return bone;
	}

	XMD::XJoint* createJoint(XMD::XModel* xmd, MR::AnimRigDef* rig, int idx)
	{
		XMD::XJoint* joint = static_cast<XMD::XJoint*>(xmd->CreateNode(XMD::XFn::Joint));

		joint->SetName(rig->getBoneName(idx));

		joint->SetTranslation(convertToXmdVec3(*rig->getBindPoseBonePos(idx)));
		joint->SetRotation(convertToXmdQuat(*rig->getBindPoseBoneQuat(idx)));

		return joint;
	}

	XMD::XMesh* createMesh(XMD::XModel* xmd, FlverModel* model, int meshIdx)
	{
		XMD::XMesh* mesh = static_cast<XMD::XMesh*>(xmd->CreateNode(XMD::XFn::Mesh));

		char meshName[256];
		sprintf_s(meshName, "mesh[%d]Shape", meshIdx);

		XMD::XVector3Array points = getPoints(model, meshIdx);

		mesh->SetName(meshName);
		mesh->SetIsIntermediateObject(false);		
		mesh->SetPoints(points);

		XMDTranslator::createSkin(xmd, mesh, model, meshIdx);
		XMDTranslator::createMeshBone(xmd, mesh, meshIdx);

		return mesh;
	}

	XMD::XPolygonMesh* createPolygonMesh(XMD::XModel* xmd, FlverModel* model, int meshIdx)
	{
		XMD::XPolygonMesh* mesh = static_cast<XMD::XPolygonMesh*>(xmd->CreateNode(XMD::XFn::PolygonMesh));
		
		char meshName[256];
		sprintf_s(meshName, "mesh[%d]Shape", meshIdx);

		XMD::XVector3Array points = getPoints(model, meshIdx);

		mesh->SetName(meshName);
		mesh->SetIsIntermediateObject(false);

		int numPolygons = points.size() / 3;
		int numVertexIndices = points.size();

		XMD::XU32Array polygons;
		for (size_t i = 0; i < numPolygons; i++)
			polygons.push_back(3);

		XMD::XS32Array indices;
		for (size_t i = 0; i < numVertexIndices; i++)
			indices.push_back(i);

		mesh->SetPolyCounts(polygons, indices);

		XMD::XIndexSet* pointsIndices = mesh->CreateIndexSet("points");

		pointsIndices->SetData(indices);

		mesh->SetPoints(points);

		XMD::XIndexSet* normalIndices = mesh->CreateIndexSet("normals");
		normalIndices->SetData(indices);

		XMD::XNormalList normalList = getNormals(model, meshIdx);
		XMD::XVertexSet* normals = mesh->CreateVertexSet("normals", normalList.size(), 3, XMD::XVertexSet::kNormal);

		for (size_t i = 0; i < normalList.size(); i++)
			normals->SetElement(normalList[i], i);

		normals->SetIndexSet(normalIndices);

		XMD::XVector3Array tangentsList = getTangents(model, meshIdx);
		XMD::XVertexSet* tangents = mesh->CreateVertexSet("tangents", tangentsList.size(), 3, XMD::XVertexSet::kTangent);

		for (size_t i = 0; i < tangentsList.size(); i++)
			tangents->SetElement(tangentsList[i], i);

		tangents->SetIndexSet(normalIndices);

		XMD::XVector3Array bitangentsList = getBiTangents(model, meshIdx);
		XMD::XVertexSet* bitangents = mesh->CreateVertexSet("bitangents", bitangentsList.size(), 3, XMD::XVertexSet::kBiTangent);

		for (size_t i = 0; i < bitangentsList.size(); i++)
			bitangents->SetElement(bitangentsList[i], i);

		bitangents->SetIndexSet(normalIndices);

		XMDTranslator::createSkin(xmd, mesh, model, meshIdx);
		XMDTranslator::createMeshBone(xmd, mesh, meshIdx);

		return mesh;
	}

	XMD::XBone* createMeshBone(XMD::XModel* xmd, XMD::XPolygonMesh* mesh, int meshIdx)
	{
		XMD::XBone* bone = static_cast<XMD::XBone*>(xmd->CreateNode(XMD::XFn::Bone));

		char meshName[256];
		sprintf_s(meshName, "mesh[%d]", meshIdx);

		bone->SetName(meshName);
		bone->AddInstance(xmd->CreateGeometryInstance(bone, mesh));
		bone->UpdateBindPose();

		return bone;
	}

	XMD::XBone* createMeshBone(XMD::XModel* xmd, XMD::XMesh* mesh, int meshIdx)
	{
		XMD::XBone* bone = static_cast<XMD::XBone*>(xmd->CreateNode(XMD::XFn::Bone));

		char meshName[256];
		sprintf_s(meshName, "mesh[%d]", meshIdx);

		bone->SetName(meshName);
		bone->AddInstance(xmd->CreateGeometryInstance(bone, mesh));
		bone->UpdateBindPose();

		return bone;
	}

	//XAnimCycle is deprecated in the XMD SDK, but it's supported by morphemeConnect 3.6.2
	XMD::XAnimCycle* createBindPoseAnimCycle(XMD::XModel* xmd, MR::AnimRigDef* rig)
	{
		XMD::XAnimCycle* animCycle = static_cast<XMD::XAnimCycle*>(xmd->CreateNode(XMD::XFn::AnimCycle));

		animCycle->SetName("untitled");
		animCycle->SetFrameRate(30);
		animCycle->SetFrameTimes(0, 30, 30);

		for (size_t i = 0; i < rig->getNumBones(); i++)
		{
			//CharacterWorldSpaceTM is never animated since its a control bone added by morpheme on export
			if (i == 0)
				continue;

			XMD::XSampledKeys* sampleKeys = animCycle->AddSampledKeys(i);
			sampleKeys->SetSize(30);

			XMD::XVector3 bindPosePos = convertToXmdVec3(*rig->getBindPoseBonePos(i));
			XMD::XQuaternion bindPoseQuat = convertToXmdQuat(*rig->getBindPoseBoneQuat(i));

			for (size_t j = 0; j < 30; j++)
			{
				sampleKeys->TranslationKeys()[j] = bindPosePos;
				sampleKeys->RotationKeys()[j] = bindPoseQuat;
			}
		}

		return animCycle;
	}

	//XAnimCycle is deprecated in the XMD SDK, but it's supported by morphemeConnect 3.6.2
	XMD::XAnimCycle* createAnimCycle(XMD::XModel* xmd, AnimObject* animObj, const char* takeName)
	{
		if (animObj->getHandle() == nullptr)
			return nullptr;

		XMD::XAnimCycle* animCycle = static_cast<XMD::XAnimCycle*>(xmd->CreateNode(XMD::XFn::AnimCycle));

		int animLenFrames = RMath::timeToFrame(animObj->getAnimLenght(), 30);

		animCycle->SetName(takeName);
		animCycle->SetFrameRate(30);
		animCycle->SetFrameTimes(0, animLenFrames, animLenFrames);

		const MR::RigToAnimMap* rigToAnimMap = animObj->getHandle()->getRigToAnimMap();

		if (rigToAnimMap == nullptr)
			return animCycle;

		for (size_t i = 0; i < rigToAnimMap->getNumRigBones(); i++)
		{
			//CharacterWorldSpaceTM is never animated since its a control bone added by morpheme on export
			if (i == 0)
				continue;

			XMD::XSampledKeys* sampleKeys = animCycle->AddSampledKeys(i);
			sampleKeys->SetSize(animLenFrames);

			for (size_t j = 0; j < animLenFrames; j++)
			{
				float time = RMath::frameToTime(j, 30);

				sampleKeys->TranslationKeys()[j] = getBoneTransformPosAtTime(animObj, time, i);
				sampleKeys->RotationKeys()[j] = getBoneTransformQuatAtTime(animObj, time, i);
			}
		}

		return animCycle;
	}

	XMD::XAnimationTake* createBindPoseAnimTake(XMD::XModel* xmd, MR::AnimRigDef* rig)
	{
		XMD::XAnimationTake* animTake = static_cast<XMD::XAnimationTake*>(xmd->CreateNode(XMD::XFn::AnimationTake));

		constexpr int fps = 30;

		int animLenFrames = 30;

		animTake->SetName("untitled");
		animTake->SetStartTime(0);
		animTake->SetEndTime(animLenFrames);
		animTake->SetNumFrames(animLenFrames);
		animTake->SetFramesPerSecond(fps);
		animTake->SetTime(0);

		XMD::XBoneList boneList;
		xmd->GetBones(boneList);

		for (size_t i = 0; i < rig->getNumBones(); i++)
		{
			//CharacterWorldSpaceTM is never animated since its a control bone added by morpheme on export
			if (i == 0)
				continue;

			std::string boneName = rig->getBoneName(i);

			XMD::XBone* bone = getBoneByName(boneList, boneName);

			if (bone)
			{
				XMD::XAnimatedNode* animNode = animTake->CreateNodeAnimation(bone);

				XMD::XAnimatedAttribute* rotAttr = animNode->CreateAnimatedAttribute("rotation");
				XMD::XAnimatedAttribute* scaleAttr = animNode->CreateAnimatedAttribute("scale");
				XMD::XAnimatedAttribute* posAttr = animNode->CreateAnimatedAttribute("translate");

				XMD::XPoseStream* scalePose = scaleAttr->CreatePoseStream();
				scalePose->Set(XM2::XVector3(1, 1, 1));

				XMD::XPoseStream* rotPose = rotAttr->CreatePoseStream();
				rotPose->Set(convertToXmdQuat(*rig->getBindPoseBoneQuat(i)));

				XMD::XPoseStream* translPose = posAttr->CreatePoseStream();
				translPose->Set(convertToXmdVec3(*rig->getBindPoseBonePos(i)));
			}
		}

		return animTake;
	}

	XMD::XAnimationTake* createAnimTake(XMD::XModel* xmd, AnimObject* animObj, const char* takeName)
	{
		if (animObj->getHandle() == nullptr)
			return nullptr;

		XMD::XAnimationTake* animTake = static_cast<XMD::XAnimationTake*>(xmd->CreateNode(XMD::XFn::AnimationTake));

		constexpr int fps = 30;

		const MR::AnimRigDef* rig = animObj->getHandle()->getRig();
		int animLenFrames = RMath::timeToFrame(animObj->getAnimLenght(), fps);

		animTake->SetName(takeName);
		animTake->SetStartTime(0);
		animTake->SetEndTime(animLenFrames);
		animTake->SetNumFrames(animLenFrames);
		animTake->SetFramesPerSecond(fps);
		animTake->SetTime(0);

		XMD::XBoneList boneList;
		xmd->GetBones(boneList);

		for (size_t i = 0; i < rig->getNumBones(); i++)
		{
			//CharacterWorldSpaceTM is never animated since its a control bone added by morpheme on export
			if (i == 0)
				continue;

			XMDTranslator::createAnimatedNode(animTake, boneList, animObj, i, fps);
		}

		return animTake;
	}

	XMD::XAnimatedNode* createAnimatedNode(XMD::XAnimationTake* animTake, XMD::XBoneList boneList, AnimObject* animObj, int boneId, int fps)
	{
		const MR::AnimRigDef* rig = animObj->getHandle()->getRig();
		int animLenFrames = RMath::timeToFrame(animObj->getAnimLenght(), fps);

		std::string boneName = rig->getBoneName(boneId);

		XMD::XBone* bone = getBoneByName(boneList, boneName);

		if (bone != nullptr)
		{
			XMD::XAnimatedNode* animNode = animTake->CreateNodeAnimation(bone);

			XMD::XAnimatedAttribute* rotAttr = animNode->CreateAnimatedAttribute("rotation");
			XMD::XAnimatedAttribute* scaleAttr = animNode->CreateAnimatedAttribute("scale");
			XMD::XAnimatedAttribute* posAttr = animNode->CreateAnimatedAttribute("translate");

			XMD::XPoseStream* scalePose = scaleAttr->CreatePoseStream();
			scalePose->Set(XM2::XVector3(1, 1, 1));

			XM2::XQuaternionArray rotations;
			if (!getSampledRotations(animObj, rotations, boneId, fps))
			{
				XMD::XSampledStream* sampledRot = rotAttr->CreateSampledStream();

				for (size_t i = 0; i < animLenFrames; i++)
					sampledRot->SetSample(i, rotations[i]);
			}
			else
			{
				XMD::XPoseStream* poseRot = rotAttr->CreatePoseStream();
				poseRot->Set(rotations[0]);
			}

			XM2::XVector3Array translations;
			if (!getSampledTranslation(animObj, translations, boneId, fps))
			{
				XMD::XSampledStream* sampledPos = posAttr->CreateSampledStream();

				for (size_t i = 0; i < animLenFrames; i++)
					sampledPos->SetSample(i, translations[i]);
			}
			else
			{
				XMD::XPoseStream* poseRot = posAttr->CreatePoseStream();
				poseRot->Set(translations[0]);
			}

			return animNode;
		}

		return nullptr;
	}

	XMD::XObjectSet* createClusterObjSet(XMD::XModel* xmd, XMD::XSkinCluster* skin, XMD::XPolygonMesh* mesh, int meshIdx)
	{
		XMD::XObjectSet* objectSet = static_cast<XMD::XObjectSet*>(xmd->CreateNode(XMD::XFn::ObjectSet));

		char setName[256];
		sprintf_s(setName, "skinCluster%dSet", meshIdx);

		XMD::XList items;
		items.push_back(mesh);
		items.push_back(skin);

		objectSet->SetName(setName);
		objectSet->SetItems(items);

		return objectSet;
	}

	XMD::XObjectSet* createClusterObjSet(XMD::XModel* xmd, XMD::XSkinCluster* skin, XMD::XMesh* mesh, int meshIdx)
	{
		XMD::XObjectSet* objectSet = static_cast<XMD::XObjectSet*>(xmd->CreateNode(XMD::XFn::ObjectSet));

		char setName[256];
		sprintf_s(setName, "skinCluster%dSet", meshIdx);

		XMD::XList items;
		items.push_back(mesh);
		items.push_back(skin);

		objectSet->SetName(setName);
		objectSet->SetItems(items);

		return objectSet;
	}
}