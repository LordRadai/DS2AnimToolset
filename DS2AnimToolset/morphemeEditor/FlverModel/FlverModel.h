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
		float boneWeights[4];
		int boneIndices[4];

		SkinnedVertex() {}
		SkinnedVertex(Vector3 pos, Vector3 normal, float* weights, int* bone_indices);
	};

	struct Settings
	{
		DisplayMode displayMode = kDispNormal;
		bool drawDummyPolygons = false;
		bool drawBones = false;
		bool drawBoundingBox = false;
		bool highlight = false;
		int selectedBone = -1;
		int selectedDummy = -1;
	};

	static FlverModel* createFromBnd(std::wstring path, MR::AnimRigDef* rig);

	void destroy();

	std::string getModelName() const { return this->m_name; }
	std::wstring getFileOrigin() const { return this->m_fileOrigin; }
	Matrix getPosition() const { return this->m_position; }
	void setPosition(Matrix position) { this->m_position = position; }

	float getScale() const { return this->m_scale; }
	void setScale(float scale) { this->m_scale = scale; }

	Matrix getWorldMatrix() const { return this->m_position * Matrix::CreateScale(this->m_scale); }

	int getNumMeshes() const { return this->m_verts.size(); }
	int getNumVerticesInMesh(int meshIdx) const { return this->m_verts[meshIdx].size(); }

	std::vector<int> getFlverToMorphemeBoneMap() const { return this->m_flverToMorphemeBoneMap; }
	std::vector<int> getMorphemeToFlverBoneMap() const { return this->m_morphemeToFlverBoneMap; }

	Settings* getSettings() { return &this->m_settings; }

	SkinnedVertex* getVertex(int meshIdx, int idx);
	SkinnedVertex* getVertexBindPose(int meshIdx, int idx);

	void update(float dt);
	void draw(RenderManager* renderManager);

	int getMorphemeBoneIdByFlverBoneId(int idx);
	int getFlverBoneIndexByMorphemeBoneIndex(int idx);

	Matrix getDummyPolygonTransform(int id);

	void animate(MR::AnimationSourceHandle* animHandle);

	//FLVER functions

	int getNumFlverMeshes() const { return this->m_flver->header.meshCount; }
	int getNumFlverDummies() const { return this->m_flver->header.dummyCount; }
	int getNumFlverBones() const { return this->m_flver->header.boneCount; }

	cfr::FLVER2::Mesh getFlverMesh(int idx) const { return this->m_flver->meshes[idx]; }
	cfr::FLVER2::Dummy getFlverDummy(int idx) const { return this->m_flver->dummies[idx]; }
	cfr::FLVER2::Bone getFlverBone(int idx) const { return this->m_flver->bones[idx]; }
	std::string getFlverBoneName(int idx);
	Matrix* getFlverBoneTransform(int idx);
	Matrix* getFlverBoneBindPose(int idx);

	Matrix* getFlverRootBoneTransform();
	Matrix* getFlverTrajectoryBoneTransform();

	Vector3 getBoundingBoxMin();
	Vector3 getBoundingBoxMax();

	int getFlverBoneIndexByName(const char* name);

	std::vector<Vector3> getFlverMeshVertices(int idx, bool flip);
	std::vector<Vector3> getFlverMeshNormals(int idx, bool flip);
	std::vector<Vector3> getFlverMeshTangents(int idx, bool flip);
	std::vector<Vector3> getFlverMeshBiTangents(int idx, bool flip);
	std::vector<Vector4> getFlverMeshBoneWeights(int idx);
	std::vector<std::vector<int>> getFlverMeshBoneIndices(int idx);

	//Morpheme functions

	MR::AnimRigDef* getRig() const { return this->m_nmRig; }
	int getNumMorphemeBones() const { return this->m_nmRig->getNumBones(); }
	std::string getMorphemeBoneName(int idx);
	Matrix* getMorphemeBoneTransform(int idx);
	Matrix* getMorphemeBoneBindPose(int idx);

	Matrix* getMorphemeRootBoneTransform();
	Matrix* getMorphemeTrajectoryBoneTransform();

	int getMorphemeBoneIndexByName(const char* name);

private:
	FlverModel();
	FlverModel(UMEM* umem, MR::AnimRigDef* rig);
	~FlverModel();

	void initModelData();

	std::wstring m_fileOrigin;
	Settings m_settings;

	bool m_loaded = false;
	std::string m_name;

	Matrix m_position = Matrix::Identity;
	Vector3 m_focusPoint = Vector3::Zero;

	cfr::FLVER2* m_flver;
	MR::AnimRigDef* m_nmRig = nullptr;
	std::vector<int> m_flverToMorphemeBoneMap;
	std::vector<int> m_morphemeToFlverBoneMap;
	std::vector<std::vector<SkinnedVertex>> m_verts;
	std::vector<std::vector<SkinnedVertex>> m_vertBindPose;
	std::vector<Matrix> m_boneTransforms;
	std::vector<Matrix> m_boneBindPose;
	std::vector<Matrix> m_morphemeBoneTransforms;
	std::vector<Matrix> m_morphemeBoneBindPose;
	std::vector<Matrix> m_dummyPolygons;
	float m_scale = 1.5f;

	void createFlverToMorphemeBoneMap();
	void createMorphemeToFlverBoneMap();
};