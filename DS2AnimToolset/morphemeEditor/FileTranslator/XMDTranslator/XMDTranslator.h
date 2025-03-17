#pragma once
#include "MorphemeSystem/XMDUtils/XMDUtils.h"
#include "FlverModel/FlverModel.h"
#include "AnimObject/AnimObject.h"
#include "Character/Character.h"
#include "morpheme/mrRig.h"
#include "../FileTranslatorBase.h"

namespace FT
{
	class XMDFileTranslator : public FileTranslatorBase
	{
	public:
		XMDFileTranslator() {}
		~XMDFileTranslator() {}

		bool exportModel(Character* character);
		bool exportAnimation(Character* character, std::wstring path, int animSetIdx, int animIdx, int fps, bool includeMeshes);

	private:
		XMD::XModel* createModel(MR::AnimRigDef* rig, FlverModel* model, const char* originalFile, bool includeMeshes);

		XMD::XSkinCluster* createSkin(XMD::XModel* xmd, XMD::XPolygonMesh* mesh, FlverModel* model, int meshIdx);
		XMD::XSkinCluster* createSkin(XMD::XModel* xmd, XMD::XMesh* mesh, FlverModel* model, int meshIdx);

		XM2::vector<XMD::XSkinnedVertex> createSkinnedVertexList(XMD::XBoneList bones, XMD::XSkinCluster* skin, FlverModel* model, int meshIdx);

		XMD::XBone* createBone(XMD::XModel* xmd, MR::AnimRigDef* rig, int idx);
		XMD::XJoint* createJoint(XMD::XModel* xmd, MR::AnimRigDef* rig, int idx);

		XMD::XMesh* createMesh(XMD::XModel* xmd, FlverModel* model, int meshIdx);
		XMD::XPolygonMesh* createPolygonMesh(XMD::XModel* xmd, FlverModel* model, int meshIdx);

		XMD::XBone* createMeshBone(XMD::XModel* xmd, XMD::XPolygonMesh* mesh, int meshIdx);
		XMD::XBone* createMeshBone(XMD::XModel* xmd, XMD::XMesh* mesh, int meshIdx);

		XMD::XAnimCycle* createBindPoseAnimCycle(XMD::XModel* xmd, MR::AnimRigDef* rig);
		XMD::XAnimCycle* createAnimCycle(XMD::XModel* xmd, AnimObject* animObj, const char* takeName, int fps);

		XMD::XAnimationTake* createBindPoseAnimTake(XMD::XModel* xmd, MR::AnimRigDef* rig);
		XMD::XAnimationTake* createAnimTake(XMD::XModel* xmd, AnimObject* animObj, const char* takeName);

		XMD::XAnimatedNode* createAnimatedNode(XMD::XAnimationTake* animTake, XMD::XBoneList boneList, AnimObject* animObj, int boneId, int fps);

		XMD::XObjectSet* createClusterObjSet(XMD::XModel* xmd, XMD::XSkinCluster* skin, XMD::XPolygonMesh* mesh, int meshIdx);
		XMD::XObjectSet* createClusterObjSet(XMD::XModel* xmd, XMD::XSkinCluster* skin, XMD::XMesh* mesh, int meshIdx);
	};
}