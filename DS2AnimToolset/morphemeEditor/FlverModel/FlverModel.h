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

using namespace cfr;

class FlverModel
{
public:
	struct SkinnedVertex
	{
		DirectX::VertexPositionNormalColor m_pos;
		float bone_weights[4];
		int bone_indices[4];

		SkinnedVertex() {}
		SkinnedVertex(Vector3 pos, Vector3 normal, float* weights, int* bone_indices);
	};

	struct Settings
	{
		bool m_wireframe = false;
		bool m_drawDummyPolygons = false;
		int m_selectedBone = -1;
		int m_selectedDummy = -1;
	} m_settings;

	bool m_loaded = false;
	std::string m_name;

	Matrix m_position = Matrix::Identity;
	Vector3 m_focusPoint = Vector3::Zero;

	FLVER2* m_flver = nullptr;
	std::vector<int> m_flverToMorphemeBoneMap;
	std::vector<std::vector<SkinnedVertex>> m_verts;
	std::vector<std::vector<SkinnedVertex>> m_vertBindPose;
	std::vector<Matrix> m_boneTransforms;
	std::vector<Matrix> m_boneBindPose;
	std::vector<Matrix> m_morphemeBoneTransforms;
	std::vector<Matrix> m_morphemeBoneBindPose;
	std::vector<Matrix> m_dummyPolygons;
	float m_scale = 1.5;

	FlverModel();
	FlverModel(UMEM* umem);
	~FlverModel();

	static FlverModel* CreateFromBnd(std::wstring path);

	std::vector<FbxVector4> GetModelMeshVertices(int idx, bool flip);
	std::vector<FbxVector4> GetModelMeshNormals(int idx, bool flip);
	std::vector<FbxVector4> GetModelMeshBoneWeights(int idx);
	void GetModelMeshBoneIndices(std::vector<int*>& buffer, int idx);

	void GetModelData();
	void UpdateModel();
	int GetBoneIndexFromName(const char* name);
	std::string GetModelName();

	std::vector<int> GetFlverToMorphemeBoneMap();
	void CreateFlverToMorphemeBoneMap(MR::AnimRigDef* pMorphemeRig);
	int GetFlverBoneIndexByMorphemeBoneIndex(int idx);

	Matrix GetDummyPolygonTransform(int id);

	void Animate(MR::AnimationSourceHandle* animHandle);
};