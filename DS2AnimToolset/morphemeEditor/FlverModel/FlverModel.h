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
		DirectX::VertexPositionColor m_pos;
		Vector3 m_normal;
		float bone_weights[4];
		int bone_indices[4];

		SkinnedVertex() {}
		SkinnedVertex(Vector3 pos, Vector3 normal, float* weights, int* bone_indices);
	};

	struct Settings
	{
		bool m_xray = false;
		bool m_drawDummyPolygons = false;
		bool m_sceneExplorer = false;
		int m_selectedBone = -1;
		int m_selectedDummy = -1;
	} m_settings;

	bool m_loaded = false;

	Vector3 m_position = Vector3::Zero;
	Vector3 m_focusPoint = Vector3::Zero;

	FLVER2* m_flver = nullptr;
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

	std::vector<FbxVector4> GetModelMeshVertices(int idx, bool flip);
	std::vector<FbxVector4> GetModelMeshNormals(int idx, bool flip);
	std::vector<FbxVector4> GetModelMeshBoneWeights(int idx);
	void GetModelMeshBoneIndices(std::vector<int*>& buffer, int idx);

	void GetModelData();
	void UpdateModel();
	int GetBoneIndexFromName(const char* name);

	void Animate(MR::AnimationSourceHandle* animHandle, std::vector<int> flverToMorphemeBoneMap);
};