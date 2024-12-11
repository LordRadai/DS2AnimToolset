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
		bool drawBoundingBox = false;
		bool highlight = false;
		int selectedBone = -1;
		int selectedDummy = -1;
	};

	static FlverModel* createFromBnd(std::wstring path, MR::AnimRigDef* rig);

	void update(float dt);
	void animate(MR::AnimationSourceHandle* animHandle);
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
	Matrix* getFlverBoneGlobalTransform(int idx);
	Matrix* getFlverBoneBindPoseGlobalTransform(int idx);

	Matrix* getFlverRootBoneGlobalTransform();
	Matrix* getFlverTrajectoryBoneGlobalTransform();

	Vector3 getBoundingBoxMin();
	Vector3 getBoundingBoxMax();

	int getFlverBoneIndexByName(const char* name);

	std::vector<SkinnedVertex> getBindPoseSkinnedVertices(int idx);
	void validateSkinnedVertexData(FlverModel::SkinnedVertex& skinnedVertex);

	// Morpheme functions

	MR::AnimRigDef* getRig() const { return this->m_nmRig; }
	int getNumMorphemeBones() const { return this->m_nmRig->getNumBones(); }
	std::string getMorphemeBoneName(int idx);
	Matrix* getMorphemeBoneGlobalTransform(int idx);
	Matrix* getMorphemeBoneBindPoseGlobalTransform(int idx);

	Matrix* getMorphemeRootBoneGlobalTransform();
	Matrix* getMorphemeTrajectoryBoneGlobalTransform();

	int getMorphemeBoneIndexByName(const char* name);

private:
	FlverModel() {}
	FlverModel(UMEM* umem, MR::AnimRigDef* rig);
	~FlverModel() {}

	std::wstring m_fileOrigin = L"";
	Settings m_settings;

	bool m_loaded = false;
	std::string m_name;

	Matrix m_position = Matrix::Identity;
	Vector3 m_focusPoint = Vector3::Zero;

	cfr::FLVER2* m_flver = nullptr;
	MR::AnimRigDef* m_nmRig = nullptr;
	std::vector<int> m_flverToMorphemeBoneMap;
	std::vector<int> m_morphemeToFlverBoneMap;
	std::vector<std::vector<SkinnedVertex>> m_meshVerticesTransforms;
	std::vector<std::vector<SkinnedVertex>> m_meshVerticesBindPoseTransforms;
	std::vector<Matrix> m_boneTransforms;
	std::vector<Matrix> m_boneBindPoseTransforms;
	std::vector<Matrix> m_boneInverseBindPoseTransforms;
	std::vector<Matrix> m_morphemeBoneTransforms;
	std::vector<Matrix> m_morphemeBoneBindPoseTransforms;
	std::vector<Matrix> m_morphemeInverseBoneBindPoseTransforms;
	std::vector<Matrix> m_dummyPolygons;
	float m_scale = 1.5f;

	bool initialise();

	void createFlverToMorphemeBoneMap();
	void createMorphemeToFlverBoneMap();

	std::vector<Vector3> getFlverMeshVertices(int idx, bool flip);
	std::vector<Vector3> getFlverMeshNormals(int idx, bool flip);
	std::vector<Vector3> getFlverMeshTangents(int idx, bool flip);
	std::vector<Vector3> getFlverMeshBiTangents(int idx, bool flip);
	std::vector<Vector4> getFlverMeshBoneWeights(int idx);
	std::vector<std::vector<int>> getFlverMeshBoneIndices(int idx);
};