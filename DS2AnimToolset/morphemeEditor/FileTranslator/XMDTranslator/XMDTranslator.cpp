#include "XMDTranslator.h"
#include "RCore.h"
#include "extern.h"
#include "morphemeEditor.h"
#include <SimpleMath.h>
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
	
	XMD::XVector3Array getVertices(FlverModel* model, int meshIdx)
	{
		std::vector<FlverModel::SkinnedVertex> skinnedVertices = model->getBindPoseSkinnedVertices(meshIdx);

		XMD::XVector3Array pointList;
		pointList.reserve(skinnedVertices.size());

		for (size_t i = 0; i < skinnedVertices.size(); i++)
			pointList.push_back(convertToXmdVec3(Vector3::Transform(skinnedVertices[i].vertexData.position, Matrix::CreateRotationX(DirectX::XM_PIDIV2))));

		return pointList;
	}

	XMD::XNormalList getNormals(FlverModel* model, int meshIdx)
	{
		std::vector<FlverModel::SkinnedVertex> skinnedVertices = model->getBindPoseSkinnedVertices(meshIdx);

		XMD::XVector3Array normalList;
		normalList.reserve(skinnedVertices.size());

		for (size_t i = 0; i < skinnedVertices.size(); i++)
			normalList.push_back(convertToXmdVec3(Vector3::TransformNormal(skinnedVertices[i].vertexData.normal, Matrix::CreateRotationX(DirectX::XM_PIDIV2))));

		return normalList;
	}

	Matrix getBoneTransformAtTime(AnimObject* animObj, float time, int boneId)
	{
		const int trajectoryBoneID = animObj->getHandle()->getRig()->getTrajectoryBoneIndex();
		const int rootBoneID = animObj->getHandle()->getRig()->getCharacterRootBoneIndex();
		Matrix transform = animObj->getTransformAtTime(time, boneId);

		if (boneId == rootBoneID)
		{
			const int parentIdx = animObj->getHandle()->getRig()->getParentBoneIndex(rootBoneID);
			transform *= animObj->getTransformAtTime(time, parentIdx) * animObj->getTransformAtTime(time, trajectoryBoneID);
		}

		return transform;
	}

	XM2::XQuaternion getBoneTransformQuatAtTime(AnimObject* animObj, float time, int boneId)
	{
		const int trajectoryBoneID = animObj->getHandle()->getRig()->getTrajectoryBoneIndex();
		const int rootBoneID = animObj->getHandle()->getRig()->getCharacterRootBoneIndex();
		Matrix transform = getBoneTransformAtTime(animObj, time, boneId);

		Vector3 position;
		Quaternion rotation;
		Vector3 scale;

		transform.Decompose(scale, rotation, position);

		return convertToXmdQuat(rotation);
	}

	XM2::XVector3 getBoneTransformPosAtTime(AnimObject* animObj, float time, int boneId)
	{
		const int trajectoryBoneID = animObj->getHandle()->getRig()->getTrajectoryBoneIndex();
		const int rootBoneID = animObj->getHandle()->getRig()->getCharacterRootBoneIndex();
		Matrix transform = getBoneTransformAtTime(animObj, time, boneId);

		Vector3 position;
		Quaternion rotation;
		Vector3 scale;

		transform.Decompose(scale, rotation, position);

		return convertToXmdVec3(position);
	}

	XM2::XVector3 getBoneTransformScaleAtTime(AnimObject* animObj, float time, int boneId)
	{
		const int trajectoryBoneID = animObj->getHandle()->getRig()->getTrajectoryBoneIndex();
		const int rootBoneID = animObj->getHandle()->getRig()->getCharacterRootBoneIndex();
		Matrix transform = getBoneTransformAtTime(animObj, time, boneId);

		Vector3 position;
		Quaternion rotation;
		Vector3 scale;

		transform.Decompose(scale, rotation, position);

		return convertToXmdVec3(scale);
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

namespace FT
{
	XMD::XModel* XMDFileTranslator::createModel(MR::AnimRigDef* rig, FlverModel* model, const char* originalFile, bool includeMeshes)
	{
		XMD::XModel* xmd = new XMD::XModel;
		XMD::XInfo* info = xmd->CreateInfo();

		info->SetOriginalFile(originalFile);
		info->SetSoftware(APPNAME_A);

		XMD::XBoneList joints;

		//Create all joints
		for (size_t i = 1; i < rig->getNumBones(); i++)
			joints.push_back(createJoint(xmd, rig, i));

		//Loop back over to set children
		for (size_t i = 1; i < rig->getNumBones(); i++)
		{
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
				createPolygonMesh(xmd, model, i);
		}

		return xmd;
	}

	XMD::XSkinCluster* XMDFileTranslator::createSkin(XMD::XModel* xmd, XMD::XPolygonMesh* mesh, FlverModel* model, int meshIdx)
	{
		XMD::XSkinCluster* skin = static_cast<XMD::XSkinCluster*>(xmd->CreateNode(XMD::XFn::SkinCluster));
		MR::AnimRigDef* rig = model->getRig();

		char skinName[256];
		sprintf_s(skinName, "mesh[%d]Skin", meshIdx);

		XMD::XBoneList bones;
		xmd->GetBones(bones);

		skin->SetName(skinName);

		skin->SetEnvelopeWeight(1.f);
		skin->AddAffected(mesh);

		XM2::vector<XMD::XSkinnedVertex> skinnedVertices = createSkinnedVertexList(bones, skin, model, meshIdx);

		skin->SetSkinWeights(skinnedVertices);

		XMD::XDeformerList deformerList(1, skin);

		mesh->SetDeformerQueue(deformerList);

		createClusterObjSet(xmd, skin, mesh, meshIdx);

		return skin;
	}

	XMD::XSkinCluster* XMDFileTranslator::createSkin(XMD::XModel* xmd, XMD::XMesh* mesh, FlverModel* model, int meshIdx)
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

		XM2::vector<XMD::XSkinnedVertex> skinnedVertices = createSkinnedVertexList(bones, skin, model, meshIdx);

		skin->SetSkinWeights(skinnedVertices);

		XMD::XDeformerList deformerList(1, skin);

		mesh->SetDeformerQueue(deformerList);

		createClusterObjSet(xmd, skin, mesh, meshIdx);

		return skin;
	}

	XM2::vector<XMD::XSkinnedVertex> XMDFileTranslator::createSkinnedVertexList(XMD::XBoneList bones, XMD::XSkinCluster* skin, FlverModel* model, int meshIdx)
	{
		MR::AnimRigDef* rig = model->getRig();

		XM2::vector<XMD::XSkinnedVertex> skinnedVertices;

		std::vector<FlverModel::SkinnedVertex> vertexData = model->getBindPoseSkinnedVertices(meshIdx);

		for (size_t i = 0; i < vertexData.size(); i++)
		{
			XMD::XSkinnedVertex skinnedVertex;

			for (size_t wt = 0; wt < 4; wt++)
			{
				int boneIdx = model->getMorphemeBoneIdByFlverBoneId(vertexData[i].boneIndices[wt]);

				if (boneIdx != -1)
				{
					XMD::XBone* bone = getBoneByName(bones, rig->getBoneName(boneIdx));

					if (bone != nullptr)
					{
						XM2::XU32 id = skin->AddInfluence(bone);
						skinnedVertex.push_back(XMD::XSkinWeight(vertexData[i].boneWeights[wt], id));
					}
				}
			}

			skinnedVertices.push_back(skinnedVertex);
		}

		return skinnedVertices;
	}

	XMD::XBone* XMDFileTranslator::createBone(XMD::XModel* xmd, MR::AnimRigDef* rig, int idx)
	{
		XMD::XBone* bone = static_cast<XMD::XBone*>(xmd->CreateNode(XMD::XFn::Bone));
		bone->SetName(rig->getBoneName(idx));

		bone->SetTranslation(convertToXmdVec3(*rig->getBindPoseBonePos(idx)));
		bone->SetRotation(convertToXmdQuat(*rig->getBindPoseBoneQuat(idx)));

		return bone;
	}

	XMD::XJoint* XMDFileTranslator::createJoint(XMD::XModel* xmd, MR::AnimRigDef* rig, int idx)
	{
		XMD::XJoint* joint = static_cast<XMD::XJoint*>(xmd->CreateNode(XMD::XFn::Joint));

		joint->SetName(rig->getBoneName(idx));
		joint->SetTranslation(convertToXmdVec3(*rig->getBindPoseBonePos(idx)));
		joint->SetRotation(convertToXmdQuat(*rig->getBindPoseBoneQuat(idx)));

		return joint;
	}

	XMD::XMesh* XMDFileTranslator::createMesh(XMD::XModel* xmd, FlverModel* model, int meshIdx)
	{
		XMD::XMesh* mesh = static_cast<XMD::XMesh*>(xmd->CreateNode(XMD::XFn::Mesh));

		char meshName[256];
		sprintf_s(meshName, "mesh[%d]Shape", meshIdx);

		XMD::XVector3Array points = getVertices(model, meshIdx);

		mesh->SetName(meshName);
		mesh->SetIsIntermediateObject(false);
		mesh->SetPoints(points);

		createSkin(xmd, mesh, model, meshIdx);
		createMeshBone(xmd, mesh, meshIdx);

		return mesh;
	}

	XMD::XPolygonMesh* XMDFileTranslator::createPolygonMesh(XMD::XModel* xmd, FlverModel* model, int meshIdx)
	{
		XMD::XPolygonMesh* mesh = static_cast<XMD::XPolygonMesh*>(xmd->CreateNode(XMD::XFn::PolygonMesh));

		char meshName[256];
		sprintf_s(meshName, "mesh[%d]Shape", meshIdx);

		XMD::XVector3Array points = getVertices(model, meshIdx);

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

		createSkin(xmd, mesh, model, meshIdx);
		createMeshBone(xmd, mesh, meshIdx);

		return mesh;
	}

	XMD::XBone* XMDFileTranslator::createMeshBone(XMD::XModel* xmd, XMD::XPolygonMesh* mesh, int meshIdx)
	{
		XMD::XBone* bone = static_cast<XMD::XBone*>(xmd->CreateNode(XMD::XFn::Bone));

		char meshName[256];
		sprintf_s(meshName, "mesh[%d]", meshIdx);

		bone->SetName(meshName);
		bone->AddInstance(xmd->CreateGeometryInstance(bone, mesh));
		bone->UpdateBindPose();

		return bone;
	}

	XMD::XBone* XMDFileTranslator::createMeshBone(XMD::XModel* xmd, XMD::XMesh* mesh, int meshIdx)
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
	XMD::XAnimCycle* XMDFileTranslator::createBindPoseAnimCycle(XMD::XModel* xmd, MR::AnimRigDef* rig)
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
	XMD::XAnimCycle* XMDFileTranslator::createAnimCycle(XMD::XModel* xmd, AnimObject* animObj, const char* takeName, int fps)
	{
		if (animObj->getHandle() == nullptr)
			return nullptr;

		XMD::XAnimCycle* animCycle = static_cast<XMD::XAnimCycle*>(xmd->CreateNode(XMD::XFn::AnimCycle));

		const int animLenFrames = RMath::timeToFrame(animObj->getAnimLenght(), fps);

		animCycle->SetName(takeName);
		animCycle->SetFrameRate(fps);
		animCycle->SetFrameTimes(0, animLenFrames, animLenFrames);

		const MR::RigToAnimMap* rigToAnimMap = animObj->getHandle()->getRigToAnimMap();

		//If it's present, we use the animToRigMap, otherwise we use the whole rig
		if (rigToAnimMap && (rigToAnimMap->getRigToAnimMapType() == MR::RigToAnimMap::AnimToRig))
		{
			const MR::AnimToRigTableMap* animToRigMap = (MR::AnimToRigTableMap*)rigToAnimMap->getRigToAnimMapData();

			for (uint32_t i = 0; i < animToRigMap->getNumAnimChannels(); i++)
			{
				const int channelID = animToRigMap->getAnimToRigMapEntry(i);

				//CharacterWorldSpaceTM is never animated since its a control bone added by morpheme on export. If a rigToAnimMap maps to it, then the map is wrong
				if (channelID == 0)
					g_appLog->panicMessage("Incorrect rigToAnimMap data. CharacterWorldSpaceTM should not be animated! (anim=%s)\n", animObj->getAnimName());

				XMD::XSampledKeys* sampleKeys = animCycle->AddSampledKeys(channelID);
				sampleKeys->SetSize(animLenFrames);

				for (size_t j = 0; j < animLenFrames; j++)
				{
					float time = RMath::frameToTime(j, fps);

					sampleKeys->TranslationKeys()[j] = getBoneTransformPosAtTime(animObj, time, channelID);
					sampleKeys->RotationKeys()[j] = getBoneTransformQuatAtTime(animObj, time, channelID);
					sampleKeys->ScaleKeys()[j] = getBoneTransformScaleAtTime(animObj, time, channelID);
				}
			}
		}
		else
		{
			const MR::AnimRigDef* animRigDef = animObj->getHandle()->getRig();

			//We start from 1 because bone 0 is added by morpheme on export, so it's not a part of the real skeleton
			for (uint32_t i = 1; i < animRigDef->getNumBones(); i++)
			{
				const int channelID = i;

				XMD::XSampledKeys* sampleKeys = animCycle->AddSampledKeys(channelID);
				sampleKeys->SetSize(animLenFrames);

				for (size_t j = 0; j < animLenFrames; j++)
				{
					float time = RMath::frameToTime(j, 30);

					sampleKeys->TranslationKeys()[j] = getBoneTransformPosAtTime(animObj, time, channelID);
					sampleKeys->RotationKeys()[j] = getBoneTransformQuatAtTime(animObj, time, channelID);
					sampleKeys->ScaleKeys()[j] = getBoneTransformScaleAtTime(animObj, time, channelID);
				}
			}
		}

		return animCycle;
	}

	XMD::XAnimationTake* XMDFileTranslator::createBindPoseAnimTake(XMD::XModel* xmd, MR::AnimRigDef* rig)
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

		for (size_t i = 1; i < rig->getNumBones(); i++)
		{
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

	XMD::XAnimationTake* XMDFileTranslator::createAnimTake(XMD::XModel* xmd, AnimObject* animObj, const char* takeName)
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

		for (size_t i = 1; i < rig->getNumBones(); i++)
			createAnimatedNode(animTake, boneList, animObj, i, fps);

		return animTake;
	}

	XMD::XAnimatedNode* XMDFileTranslator::createAnimatedNode(XMD::XAnimationTake* animTake, XMD::XBoneList boneList, AnimObject* animObj, int boneId, int fps)
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

	XMD::XObjectSet* XMDFileTranslator::createClusterObjSet(XMD::XModel* xmd, XMD::XSkinCluster* skin, XMD::XPolygonMesh* mesh, int meshIdx)
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

	XMD::XObjectSet* XMDFileTranslator::createClusterObjSet(XMD::XModel* xmd, XMD::XSkinCluster* skin, XMD::XMesh* mesh, int meshIdx)
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

	bool XMDFileTranslator::exportModel(Character* character)
	{
		bool status = true;

		g_appLog->debugMessage(MsgLevel_Info, "Exporting model to XMD for character %ws\n", character->getCharacterName().c_str());

		FlverModel* model = character->getCharacterModelCtrl()->getModel();
		MR::AnimRigDef* rig = character->getRig(0);

		XMD::XModel* modelXmd = createModel(rig, model, RString::toNarrow(model->getFileOrigin()).c_str(), true);
		createBindPoseAnimCycle(modelXmd, rig);

		if (modelXmd->Save(RString::toNarrow(character->getCharacterName()) + ".xmd") != XMD::XFileError::Success)
			status = false;

		delete modelXmd;

		return status;
	}

	bool XMDFileTranslator::exportAnimation(Character* character, std::wstring path, int animSetIdx, int animIdx, int fps, bool includeMeshes)
	{
		bool status = true;

		MorphemeCharacterDef* characterDef = character->getMorphemeCharacterDef();
		AnimObject* anim = characterDef->getAnimation(animSetIdx, animIdx);

		int animId = anim->getAnimID();

		if (!anim->isLoaded())
			anim = characterDef->getAnimation(0, 0);

		if (!anim->isLoaded())
			return false;

		MR::AnimRigDef* rig = character->getRig(0);

		std::string animName = RString::toNarrow(path) + RString::removeExtension(characterDef->getAnimFileLookUp()->getSourceFilename(animId)) + ".xmd";

		g_appLog->debugMessage(MsgLevel_Info, "\tExporting animation \"%s\" to XMD (%ws)\n", animName.c_str(), character->getCharacterName().c_str());

		XMD::XModel* xmd = createModel(rig, character->getCharacterModelCtrl()->getModel(), characterDef->getAnimFileLookUp()->getFilename(animId), includeMeshes);
		XMD::XAnimCycle* animTake = createAnimCycle(xmd, anim, characterDef->getAnimFileLookUp()->getTakeName(animId), fps);

		if ((animTake == nullptr) || (xmd->Save(animName) != XMD::XFileError::Success))
			status = false;

		delete xmd;

		return status;
	}
}