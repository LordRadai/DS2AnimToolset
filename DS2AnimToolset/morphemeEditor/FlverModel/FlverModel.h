#pragma once
#include <windows.h>
#include <io.h>
#include <d3d11.h>
#include <dxgi.h>
#include <string>
#include <vector>
#include <fbxsdk.h>

#include "SimpleMath.h"
#include "VertexTypes.h"

#include "fromloader/fromloader.h"
#include "MorphemeSystem/MorphemeSystem.h"
#include "AnimObject/AnimObject.h"
#include <PrimitiveBatch.h>

class RenderManager;

using namespace cfr;

enum DisplayMode
{
	kDispNormal,
	kDispXRay,
	kDispWireframe,
};

using namespace DirectX::SimpleMath;

class FlverModel
{
public:
	struct SkinnedVertex
	{
		DirectX::VertexPositionNormalColor vertexData;
		float boneWeights[4] = { 0, 0, 0, 0 };
		int boneIndices[4] = { -1, -1, -1, -1 };

		SkinnedVertex() {}
		SkinnedVertex(Vector3 pos, Vector3 normal, float* weights, int* bone_indices);
	};

	struct Settings
	{
		DisplayMode displayMode = kDispNormal;
		bool drawDummyPolygons = false;
		bool drawMeshes = true;
		bool drawBones = false;
		bool drawMorphemeBones = false;
		bool drawBoundingBox = false;
		bool highlight = false;
		bool enableRootMotion = true;
		int selectedBone = -1;
		int selectedDummy = -1;
	};

	static FlverModel* createFromBnd(std::wstring path, MR::AnimRigDef* rig);

	void update(float dt);
	void animate(AnimObject* anim);
	void draw(RenderManager* renderManager);
	void destroy();

	std::string getModelName() const { return this->m_name; }
	std::wstring getFileOrigin() const { return this->m_fileOrigin; }
	Matrix getPosition() const { return this->m_position; }
	void setPosition(Matrix position) { this->m_position = position; }

	float getScale() const { return this->m_scale; }
	void setScale(float scale) { this->m_scale = scale; }

	Matrix getWorldMatrix() const { return this->m_position * Matrix::CreateScale(this->m_scale); }

	int getNumMeshes() const { return this->m_meshVerticesTransforms.size(); }
	int getNumVerticesInMesh(int meshIdx) const { return this->m_meshVerticesTransforms[meshIdx].size(); }

	std::vector<int> getFlverToMorphemeBoneMap() const { return this->m_flverToMorphemeBoneMap; }
	std::vector<int> getMorphemeToFlverBoneMap() const { return this->m_morphemeToFlverBoneMap; }

	Settings* getSettings() { return &this->m_settings; }

	SkinnedVertex* getVertex(int meshIdx, int idx);
	SkinnedVertex* getVertexBindPose(int meshIdx, int idx);

	int getMorphemeBoneIdByFlverBoneId(int idx);
	int getFlverBoneIndexByMorphemeBoneIndex(int idx);

	Matrix getDummyPolygonTransform(int id);

	// FLVER functions

	int getNumFlverMeshes() const { return this->m_flver->header.meshCount; }
	int getNumFlverDummies() const { return this->m_flver->header.dummyCount; }
	int getNumFlverBones() const { return this->m_flver->header.boneCount; }

	cfr::FLVER2::Mesh getFlverMesh(int idx) const { return this->m_flver->meshes[idx]; }
	cfr::FLVER2::Dummy getFlverDummy(int idx) const { return this->m_flver->dummies[idx]; }
	cfr::FLVER2::Bone getFlverBone(int idx) const { return this->m_flver->bones[idx]; }
	std::string getFlverBoneName(int idx);

	Matrix getFlverBoneGlobalTransform(int idx);
	Matrix getFlverBoneBindPoseGlobalTransform(int idx);

	Matrix getFlverRootBoneGlobalTransform();
	Matrix getFlverTrajectoryBoneGlobalTransform();

	int getFlverTrajectoryBoneIndex();
	int getFlverRootBoneIndex();

	int getMorphemeTrajectoryBoneIndex();
	int getMorphemeRootBoneIndex();

	Vector3 getBoundingBoxMin();
	Vector3 getBoundingBoxMax();

	int getFlverBoneIndexByName(const char* name);

	std::vector<SkinnedVertex> getBindPoseSkinnedVertices(int idx);

	// Morpheme functions

	MR::AnimRigDef* getRig() const { return this->m_nmRig; }
	int getNumMorphemeBones() const { return this->m_nmRig->getNumBones(); }
	std::string getMorphemeBoneName(int idx);
	Matrix getMorphemeBoneGlobalTransform(int idx);
	Matrix getMorphemeBoneBindPoseGlobalTransform(int idx);

	Matrix getMorphemeRootBoneGlobalTransform();
	Matrix getMorphemeTrajectoryBoneGlobalTransform();

	Matrix getNmBoneRelativeTransform(int idx);
	Matrix getFlverBoneRelativeTransform(int idx);

	int getMorphemeBoneIndexByName(const char* name);

private:
	FlverModel() {}
	FlverModel(UMEM* umem, MR::AnimRigDef* rig);
	~FlverModel() {}

	/**
	 * \brief Validates the bone influence IDs. This function modifies the skinned vertex data to ensure that all inlfuences are valid for the morpheme rig.
	 * 
	 * \param skinnedVertex The input skinned vertex data to validate. This will get modified.
	 */
	void validateSkinnedVertexData(FlverModel::SkinnedVertex& skinnedVertex);

	/**
	 * \brief Finds the first bone with valid influence for the given bone ID.
	 * Needs to be revised, if I can figure out how the fucking game handles this.
	 * 
	 * \param boneID The bone ID to validate.
	 * \return The valid bone index, or -1 if no valid index is found.
	 */
	int findValidBoneIndex(int boneID);

	std::wstring m_fileOrigin = L"";
	Settings m_settings;

	bool m_loaded = false;
	std::string m_name;

	Matrix m_position = Matrix::Identity;

	Matrix m_trajectoryPos = Matrix::Identity;
	Matrix m_trajectoryLastPos = Matrix::Identity;

	Vector3 m_focusPoint = Vector3::Zero;

	cfr::FLVER2* m_flver = nullptr;
	MR::AnimRigDef* m_nmRig = nullptr;
	std::vector<int> m_flverToMorphemeBoneMap;
	std::vector<int> m_morphemeToFlverBoneMap;
	std::vector<std::vector<SkinnedVertex>> m_meshVerticesTransforms;
	std::vector<std::vector<SkinnedVertex>> m_meshVerticesBindPoseTransforms;
	std::vector<Matrix> m_flverBoneTransforms;
	std::vector<Matrix> m_flverBindPoseTransforms;
	std::vector<Matrix> m_flverInverseBindPoseTransforms;
	std::vector<Matrix> m_nmBoneTransforms;
	std::vector<Matrix> m_nmBindPoseTransforms;
	std::vector<Matrix> m_nmInverseBoneBindPoseTransforms;
	std::vector<Matrix> m_dummyPolygons;
	float m_scale = 1.5f;

	bool initialise();

	void createFlverToMorphemeBoneMap();
	void createMorphemeToFlverBoneMap();

	std::vector<Vector3> getFlverMeshVertices(int idx);
	std::vector<Vector3> getFlverMeshNormals(int idx);
	std::vector<Vector3> getFlverMeshTangents(int idx);
	std::vector<Vector3> getFlverMeshBiTangents(int idx);
	std::vector<Vector4> getFlverMeshBoneWeights(int idx);
	std::vector<std::vector<int>> getFlverMeshBoneIndices(int idx);
	void resetBoneTransforms();
	void computeAnimationTransforms(MR::AnimationSourceHandle* animHandle);
	void computeBoneRelativeTransforms(std::vector<Matrix>& out);
	void transformMesh(int meshIdx, const std::vector<Matrix>& boneRelativeTransforms);
	void transformVertex(int meshIdx, int vertexIndex, const std::vector<Matrix>& boneRelativeTransforms);

	void drawFlverBones(RenderManager* renderManager, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& prim);
	void drawMorphemeBones(RenderManager* renderManager, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& prim);
};