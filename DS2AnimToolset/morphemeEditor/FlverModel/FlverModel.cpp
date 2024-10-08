#include "FlverModel.h"
#include "framework.h"
#include "extern.h"
#include "utils/NMDX/NMDX.h"
#include "RenderManager/RenderManager.h"
#include "FromSoftware/BND4/BND4.h"

namespace
{
	int getMorphemeRigBoneIndexByFlverBoneIndex(MR::AnimRigDef* pRig, FlverModel* pFlverModel, int boneId)
	{
		if (boneId == -1)
			return -1;

		std::string boneName = pFlverModel->getFlverBoneName(boneId);

		if (boneName == "LCalfTwist")
			boneName = "L_Calf";
		else if (boneName == "RCalfTwist")
			boneName = "R_Calf";

		if (boneName == "L_CalfTwist")
			boneName = "L_Calf";
		else if (boneName == "R_CalfTwist")
			boneName = "R_Calf";

		else if (boneName == "LThighTwist")
			boneName = "L_Thigh";
		else if (boneName == "RThighTwist")
			boneName = "R_Thigh";

		else if (boneName == "L_ThighTwist")
			boneName = "L_Thigh";
		else if (boneName == "R_ThighTwist")
			boneName = "R_Thigh";

		else if (boneName == "L_UpArmTwist")
			boneName = "L_UpperArm";
		else if (boneName == "R_UpArmTwist")
			boneName = "R_UpperArm";

		else if (boneName == "LUpArmTwist")
			boneName = "L_UpperArm";
		else if (boneName == "RUpArmTwist")
			boneName = "R_UpperArm";

		else if (boneName == "L_ForeTwist")
			boneName = "L_Forearm";
		else if (boneName == "R_ForeTwist")
			boneName = "R_Forearm";

		else if (boneName == "LForeTwist")
			boneName = "L_Forearm";
		else if (boneName == "RForeTwist")
			boneName = "R_Forearm";

		int boneIdx = pRig->getBoneIndexFromName(boneName.c_str());

		g_appLog->debugMessage(MsgLevel_Debug, "Bone %s: (to=%d, from=%d)\n", boneName.c_str(), boneIdx, boneId);

		return boneIdx;
	}

	int getFlverBoneIDByMorphemeBoneID(MR::AnimRigDef* pRig, FlverModel* pFlverModel, int idx)
	{
		std::string boneName = pRig->getBoneName(idx);
		int flverIdx = pFlverModel->getFlverBoneIndexByName(boneName.c_str());

		return flverIdx;
	}

	Matrix getNmTrajectoryTransform(MR::AnimationSourceHandle* animHandle)
	{
		NMP::Vector3 pos;
		NMP::Quat rot;

		animHandle->getTrajectory(rot, pos);

		float tmp = pos.z;
		pos.z = pos.y;
		pos.y = tmp;
		pos.x = -pos.x;

		Matrix translation = Matrix::CreateRotationX(-DirectX::XM_PIDIV2) * Matrix::CreateTranslation(NMDX::getDxVector(pos));
		Matrix rotation = Matrix::CreateRotationX(-DirectX::XM_PIDIV2) * Matrix::CreateFromQuaternion(NMDX::getDxQuat(rot));

		return rotation * translation;
	}

	Matrix getNmBoneTranform(MR::AnimationSourceHandle* animHandle, int channelId)
	{
		return NMDX::getWorldMatrix(animHandle->getChannelData()[channelId].m_quat, animHandle->getChannelData()[channelId].m_pos);
	}

	Matrix computeNmBoneGlobalTransform(MR::AnimationSourceHandle* animHandle, int channelId)
	{
		const MR::AnimRigDef* rig = animHandle->getRig();

		DirectX::XMMATRIX boneLocalTransform = getNmBoneTranform(animHandle, channelId);

		int parentIdx = rig->getParentBoneIndex(channelId);

		while (parentIdx != -1)
		{
			boneLocalTransform *= getNmBoneTranform(animHandle, parentIdx);

			parentIdx = rig->getParentBoneIndex(parentIdx);
		}

		boneLocalTransform *= Matrix::CreateRotationZ(DirectX::XM_PI);
		boneLocalTransform *= Matrix::CreateRotationX(DirectX::XM_PIDIV2);

		return boneLocalTransform;
	}

	Matrix computeNmBoneBindPoseGlobalTransform(const MR::AnimRigDef* rig, int channelId)
	{
		DirectX::XMMATRIX boneLocalTransform = NMDX::getWorldMatrix(*rig->getBindPoseBoneQuat(channelId), *rig->getBindPoseBonePos(channelId));
		int parentIdx = rig->getParentBoneIndex(channelId);

		while (parentIdx != -1)
		{
			boneLocalTransform *= NMDX::getWorldMatrix(*rig->getBindPoseBoneQuat(parentIdx), *rig->getBindPoseBonePos(parentIdx));

			parentIdx = rig->getParentBoneIndex(parentIdx);
		}

		boneLocalTransform *= Matrix::CreateRotationZ(DirectX::XM_PI);
		boneLocalTransform *= Matrix::CreateRotationX(DirectX::XM_PIDIV2);

		return boneLocalTransform;
	}

	Matrix getFlverBoneTransform(cfr::FLVER2* flver, int bone_id)
	{
		Matrix transform = Matrix::CreateScale(flver->bones[bone_id].scale.x, flver->bones[bone_id].scale.y, flver->bones[bone_id].scale.z);
		transform *= Matrix::CreateRotationX(flver->bones[bone_id].rot.x);
		transform *= Matrix::CreateRotationZ(flver->bones[bone_id].rot.z);
		transform *= Matrix::CreateRotationY(flver->bones[bone_id].rot.y);
		transform *= Matrix::CreateTranslation(flver->bones[bone_id].translation.x, flver->bones[bone_id].translation.y, flver->bones[bone_id].translation.z);

		return transform;
	}

	Matrix computeFlvBoneGlobalTransform(FLVER2* flv, int channelId)
	{
		Matrix localTransform = getFlverBoneTransform(flv, channelId);

		int parentIdx = flv->bones[channelId].parentIndex;

		while (parentIdx != -1)
		{
			Matrix parentBoneTransform = getFlverBoneTransform(flv, parentIdx);

			localTransform *= parentBoneTransform;

			parentIdx = flv->bones[parentIdx].parentIndex;
		}

		localTransform *= Matrix::CreateRotationY(DirectX::XM_PI);
		localTransform *= Matrix::CreateReflection(Plane(Vector3::Right));

		return localTransform;
	}

	Matrix applyTransformToFlverBone(std::vector<Matrix> transforms, FLVER2* flv, int channelId)
	{
		Matrix localTransform = transforms[channelId] * getFlverBoneTransform(flv, channelId);

		int parentIdx = flv->bones[channelId].parentIndex;

		while (parentIdx != -1)
		{
			Matrix parentBoneTransform = transforms[parentIdx] * getFlverBoneTransform(flv, parentIdx);

			localTransform *= parentBoneTransform;

			parentIdx = flv->bones[parentIdx].parentIndex;
		}

		localTransform *= Matrix::CreateRotationY(DirectX::XM_PI);

		return localTransform;
	}

	std::vector<Matrix> computeGlobalFlverRigTransform(FLVER2* flver)
	{
		std::vector<Matrix> boneTransforms;
		boneTransforms.reserve(flver->header.boneCount);

		for (size_t i = 0; i < flver->header.boneCount; i++)
			boneTransforms.push_back(computeFlvBoneGlobalTransform(flver, i));

		return boneTransforms;
	}

	std::vector<Matrix> computeGlobalTransforms(std::vector<Matrix> relativeTransforms, FLVER2* flv)
	{
		std::vector<Matrix> globalTransformedPos;
		globalTransformedPos.reserve(flv->header.boneCount);

		for (size_t i = 0; i < flv->header.boneCount; i++)
			globalTransformedPos.push_back(applyTransformToFlverBone(relativeTransforms, flv, i));

		return globalTransformedPos;
	}

	Matrix getNmRelativeBindPose(const MR::AnimRigDef* rig, int idx)
	{
		Matrix transform = NMDX::getWorldMatrix(*rig->getBindPoseBoneQuat(idx), *rig->getBindPoseBonePos(idx));
		transform *= Matrix::CreateRotationX(-DirectX::XM_PIDIV2);
		transform *= Matrix::CreateReflection(Plane(Vector3::Up));

		return transform;
	}

	Matrix getNmRelativeTransform(MR::AnimationSourceHandle* animHandle, int idx)
	{
		Matrix transform = NMDX::getWorldMatrix(animHandle->getChannelData()[idx].m_quat, animHandle->getChannelData()[idx].m_pos);
		transform *= Matrix::CreateRotationX(-DirectX::XM_PIDIV2);
		transform *= Matrix::CreateReflection(Plane(Vector3::Up));

		return transform;
	}

	void applyTransform(std::vector<Matrix>& buffer, FLVER2* flv, std::vector<Matrix> bindPose, Matrix transform, int boneID)
	{
		//Transform the bone
		buffer[boneID] = bindPose[boneID] * transform;

		int siblingIndex = flv->bones[boneID].nextSiblingIndex;

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
}

FlverModel::SkinnedVertex::SkinnedVertex(Vector3 pos, Vector3 normal, float* weights, int* bone_indices)
{
	this->vertexData = DirectX::VertexPositionNormalColor(pos, normal, DirectX::Colors::White);

	for (size_t i = 0; i < 4; i++)
	{
		this->boneIndices[i] = bone_indices[i];
		this->boneWeights[i] = weights[i];
	}
}

FlverModel::FlverModel()
{
}

FlverModel::FlverModel(UMEM* umem, MR::AnimRigDef* rig)
{
	this->m_loaded = false;
	this->m_verts.clear();

	this->m_position = Matrix::Identity;
	this->m_flver = new FLVER2(umem);

	float focus_y = (this->m_flver->header.boundingBoxMax.y + this->m_flver->header.boundingBoxMin.y) / 2;

	this->m_focusPoint = Vector3::Transform(Vector3::Zero, this->m_position) + DirectX::SimpleMath::Vector3(0, focus_y, 0);

	this->initModelData();

	this->m_loaded = true;

	this->m_nmRig = rig;

	this->createFlverToMorphemeBoneMap();
	this->createMorphemeToFlverBoneMap();
}

FlverModel::~FlverModel()
{
}

FlverModel* FlverModel::createFromBnd(std::wstring path, MR::AnimRigDef* rig)
{
	FlverModel* model = nullptr;

	Bnd4* bnd = Bnd4::loadFromFile(path);

	if (bnd == nullptr)
		return nullptr;

	bool found = false;

	for (size_t i = 0; i < bnd->getNumFiles(); i++)
	{
		std::filesystem::path name = bnd->getFile(i)->name;

		if (name.extension().compare(".flv") == 0)
		{
			UMEM* umem = uopenMem((char*)bnd->getFile(i)->data, bnd->getFile(i)->uncompressedSize);

			model = new FlverModel(umem, rig);
			model->m_name = std::filesystem::path(path).filename().replace_extension("").string();
			model->m_fileOrigin = path + L"\\" + name.c_str();

			g_appLog->debugMessage(MsgLevel_Debug, "Loaded model %s\n", model->m_name.c_str());

			found = true;
			break;
		}
	}

	if (!found)
		g_appLog->debugMessage(MsgLevel_Error, "Could not find model %ws\n", path);

	bnd->destroy();

	delete bnd;

	return model;
}

void FlverModel::destroy()
{
	if (this->m_flver)
		delete this->m_flver;

	delete this;
}

//Gets the vertices for the FLVER mesh at index idx
std::vector<Vector3> FlverModel::getFlverMeshVertices(int idx, bool flip)
{
	std::vector<Vector3> vertices;

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

	if (facesetp)
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
				vertices.push_back(Vector3(pos.x, pos.y, pos.z));
			else
				vertices.push_back(Vector3(pos.x, pos.y, -pos.z));
		}
	}

	return vertices;
}

//Gets the normals for the FLVER mesh at index idx
std::vector<Vector3> FlverModel::getFlverMeshNormals(int idx, bool flip)
{
	std::vector<Vector3> normals;

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

	if (facesetp)
	{
		for (int i = 0; i < facesetp->triCount; i++)
		{
			int vertexIndex = facesetp->triList[i];

			float x = mesh->vertexData->normals[(vertexIndex * 3) + 0];
			float y = mesh->vertexData->normals[(vertexIndex * 3) + 2];
			float z = mesh->vertexData->normals[(vertexIndex * 3) + 1];

			if (!flip)
				normals.push_back(Vector3(x, y, z));
			else
				normals.push_back(Vector3(x, y, -z));
		}
	}

	return normals;
}

std::vector<Vector3> FlverModel::getFlverMeshTangents(int idx, bool flip)
{
	std::vector<Vector3> tangents;

	if (m_flver == nullptr)
		return tangents;

	if (idx > m_flver->header.meshCount)
		return tangents;

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

	if (facesetp)
	{
		for (int i = 0; i < facesetp->triCount; i++)
		{
			int vertexIndex = facesetp->triList[i];

			float x = mesh->vertexData->tangents[(vertexIndex * 3) + 0];
			float y = mesh->vertexData->tangents[(vertexIndex * 3) + 2];
			float z = mesh->vertexData->tangents[(vertexIndex * 3) + 1];

			if (!flip)
				tangents.push_back(Vector3(x, y, z));
			else
				tangents.push_back(Vector3(x, y, -z));
		}
	}

	return tangents;
}

std::vector<Vector3> FlverModel::getFlverMeshBiTangents(int idx, bool flip)
{
	std::vector<Vector3> bitangents;

	if (m_flver == nullptr)
		return bitangents;

	if (idx > m_flver->header.meshCount)
		return bitangents;

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

	if (facesetp)
	{
		for (int i = 0; i < facesetp->triCount; i++)
		{
			int vertexIndex = facesetp->triList[i];

			float x = mesh->vertexData->bitangents[(vertexIndex * 3) + 0];
			float y = mesh->vertexData->bitangents[(vertexIndex * 3) + 2];
			float z = mesh->vertexData->bitangents[(vertexIndex * 3) + 1];

			if (!flip)
				bitangents.push_back(Vector3(x, y, z));
			else
				bitangents.push_back(Vector3(x, y, -z));
		}
	}

	return bitangents;
}

//Gets the bone weights for the FLVER mesh at index idx
std::vector<Vector4> FlverModel::getFlverMeshBoneWeights(int idx)
{
	std::vector<Vector4> weights;

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

	if (facesetp)
	{
		for (size_t i = 0; i < facesetp->triCount; i++)
		{
			int vertexIndex = facesetp->triList[i];

			float x = mesh->vertexData->bone_weights[(vertexIndex * 4) + 0];
			float y = mesh->vertexData->bone_weights[(vertexIndex * 4) + 1];
			float z = mesh->vertexData->bone_weights[(vertexIndex * 4) + 2];
			float w = mesh->vertexData->bone_weights[(vertexIndex * 4) + 3];

			weights.push_back(Vector4(x, y, z, w));
		}
	}

	return weights;
}

std::vector<std::vector<int>> FlverModel::getFlverMeshBoneIndices(int idx)
{
	std::vector<std::vector<int>> buffer;

	if (m_flver == nullptr)
		return buffer;

	if (idx > m_flver->header.meshCount)
		return buffer;

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

	if (facesetp)
	{
		buffer.reserve(facesetp->triCount);

		for (size_t i = 0; i < facesetp->triCount; i++)
		{
			int vertexIndex = facesetp->triList[i];

			std::vector<int> indices;
			indices.reserve(4);

			for (size_t j = 0; j < 4; j++)
				indices.push_back(mesh->boneIndices[mesh->vertexData->bone_indices[(vertexIndex * 4) + j]]);

			buffer.push_back(indices);
		}
	}

	return buffer;
}

//Gets all the model vertices for all the meshes and stores them into m_verts
void FlverModel::initModelData()
{
	this->m_verts.clear();
	this->m_boneTransforms.clear();
	this->m_vertBindPose.clear();
	this->m_boneBindPose.clear();

	if (m_flver == nullptr)
		return;

	this->m_boneBindPose = computeGlobalFlverRigTransform(this->m_flver);
	this->m_boneTransforms = this->m_boneBindPose;

	this->m_verts.reserve(m_flver->header.meshCount);
	this->m_vertBindPose.reserve(m_flver->header.meshCount);

	for (int i = 0; i < m_flver->header.meshCount; i++)
	{
		std::vector<Vector3> vertices = this->getFlverMeshVertices(i, false);
		std::vector<Vector3> normals = this->getFlverMeshNormals(i, false);
		std::vector<Vector3> tangents = this->getFlverMeshTangents(i, false);
		std::vector<Vector3> bitangents = this->getFlverMeshBiTangents(i, false);
		std::vector<Vector4> boneWeights = this->getFlverMeshBoneWeights(i);
		std::vector<std::vector<int>> boneIndices = this->getFlverMeshBoneIndices(i);

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

		std::vector<SkinnedVertex> meshSkinnedVertices;

		if (facesetp)
		{
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

				if (mesh->vertexData->bone_indices[(vertexIndex * 4) + 0] < mesh->header.boneCount)
					indices[0] = mesh->boneIndices[mesh->vertexData->bone_indices[(vertexIndex * 4) + 0]];

				if (mesh->vertexData->bone_indices[(vertexIndex * 4) + 1] < mesh->header.boneCount)
					indices[1] = mesh->boneIndices[mesh->vertexData->bone_indices[(vertexIndex * 4) + 1]];

				if (mesh->vertexData->bone_indices[(vertexIndex * 4) + 2] < mesh->header.boneCount)
					indices[2] = mesh->boneIndices[mesh->vertexData->bone_indices[(vertexIndex * 4) + 2]];

				if (mesh->vertexData->bone_indices[(vertexIndex * 4) + 3] < mesh->header.boneCount)
					indices[3] = mesh->boneIndices[mesh->vertexData->bone_indices[(vertexIndex * 4) + 3]];

				float x = mesh->vertexData->positions[(vertexIndex * 3) + 0];
				float y = mesh->vertexData->positions[(vertexIndex * 3) + 2];
				float z = mesh->vertexData->positions[(vertexIndex * 3) + 1];

				Vector3 pos = Vector3::Transform(DirectX::SimpleMath::Vector3(x, y, z), Matrix::CreateReflection(Plane(Vector3::Right)));

				float norm_x = mesh->vertexData->normals[(vertexIndex * 3) + 0];
				float norm_y = mesh->vertexData->normals[(vertexIndex * 3) + 2];
				float norm_z = mesh->vertexData->normals[(vertexIndex * 3) + 1];

				Vector3 normal = Vector3::Transform(DirectX::SimpleMath::Vector3(norm_x, norm_y, norm_z), Matrix::CreateReflection(Plane(Vector3::Right)));

				meshSkinnedVertices.push_back(SkinnedVertex(pos, normal, weights, indices));
			}
		}

		this->m_vertBindPose.push_back(meshSkinnedVertices);
	}

	this->m_verts = this->m_vertBindPose;
}

void FlverModel::update(float dt)
{
	if (m_flver == nullptr)
		return;

	DirectX::SimpleMath::Vector4 color = DirectX::SimpleMath::Vector4(0.7f, 0.7f, 0.7f, 1.f);

	for (int i = 0; i < this->m_vertBindPose.size(); i++)
		for (size_t j = 0; j < this->m_vertBindPose[i].size(); j++)
			m_vertBindPose[i][j].vertexData.color = color;

	this->m_focusPoint = Vector3::Transform(Vector3::Zero, this->m_position * Matrix::CreateScale(this->m_scale));

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

void FlverModel::draw(RenderManager* renderManager)
{
	const Vector4 boneMarkerColor = RMath::getFloatColor(IM_COL32(51, 102, 255, 255));

	Matrix world = this->getWorldMatrix();

	renderManager->applyDebugEffect(world);
	renderManager->setInputLayout(kDebugLayout);

	DirectX::PrimitiveBatch<DirectX::VertexPositionColor> prim(renderManager->getDeviceContext(), UINT16_MAX * 3, UINT16_MAX);
	prim.Begin();

	int boneCount = this->m_boneTransforms.size();

	int trajectoryBoneIndex = this->getFlverBoneIndexByMorphemeBoneIndex(this->m_nmRig->getTrajectoryBoneIndex());
	int characterRootBoneIdx = this->getFlverBoneIndexByMorphemeBoneIndex(this->m_nmRig->getCharacterRootBoneIndex());

	if (this->m_settings.selectedBone != -1)
	{
		DX::DrawReferenceFrame(&prim, this->m_boneTransforms[this->m_settings.selectedBone]);
		renderManager->addText(RString::toNarrow(this->m_flver->bones[this->m_settings.selectedBone].name).c_str(), this->m_boneTransforms[this->m_settings.selectedBone] * world);
	}

	if (this->m_settings.drawDummyPolygons && this->m_flver)
	{
		for (size_t i = 0; i < this->m_flver->header.dummyCount; i++)
		{
			std::string dummy_name = "Dmy_" + std::to_string(this->m_flver->dummies[i].referenceID);

			DX::DrawReferenceFrame(&prim, this->m_dummyPolygons[i]);
			renderManager->addText(dummy_name.c_str(), this->m_dummyPolygons[i] * world);
		}
	}
	else if (this->m_settings.selectedDummy != -1)
	{
		std::string dummy_name = "Dmy_" + std::to_string(this->m_flver->dummies[this->m_settings.selectedDummy].referenceID);

		DX::DrawReferenceFrame(&prim, this->m_dummyPolygons[this->m_settings.selectedDummy]);
		renderManager->addText(dummy_name.c_str(), this->m_dummyPolygons[this->m_settings.selectedDummy] * world);
	}

	if (this->m_settings.drawBones && this->m_flver)
	{
		for (size_t i = 0; i < boneCount; i++)
		{
			int morphemeBoneIdx = this->getMorphemeBoneIdByFlverBoneId(i);

			if ((morphemeBoneIdx == -1) || (i == trajectoryBoneIndex) || (i == characterRootBoneIdx))
				continue;

			int parentIndex = this->m_flver->bones[i].parentIndex;

			if (parentIndex != -1)
			{
				Vector3 boneA = Vector3::Transform(Vector3::Zero, this->m_boneTransforms[i]);
				Vector3 boneB = Vector3::Transform(Vector3::Zero, this->m_boneTransforms[parentIndex]);

				DX::DrawJoint(&prim, Matrix::Identity, boneB, boneA, boneMarkerColor);

				if (this->m_flver->bones[i].childIndex == -1)
					DX::Draw(&prim, DirectX::BoundingSphere(boneA, 0.03f), boneMarkerColor);
			}
		}

		DX::DrawSphere(&prim, *this->getFlverRootBoneTransform(), 0.03f, DirectX::Colors::MediumBlue);
		DX::DrawReferenceFrame(&prim, *this->getFlverTrajectoryBoneTransform());
	}

	if (this->m_settings.drawBoundingBox)
	{
		Vector3 halfExtents = (this->getBoundingBoxMax() - this->getBoundingBoxMin()) / 2;

		DX::DrawBoundingBox(&prim, world, Vector3::Zero, halfExtents, DirectX::Colors::DarkRed);
	}

	if (this->m_settings.highlight)
	{
		renderManager->addText(this->getModelName(), world);

		if (this->m_settings.displayMode != kDispWireframe)
			DX::DrawModelWireframe(&prim, Matrix::Identity, this, Vector4(DirectX::Colors::White));
	}

	prim.End();

	if (this->m_settings.displayMode != kDispWireframe)
	{
		DirectX::PrimitiveBatch<DirectX::VertexPositionNormalColor> primShaded(renderManager->getDeviceContext(), UINT16_MAX * 3, UINT16_MAX);

		float alpha = 1.f;

		if (this->m_settings.displayMode == kDispXRay)
			alpha = 0.5f;

		renderManager->applyPhysicalEffect(world, alpha);
		renderManager->setInputLayout(kPhysicalLayout);

		primShaded.Begin();
		DX::DrawModel(&primShaded, Matrix::Identity, this);
		primShaded.End();
	}
	else
	{
		prim.Begin();
		DX::DrawModelWireframe(&prim, Matrix::Identity, this, Vector4(DirectX::Colors::White));
		prim.End();
	}
}

int FlverModel::getFlverBoneIndexByName(const char* name)
{
	for (size_t i = 0; i < this->m_flver->header.boneCount; i++)
	{
		if (this->getFlverBoneName(i).compare(name) == 0)
			return i;
	}

	return -1;
}

int FlverModel::getMorphemeBoneIndexByName(const char* name)
{
	for (size_t i = 0; i < this->m_nmRig->getNumBones(); i++)
	{
		if (this->getMorphemeBoneName(i).compare(name) == 0)
			return i;
	}

	return -1;
}

//Creates an anim map from the flver model bone to the morpheme rig and saves it in m_morphemeToFlverRigMap
void FlverModel::createFlverToMorphemeBoneMap()
{
	this->m_flverToMorphemeBoneMap.clear();
	this->m_flverToMorphemeBoneMap.reserve(this->m_flver->header.boneCount);

	for (int i = 0; i < this->m_flver->header.boneCount; i++)
		this->m_flverToMorphemeBoneMap.push_back(getMorphemeRigBoneIndexByFlverBoneIndex(this->m_nmRig, this, i));
}

void FlverModel::createMorphemeToFlverBoneMap()
{
	this->m_morphemeToFlverBoneMap.clear();
	this->m_morphemeToFlverBoneMap.reserve(this->m_nmRig->getNumBones());

	for (int idx = 0; idx < this->m_flver->header.boneCount; idx++)
	{
		this->m_morphemeToFlverBoneMap.push_back(-1);

		for (size_t i = 0; i < this->m_flverToMorphemeBoneMap.size(); i++)
		{
			if (this->m_flverToMorphemeBoneMap[i] == idx)
				this->m_morphemeToFlverBoneMap.back() = i;
		}
	}
}

int FlverModel::getMorphemeBoneIdByFlverBoneId(int idx)
{
	if (idx < this->m_flverToMorphemeBoneMap.size())
		return this->m_flverToMorphemeBoneMap[idx];

	return -1;
}

int FlverModel::getFlverBoneIndexByMorphemeBoneIndex(int idx)
{
	if (idx < this->m_morphemeToFlverBoneMap.size())
		return this->m_morphemeToFlverBoneMap[idx];

	return -1;
}

Matrix FlverModel::getDummyPolygonTransform(int id)
{
	for (size_t i = 0; i < this->m_flver->header.dummyCount; i++)
	{
		if (this->m_flver->dummies[i].referenceID == id)
			return this->m_dummyPolygons[i] * this->m_position;
	}

	g_appLog->debugMessage(MsgLevel_Error, "Could not find dummy polygon %d (%s)\n", id, this->m_name);

	return Matrix::Identity;
}

FlverModel::SkinnedVertex* FlverModel::getVertex(int meshIdx, int idx)
{
	if (meshIdx > this->m_verts.size() || idx > this->m_verts[meshIdx].size())
		return nullptr;

	return &this->m_verts[meshIdx][idx];
}

FlverModel::SkinnedVertex* FlverModel::getVertexBindPose(int meshIdx, int idx)
{
	if (meshIdx > this->m_vertBindPose.size() || idx > this->m_vertBindPose[meshIdx].size())
		return nullptr;

	return &this->m_vertBindPose[meshIdx][idx];
}

Matrix* FlverModel::getFlverBoneTransform(int idx)
{
	if (idx > this->m_boneTransforms.size())
		return nullptr;

	return &this->m_boneTransforms[idx];
}

Matrix* FlverModel::getFlverBoneBindPose(int idx)
{
	if (idx > this->m_boneBindPose.size())
		return nullptr;

	return &this->m_boneBindPose[idx];
}

Matrix* FlverModel::getFlverRootBoneTransform()
{
	int flverBoneIdx = this->getFlverBoneIndexByMorphemeBoneIndex(this->m_nmRig->getCharacterRootBoneIndex());

	return this->getFlverBoneTransform(flverBoneIdx);
}

Matrix* FlverModel::getFlverTrajectoryBoneTransform()
{
	int flverBoneIdx = this->getFlverBoneIndexByMorphemeBoneIndex(this->m_nmRig->getTrajectoryBoneIndex());

	return this->getFlverBoneTransform(flverBoneIdx);
}

Vector3 FlverModel::getBoundingBoxMin()
{
	return Vector3(this->m_flver->header.boundingBoxMin.x, this->m_flver->header.boundingBoxMin.y, this->m_flver->header.boundingBoxMin.z);
}

Vector3 FlverModel::getBoundingBoxMax()
{
	return Vector3(this->m_flver->header.boundingBoxMax.x, this->m_flver->header.boundingBoxMax.y, this->m_flver->header.boundingBoxMax.z);
}

Matrix* FlverModel::getMorphemeBoneTransform(int idx)
{
	if (idx > this->m_morphemeBoneTransforms.size())
		return nullptr;

	return &this->m_morphemeBoneBindPose[idx];
}

Matrix* FlverModel::getMorphemeBoneBindPose(int idx)
{
	if (idx > this->m_morphemeBoneBindPose.size())
		return nullptr;

	return &this->m_morphemeBoneBindPose[idx];
}

Matrix* FlverModel::getMorphemeRootBoneTransform()
{
	return this->getMorphemeBoneTransform(this->m_nmRig->getCharacterRootBoneIndex());
}

Matrix* FlverModel::getMorphemeTrajectoryBoneTransform()
{
	return this->getMorphemeBoneTransform(this->m_nmRig->getTrajectoryBoneIndex());
}

std::string FlverModel::getMorphemeBoneName(int idx)
{
	if (idx < this->m_nmRig->getNumBones())
		return this->m_nmRig->getBoneName(idx);

	return "";
}

std::string FlverModel::getFlverBoneName(int idx)
{
	if (idx < this->m_flver->header.boneCount)
		return RString::toNarrow(this->m_flver->bones[idx].name);

	return "";
}

void FlverModel::animate(MR::AnimationSourceHandle* animHandle)
{
	//We initialise the final transforms to the flver bind pose so we can skip bones unhandled by morpheme in the next loop
	this->m_boneTransforms = this->m_boneBindPose;
	this->m_verts = this->m_vertBindPose;

	if (animHandle)
	{
		const MR::AnimRigDef* rig = animHandle->getRig();

		//Store morpheme global bind pose and animation transforms
		this->m_morphemeBoneTransforms.clear();
		this->m_morphemeBoneBindPose.clear();
		for (size_t i = 0; i < animHandle->getChannelCount(); i++)
		{
			this->m_morphemeBoneBindPose.push_back(computeNmBoneBindPoseGlobalTransform(animHandle->getRig(), i));
			this->m_morphemeBoneTransforms.push_back(computeNmBoneGlobalTransform(animHandle, i));
		}

		this->m_position = getNmTrajectoryTransform(animHandle) * Matrix::CreateRotationY(DirectX::XM_PI);

		for (size_t i = 0; i < this->m_flver->header.boneCount; i++)
		{
			int morphemeBoneIdx = this->m_flverToMorphemeBoneMap[i];

			if (morphemeBoneIdx != -1)
			{
				//Take the morpheme animation transform relative to the morpheme bind pose, mirror it on the ZY plane, and then apply them to the flver bind pose. Propagate to all children of the current bone
				Matrix morphemeRelativeTransform = (this->m_morphemeBoneBindPose[morphemeBoneIdx].Invert() * this->m_morphemeBoneTransforms[morphemeBoneIdx]);

				applyTransform(this->m_boneTransforms, this->m_flver, this->m_boneBindPose, (Matrix::CreateReflection(Plane(Vector3::Right)) * Matrix::CreateReflection(Plane(Vector3::Up)) * morphemeRelativeTransform), i);
			}
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
			int* indices = this->m_vertBindPose[meshIdx][vertexIndex].boneIndices;
			float* weights = this->m_vertBindPose[meshIdx][vertexIndex].boneWeights;

			Vector3 newPos = Vector3::Zero;
			Vector3 newNorm = Vector3::Zero;

			for (size_t wt = 0; wt < 4; wt++)
			{
				int boneID = indices[wt];

				if (boneID < 0 || boneID >= boneRelativeTransforms.size())
					continue;

				newPos += Vector3::Transform(this->m_vertBindPose[meshIdx][vertexIndex].vertexData.position, boneRelativeTransforms[boneID]) * weights[wt];
				newNorm += Vector3::Transform(this->m_vertBindPose[meshIdx][vertexIndex].vertexData.normal, boneRelativeTransforms[boneID]) * weights[wt];
			}

			this->m_verts[meshIdx][vertexIndex].vertexData.position = newPos;
			this->m_verts[meshIdx][vertexIndex].vertexData.normal = newNorm;
		}
	}
}