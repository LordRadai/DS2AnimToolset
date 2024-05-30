#include "FlverModel.h"
#include "../framework.h"
#include "../extern.h"
#include "utils/MorphemeToDirectX.h"

FlverModel::SkinnedVertex::SkinnedVertex(Vector3 pos, Vector3 normal, float* weights, int* bone_indices)
{
	this->m_pos = DirectX::VertexPositionColor(pos, Vector4(0.7f, 0.7f, 0.7f, 1.f));
	this->m_normal = normal;

	for (size_t i = 0; i < 4; i++)
	{
		this->bone_indices[i] = bone_indices[i];
		this->bone_weights[i] = weights[i];
	}
}

FlverModel::FlverModel()
{
	this->m_position = DirectX::SimpleMath::Vector3::Zero;
}

FlverModel::FlverModel(UMEM* umem)
{
	this->m_loaded = false;
	this->m_verts.clear();

	if (this->m_loaded)
		delete this->m_flver;

	this->m_position = DirectX::SimpleMath::Vector3::Zero;
	this->m_flver = new FLVER2(umem);

	float focus_y = (this->m_flver->header.boundingBoxMax.y + this->m_flver->header.boundingBoxMin.y) / 2;

	this->m_focusPoint = this->m_position + DirectX::SimpleMath::Vector3(0, focus_y, 0);

	this->GetModelData();

	this->m_loaded = true;
}

FlverModel::~FlverModel()
{
}

//Gets the vertices for the FLVER mesh at index idx
std::vector<FbxVector4> FlverModel::GetModelMeshVertices(int idx, bool flip)
{
	std::vector<FbxVector4> vertices;

	if (m_flver == nullptr)
		return vertices;

	if (idx > m_flver->header.meshCount)
		return vertices;

	cfr::FLVER2::Mesh* mesh = &m_flver->meshes[idx];

	int uvCount = 0;
	int colorCount = 0;
	int tanCount = 0;

	m_flver->getVertexData(idx, &uvCount, &colorCount, &tanCount);

	uint64_t lowest_flags = LLONG_MAX;
	cfr::FLVER2::Faceset* facesetp = nullptr;

	for (int mfsi = 0; mfsi < mesh->header.facesetCount; mfsi++)
	{
		int fsindex = mesh->facesetIndices[mfsi];
		if (this->m_flver->facesets[fsindex].header.flags < lowest_flags)
		{
			facesetp = &this->m_flver->facesets[fsindex];
			lowest_flags = facesetp->header.flags;
		}
	}

	facesetp->triangulate();

	if (facesetp != nullptr)
	{
		for (int j = 0; j < facesetp->triCount; j++)
		{
			int vertexIndex = facesetp->triList[j];

			float x = mesh->vertexData->positions[(vertexIndex * 3) + 0];
			float y = mesh->vertexData->positions[(vertexIndex * 3) + 2];
			float z = mesh->vertexData->positions[(vertexIndex * 3) + 1];

			Vector3 pos(x, y, z);

			pos = Vector3::Transform(pos, Matrix::CreateRotationX(DirectX::XM_PIDIV2) * Matrix::CreateRotationY(DirectX::XM_PI));

			if (!flip)
				vertices.push_back(FbxVector4(pos.x, pos.y, pos.z));
			else
				vertices.push_back(FbxVector4(pos.x, pos.y, -pos.z));
		}
	}

	return vertices;
}

//Gets the normals for the FLVER mesh at index idx
std::vector<FbxVector4> FlverModel::GetModelMeshNormals(int idx, bool flip)
{
	std::vector<FbxVector4> normals;

	if (m_flver == nullptr)
		return normals;

	if (idx > m_flver->header.meshCount)
		return normals;

	cfr::FLVER2::Mesh* mesh = &m_flver->meshes[idx];

	int uvCount = 0;
	int colorCount = 0;
	int tanCount = 0;

	m_flver->getVertexData(idx, &uvCount, &colorCount, &tanCount);

	uint64_t lowest_flags = LLONG_MAX;
	cfr::FLVER2::Faceset* facesetp = nullptr;

	for (int mfsi = 0; mfsi < mesh->header.facesetCount; mfsi++)
	{
		int fsindex = mesh->facesetIndices[mfsi];
		if (this->m_flver->facesets[fsindex].header.flags < lowest_flags)
		{
			facesetp = &this->m_flver->facesets[fsindex];
			lowest_flags = facesetp->header.flags;
		}
	}

	facesetp->triangulate();

	if (facesetp != nullptr)
	{
		for (int j = 0; j < facesetp->triCount; j++)
		{
			int vertexIndex = facesetp->triList[j];

			float x = mesh->vertexData->normals[(vertexIndex * 3) + 0];
			float y = mesh->vertexData->normals[(vertexIndex * 3) + 2];
			float z = mesh->vertexData->normals[(vertexIndex * 3) + 1];

			if (!flip)
				normals.push_back(FbxVector4(x, y, z));
			else
				normals.push_back(FbxVector4(x, y, -z));
		}
	}

	return normals;
}

//Gets the bone weights for the FLVER mesh at index idx
std::vector<FbxVector4> FlverModel::GetModelMeshBoneWeights(int idx)
{
	std::vector<FbxVector4> weights;

	if (m_flver == nullptr)
		return weights;

	if (idx > m_flver->header.meshCount)
		return weights;

	cfr::FLVER2::Mesh* mesh = &m_flver->meshes[idx];

	int uvCount = 0;
	int colorCount = 0;
	int tanCount = 0;

	m_flver->getVertexData(idx, &uvCount, &colorCount, &tanCount);

	uint64_t lowest_flags = LLONG_MAX;
	cfr::FLVER2::Faceset* facesetp = nullptr;

	for (int mfsi = 0; mfsi < mesh->header.facesetCount; mfsi++)
	{
		int fsindex = mesh->facesetIndices[mfsi];
		if (this->m_flver->facesets[fsindex].header.flags < lowest_flags)
		{
			facesetp = &this->m_flver->facesets[fsindex];
			lowest_flags = facesetp->header.flags;
		}
	}

	facesetp->triangulate();

	if (facesetp != nullptr)
	{
		for (size_t j = 0; j < facesetp->triCount; j++)
		{
			int vertexIndex = facesetp->triList[j];

			float x = mesh->vertexData->bone_weights[(vertexIndex * 4) + 0];
			float y = mesh->vertexData->bone_weights[(vertexIndex * 4) + 1];
			float z = mesh->vertexData->bone_weights[(vertexIndex * 4) + 2];
			float w = mesh->vertexData->bone_weights[(vertexIndex * 4) + 3];

			weights.push_back(FbxVector4(x, y, z, w));
		}
	}

	return weights;
}

//Gets the bone indices for the FLVER mesh at index idx
void FlverModel::GetModelMeshBoneIndices(std::vector<int*>& buffer, int idx)
{
	if (m_flver == nullptr)
		return;

	if (idx > m_flver->header.meshCount)
		return;

	cfr::FLVER2::Mesh* mesh = &m_flver->meshes[idx];

	int uvCount = 0;
	int colorCount = 0;
	int tanCount = 0;

	m_flver->getVertexData(idx, &uvCount, &colorCount, &tanCount);

	uint64_t lowest_flags = LLONG_MAX;
	cfr::FLVER2::Faceset* facesetp = nullptr;

	for (int mfsi = 0; mfsi < mesh->header.facesetCount; mfsi++)
	{
		int fsindex = mesh->facesetIndices[mfsi];
		if (this->m_flver->facesets[fsindex].header.flags < lowest_flags)
		{
			facesetp = &this->m_flver->facesets[fsindex];
			lowest_flags = facesetp->header.flags;
		}
	}

	facesetp->triangulate();

	if (facesetp != nullptr)
	{
		for (size_t j = 0; j < facesetp->triCount; j++)
		{
			int vertexIndex = facesetp->triList[j];
			int* indices = new int[4];

			indices[0] = mesh->boneIndices[mesh->vertexData->bone_indices[(vertexIndex * 4) + 0]];
			indices[1] = mesh->boneIndices[mesh->vertexData->bone_indices[(vertexIndex * 4) + 1]];
			indices[2] = mesh->boneIndices[mesh->vertexData->bone_indices[(vertexIndex * 4) + 2]];
			indices[3] = mesh->boneIndices[mesh->vertexData->bone_indices[(vertexIndex * 4) + 3]];

			buffer.push_back(indices);
		}
	}

	return;
}

Matrix GetNmTrajectoryTransform(MR::AnimationSourceHandle* animHandle)
{
	NMP::Vector3 pos;
	NMP::Quat rot;

	animHandle->getTrajectory(rot, pos);

	return NMDX::GetWorldMatrix(rot, pos);
}

Matrix GetNmBoneTranform(MR::AnimationSourceHandle* animHandle, int channelId)
{
	return NMDX::GetWorldMatrix(animHandle->getChannelData()[channelId].m_quat, animHandle->getChannelData()[channelId].m_pos);
}

Matrix ComputeNmBoneGlobalTransform(MR::AnimationSourceHandle* animHandle, int channelId)
{
	const MR::AnimRigDef* rig = animHandle->getRig();
	DirectX::XMMATRIX boneLocalTransform = GetNmBoneTranform(animHandle, channelId);

	int parentIdx = rig->getParentBoneIndex(channelId);

	while (parentIdx != -1)
	{
		boneLocalTransform *= GetNmBoneTranform(animHandle, parentIdx);

		parentIdx = rig->getParentBoneIndex(parentIdx);
	}

	boneLocalTransform *= Matrix::CreateRotationX(-DirectX::XM_PIDIV2);

	return boneLocalTransform;
}

Matrix ComputeNmBoneBindPoseGlobalTransform(const MR::AnimRigDef* rig, int channelId)
{
	DirectX::XMMATRIX boneLocalTransform = NMDX::GetWorldMatrix(*rig->getBindPoseBoneQuat(channelId), *rig->getBindPoseBonePos(channelId));
	int parentIdx = rig->getParentBoneIndex(channelId);

	while (parentIdx != -1)
	{
		boneLocalTransform *= NMDX::GetWorldMatrix(*rig->getBindPoseBoneQuat(parentIdx), *rig->getBindPoseBonePos(parentIdx));

		parentIdx = rig->getParentBoneIndex(parentIdx);
	}

	boneLocalTransform *= Matrix::CreateRotationX(-DirectX::XM_PIDIV2);

	return boneLocalTransform;
}

Matrix GetFlverBoneTransform(cfr::FLVER2* flver, int bone_id)
{
	Matrix transform = Matrix::CreateScale(flver->bones[bone_id].scale.x, flver->bones[bone_id].scale.y, flver->bones[bone_id].scale.z);
	transform *= Matrix::CreateRotationX(flver->bones[bone_id].rot.x);
	transform *= Matrix::CreateRotationZ(flver->bones[bone_id].rot.z);
	transform *= Matrix::CreateRotationY(flver->bones[bone_id].rot.y);
	transform *= Matrix::CreateTranslation(flver->bones[bone_id].translation.x, flver->bones[bone_id].translation.y, flver->bones[bone_id].translation.z);

	return transform;
}

Matrix ComputeFlvBoneGlobalTransform(FLVER2* flv, int channelId)
{
	Matrix localTransform = GetFlverBoneTransform(flv, channelId);

	int parentIdx = flv->bones[channelId].parentIndex;

	while (parentIdx != -1)
	{
		Matrix parentBoneTransform = GetFlverBoneTransform(flv, parentIdx);

		localTransform *= parentBoneTransform;

		parentIdx = flv->bones[parentIdx].parentIndex;
	}

	localTransform *= Matrix::CreateRotationY(DirectX::XM_PI);

	return localTransform;
}

Matrix ApplyTransformToFlverBone(std::vector<Matrix> transforms, FLVER2* flv, int channelId)
{
	Matrix localTransform = transforms[channelId] * GetFlverBoneTransform(flv, channelId);

	int parentIdx = flv->bones[channelId].parentIndex;

	while (parentIdx != -1)
	{
		Matrix parentBoneTransform = transforms[parentIdx] * GetFlverBoneTransform(flv, parentIdx);

		localTransform *= parentBoneTransform;

		parentIdx = flv->bones[parentIdx].parentIndex;
	}

	localTransform *= Matrix::CreateRotationY(DirectX::XM_PI);

	return localTransform;
}

std::vector<Matrix> ComputeGlobalFlverRigTransform(FLVER2* flver)
{
	std::vector<Matrix> boneTransforms;
	boneTransforms.reserve(flver->header.boneCount);

	for (size_t i = 0; i < flver->header.boneCount; i++)
		boneTransforms.push_back(ComputeFlvBoneGlobalTransform(flver, i));

	return boneTransforms;
}

//Gets all the model vertices for all the meshes and stores them into m_verts
void FlverModel::GetModelData()
{
	this->m_verts.clear();
	this->m_boneTransforms.clear();
	this->m_vertBindPose.clear();
	this->m_boneBindPose.clear();

	if (m_flver == nullptr)
		return;

	this->m_boneBindPose = ComputeGlobalFlverRigTransform(this->m_flver);
	this->m_boneTransforms = this->m_boneBindPose;

	this->m_verts.reserve(m_flver->header.meshCount);
	this->m_vertBindPose.reserve(m_flver->header.meshCount);

	for (int i = 0; i < m_flver->header.meshCount; i++)
	{
		cfr::FLVER2::Mesh* mesh = &m_flver->meshes[i];

		int vertexCount = 0;

		for (int vbi = 0; vbi < mesh->header.vertexBufferCount; vbi++)
		{
			int vb_index = m_flver->meshes[i].vertexBufferIndices[vbi];
			vertexCount += m_flver->vertexBuffers[vb_index].header.vertexCount;
		}

		int uvCount = 0;
		int colorCount = 0;
		int tanCount = 0;

		m_flver->getVertexData(i, &uvCount, &colorCount, &tanCount);

		uint64_t lowest_flags = LLONG_MAX;
		cfr::FLVER2::Faceset* facesetp = nullptr;

		for (int mfsi = 0; mfsi < mesh->header.facesetCount; mfsi++)
		{
			int fsindex = mesh->facesetIndices[mfsi];
			if (this->m_flver->facesets[fsindex].header.flags < lowest_flags)
			{
				facesetp = &this->m_flver->facesets[fsindex];
				lowest_flags = facesetp->header.flags;
			}
		}

		facesetp->triangulate();

		std::vector<Vector3> meshVertices;
		std::vector<SkinnedVertex> meshSkinnedVertices;

		if (facesetp != nullptr)
		{
			meshVertices.reserve(facesetp->triCount);
			meshSkinnedVertices.reserve(facesetp->triCount);

			for (int j = 0; j < facesetp->triCount; j++)
			{
				int vertexIndex = facesetp->triList[j];

				float weights[4];

				weights[0] = mesh->vertexData->bone_weights[(vertexIndex * 4) + 0];
				weights[1] = mesh->vertexData->bone_weights[(vertexIndex * 4) + 1];
				weights[2] = mesh->vertexData->bone_weights[(vertexIndex * 4) + 2];
				weights[3] = mesh->vertexData->bone_weights[(vertexIndex * 4) + 3];

				int indices[4];
			
				indices[0] = mesh->boneIndices[mesh->vertexData->bone_indices[(vertexIndex * 4) + 0]];
				indices[1] = mesh->boneIndices[mesh->vertexData->bone_indices[(vertexIndex * 4) + 1]];
				indices[2] = mesh->boneIndices[mesh->vertexData->bone_indices[(vertexIndex * 4) + 2]];
				indices[3] = mesh->boneIndices[mesh->vertexData->bone_indices[(vertexIndex * 4) + 3]];

				float x = mesh->vertexData->positions[(vertexIndex * 3) + 0];
				float y = mesh->vertexData->positions[(vertexIndex * 3) + 2];
				float z = mesh->vertexData->positions[(vertexIndex * 3) + 1];

				Vector3 pos = DirectX::SimpleMath::Vector3(x, y, z);

				float norm_x = mesh->vertexData->positions[(vertexIndex * 3) + 0];
				float norm_y = mesh->vertexData->positions[(vertexIndex * 3) + 2];
				float norm_z = mesh->vertexData->positions[(vertexIndex * 3) + 1];

				Vector3 normal(norm_x, norm_y, norm_z);

				meshSkinnedVertices.push_back(SkinnedVertex(pos, normal, weights, indices));
				meshVertices.push_back(Vector3::Zero);
			}
		}

		this->m_vertBindPose.push_back(meshSkinnedVertices);
	}

	this->m_verts = this->m_vertBindPose;
}

void FlverModel::UpdateModel()
{
	if (m_flver == nullptr)
		return;

	DirectX::SimpleMath::Vector4 color = DirectX::SimpleMath::Vector4(0.7f, 0.7f, 0.7f, 1.f);

	if (this->m_settings.m_xray)
		color = DirectX::SimpleMath::Vector4(0.7f, 0.7f, 0.7f, 0.f);

	for (int i = 0; i < this->m_vertBindPose.size(); i++)
		for (size_t j = 0; j < this->m_vertBindPose[i].size(); j++)
			m_vertBindPose[i][j].m_pos.color = color;

	this->m_focusPoint = Vector3::Transform(this->m_position, Matrix::CreateScale(this->m_scale));

	this->m_dummyPolygons.clear();
	this->m_dummyPolygons.reserve(this->m_flver->header.dummyCount);
	for (size_t i = 0; i < this->m_flver->header.dummyCount; i++)
	{
		Vector3 dummyPos(this->m_flver->dummies[i].position.x, this->m_flver->dummies[i].position.y, this->m_flver->dummies[i].position.z);
		Vector3 dummyUp(this->m_flver->dummies[i].upward.x, this->m_flver->dummies[i].upward.y, this->m_flver->dummies[i].upward.z);
		Vector3 dummyForward(this->m_flver->dummies[i].forward.x, this->m_flver->dummies[i].forward.y, this->m_flver->dummies[i].forward.z);

		Matrix dummyLocalTransform = Matrix::CreateWorld(dummyPos, dummyUp, dummyForward);

		this->m_dummyPolygons.push_back(dummyLocalTransform * this->m_boneTransforms[this->m_flver->dummies[i].dummyBoneIndex]);
	}
}

int FlverModel::GetBoneIndexFromName(const char* name)
{
	for (size_t i = 0; i < this->m_flver->header.boneCount; i++)
	{
		if (strcmp(RString::ToNarrow(this->m_flver->bones[i].name).c_str(), name) == 0)
			return i;
	}

	return -1;
}

std::vector<Matrix> ComputeGlobalTransforms(std::vector<Matrix> relativeTransforms, FLVER2* flv)
{
	std::vector<Matrix> globalTransformedPos; 
	globalTransformedPos.reserve(flv->header.boneCount);

	for (size_t i = 0; i < flv->header.boneCount; i++)
		globalTransformedPos.push_back(ApplyTransformToFlverBone(relativeTransforms, flv, i));

	return globalTransformedPos;
}

Matrix GetNmRelativeBindPose(const MR::AnimRigDef* rig, int idx)
{
	Matrix transform = NMDX::GetWorldMatrix(*rig->getBindPoseBoneQuat(idx), *rig->getBindPoseBonePos(idx));
	transform *= Matrix::CreateRotationX(DirectX::XM_PIDIV2);
	transform *= Matrix::CreateReflection(Plane(Vector3::Up));

	return transform;
}

Matrix GetNmRelativeTransform(MR::AnimationSourceHandle* animHandle, int idx)
{
	Matrix transform = NMDX::GetWorldMatrix(animHandle->getChannelData()[idx].m_quat, animHandle->getChannelData()[idx].m_pos);
	transform *= Matrix::CreateRotationX(DirectX::XM_PIDIV2);
	transform *= Matrix::CreateReflection(Plane(Vector3::Up));

	return transform;
}

void ApplyTransform(std::vector<Matrix>& buffer, FLVER2* flv, std::vector<Matrix> bindPose, Matrix transform, int boneID)
{
	//Transform the bone
	buffer[boneID] = bindPose[boneID] * transform;

	//Transform all sibling of the current bone
	int siblingIndex = flv->bones[boneID].nextSiblingIndex;

	while (siblingIndex != -1)
	{
		buffer[siblingIndex] = bindPose[siblingIndex] * transform;
		siblingIndex = flv->bones[siblingIndex].nextSiblingIndex;
	}

	//Transforms all the children of the current bone
	int childIndex = flv->bones[boneID].childIndex;

	if (childIndex != -1)
	{
		buffer[childIndex] = bindPose[childIndex] * transform;
		siblingIndex = flv->bones[childIndex].nextSiblingIndex;

		while (siblingIndex != -1)
		{
			buffer[siblingIndex] = bindPose[siblingIndex] * transform;
			siblingIndex = flv->bones[siblingIndex].nextSiblingIndex;
		}
	}
}

void FlverModel::Animate(MR::AnimationSourceHandle* animHandle, std::vector<int> flverToMorphemeBoneMap)
{
	//We initialise the final transforms to the flver bind pose so we can skip bones unhandled by morpheme in the next loop
	this->m_boneTransforms = this->m_boneBindPose;
	this->m_verts = this->m_vertBindPose;

	if (animHandle == nullptr)
		return;

	const MR::AnimRigDef* rig = animHandle->getRig();

	//Store morpheme global bind pose and animation transforms
	this->m_morphemeBoneTransforms.clear();
	this->m_morphemeBoneBindPose.clear();
	for (size_t i = 0; i < animHandle->getChannelCount(); i++)
	{
		this->m_morphemeBoneBindPose.push_back(ComputeNmBoneBindPoseGlobalTransform(animHandle->getRig(), i));
		this->m_morphemeBoneTransforms.push_back(ComputeNmBoneGlobalTransform(animHandle, i));
	}

	this->m_position = Vector3::Transform(Vector3::Zero, GetNmTrajectoryTransform(animHandle) * Matrix::CreateRotationX(-DirectX::XM_PIDIV2));

	for (size_t i = 0; i < this->m_flver->header.boneCount; i++)
	{
		int morphemeBoneIdx = flverToMorphemeBoneMap[i];

		if (morphemeBoneIdx != -1)
		{
			//Take the morpheme animation transform relative to the morpheme bind pose, mirror it on the ZY plane, and then apply them to the flver bind pose. Propagate to all children of the current bone
			Matrix morphemeRelativeTransform = (this->m_morphemeBoneBindPose[morphemeBoneIdx].Invert() * this->m_morphemeBoneTransforms[morphemeBoneIdx]);

			ApplyTransform(this->m_boneTransforms, this->m_flver, this->m_boneBindPose, (Matrix::CreateReflection(Plane(Vector3::Right)) * morphemeRelativeTransform), i);
		}
	}

	//Compute the relative transform to the flver bind pose. This will be used to transform model meshes in the skinning process
	std::vector<Matrix> boneRelativeTransforms;
	boneRelativeTransforms.reserve(this->m_flver->header.boneCount);

	for (size_t i = 0; i < this->m_flver->header.boneCount; i++)
		boneRelativeTransforms.push_back(this->m_boneBindPose[i].Invert() * this->m_boneTransforms[i]);

	for (size_t meshIdx = 0; meshIdx < this->m_flver->header.meshCount; meshIdx++)
	{
		for (size_t vertexIndex = 0; vertexIndex < this->m_vertBindPose[meshIdx].size(); vertexIndex++)
		{
			int* indices = this->m_vertBindPose[meshIdx][vertexIndex].bone_indices;
			float* weights = this->m_vertBindPose[meshIdx][vertexIndex].bone_weights;

			Vector3 newPos = Vector3::Zero;

			for (size_t wt = 0; wt < 4; wt++)
			{
				int boneID = indices[wt];

				newPos += Vector3::Transform(this->m_vertBindPose[meshIdx][vertexIndex].m_pos.position, boneRelativeTransforms[boneID]) * weights[wt];
			}

			this->m_verts[meshIdx][vertexIndex].m_pos.position = newPos;
		}
	}
}