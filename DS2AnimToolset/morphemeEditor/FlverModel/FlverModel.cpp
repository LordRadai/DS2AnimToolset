#include "FlverModel.h"
#include "framework.h"
#include "extern.h"
#include "utils/NMDX/NMDX.h"
#include "RenderManager/RenderManager.h"
#include "RCore.h"

namespace
{
	int getMorphemeRigBoneIndexByFlverBoneIndex(MR::AnimRigDef* pRig, FlverModel* pFlverModel, int boneId)
	{
		if (boneId == -1)
			return -1;

		std::string boneName = pFlverModel->getFlverBoneName(boneId);

		return pRig->getBoneIndexFromName(boneName.c_str());
	}

	int getFlverBoneIDByMorphemeBoneID(MR::AnimRigDef* pRig, FlverModel* pFlverModel, int idx)
	{
		std::string boneName = pRig->getBoneName(idx);

		return pFlverModel->getFlverBoneIndexByName(boneName.c_str());
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

		Matrix translation = Matrix::CreateRotationX(-DirectX::XM_PIDIV2) * Matrix::CreateTranslation(utils::NMDX::getDxVector(pos));
		Matrix rotation = Matrix::CreateRotationX(-DirectX::XM_PIDIV2) * Matrix::CreateFromQuaternion(utils::NMDX::getDxQuat(rot));

		return rotation * translation;
	}

	Matrix getAnimBoneTranform(const MR::AnimationSourceHandle* animHandle, int channelId)
	{
		return utils::NMDX::getTransformMatrix(animHandle->getChannelData()[channelId].m_quat, animHandle->getChannelData()[channelId].m_pos);
	}

	Matrix computeNmAnimGlobalTransform(const MR::AnimationSourceHandle* animHandle, int channelId)
	{
		const MR::AnimRigDef* rig = animHandle->getRig();

		DirectX::XMMATRIX boneLocalTransform = getAnimBoneTranform(animHandle, channelId);
		int parentID = rig->getParentBoneIndex(channelId);

		while (parentID != -1)
		{
			boneLocalTransform *= getAnimBoneTranform(animHandle, parentID);

			parentID = rig->getParentBoneIndex(parentID);
		}

		boneLocalTransform *= Matrix::CreateRotationZ(DirectX::XM_PI);
		boneLocalTransform *= Matrix::CreateRotationX(DirectX::XM_PIDIV2);

		return boneLocalTransform;
	}

	Matrix computeNmBoneGlobalTransform(const MR::AnimRigDef* rig, int channelId)
	{
		DirectX::XMMATRIX boneLocalTransform = utils::NMDX::getTransformMatrix(*rig->getBindPoseBoneQuat(channelId), *rig->getBindPoseBonePos(channelId));
		int parentIdx = rig->getParentBoneIndex(channelId);

		while (parentIdx != -1)
		{
			boneLocalTransform *= utils::NMDX::getTransformMatrix(*rig->getBindPoseBoneQuat(parentIdx), *rig->getBindPoseBonePos(parentIdx));

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

	std::vector<Matrix> computeGlobalFlverRigTransforms(FLVER2* flver)
	{
		std::vector<Matrix> boneTransforms;
		boneTransforms.reserve(flver->header.boneCount);

		for (size_t i = 0; i < flver->header.boneCount; i++)
			boneTransforms.push_back(computeFlvBoneGlobalTransform(flver, i));

		return boneTransforms;
	}

	std::vector<Matrix> computeGlobalMorphemeRigTransforms(MR::AnimRigDef* rig)
	{
		std::vector<Matrix> boneTransforms;
		boneTransforms.reserve(rig->getNumBones());

		for (size_t i = 0; i < rig->getNumBones(); i++)
			boneTransforms.push_back(computeNmBoneGlobalTransform(rig, i));

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
		Matrix transform = utils::NMDX::getTransformMatrix(*rig->getBindPoseBoneQuat(idx), *rig->getBindPoseBonePos(idx));
		transform *= Matrix::CreateRotationX(-DirectX::XM_PIDIV2);
		transform *= Matrix::CreateReflection(Plane(Vector3::Up));

		return transform;
	}

	Matrix getNmRelativeTransform(MR::AnimationSourceHandle* animHandle, int idx)
	{
		Matrix transform = utils::NMDX::getTransformMatrix(animHandle->getChannelData()[idx].m_quat, animHandle->getChannelData()[idx].m_pos);
		transform *= Matrix::CreateRotationX(-DirectX::XM_PIDIV2);
		transform *= Matrix::CreateReflection(Plane(Vector3::Up));

		return transform;
	}

	void applyTransform(std::vector<Matrix>& buffer, FLVER2* flv, std::vector<Matrix> bindPose, Matrix transform, int boneID)
	{
		// Transform the bone
		buffer[boneID] = bindPose[boneID] * transform;

		// Transforms all the children of the current bone
		int childIndex = flv->bones[boneID].childIndex;

		if (childIndex != -1)
		{
			buffer[childIndex] = bindPose[childIndex] * transform;
			int siblingIndex = flv->bones[childIndex].nextSiblingIndex;

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
	const Vector4 vertexColor = Vector4(DirectX::Colors::White);

	this->vertexData = DirectX::VertexPositionNormalColor(pos, normal, vertexColor);

	for (uint8_t i = 0; i < 4; i++)
	{
		this->boneIndices[i] = bone_indices[i];
		this->boneWeights[i] = weights[i];
	}
}

FlverModel::FlverModel(UMEM* umem, MR::AnimRigDef* rig)
{
	this->m_loaded = false;
	this->m_meshVerticesTransforms.clear();

	this->m_position = Matrix::Identity;
	this->m_flver = new FLVER2(umem);

	float focus_y = (this->m_flver->header.boundingBoxMax.y + this->m_flver->header.boundingBoxMin.y) / 2;

	this->m_focusPoint = Vector3::Transform(Vector3::Zero, this->m_position) + Vector3(0, focus_y, 0);

	this->m_loaded = true;

	this->m_nmRig = rig;

	g_appLog->debugMessage(MsgLevel_Info, "Creating bone maps:\n");

	this->createFlverToMorphemeBoneMap();
	this->createMorphemeToFlverBoneMap();

	if (!this->initialise())
		g_appLog->panicMessage("Flver model initialisation failed");
}

FlverModel* FlverModel::createFromBnd(std::wstring path, MR::AnimRigDef* rig)
{
	FlverModel* model = nullptr;

	try
	{
		BND4::Bnd4* bnd = BND4::Bnd4::loadFromFile(path);

		if (bnd == nullptr)
			return nullptr;

		BND4::BndFile* flverFile = bnd->getFirstFileWithExtension(".flv");

		if (flverFile)
		{
			g_appLog->debugMessage(MsgLevel_Debug, "Loading model \"%ws\"\n", path.c_str());

			UMEM* umem = uopenMem((char*)flverFile->data, flverFile->uncompressedSize);

			model = new FlverModel(umem, rig);
			model->m_name = std::filesystem::path(path).filename().replace_extension("").string();
			model->m_fileOrigin = path + L"\\" + RString::toWide(flverFile->name.c_str());
		}
		else
			g_appLog->debugMessage(MsgLevel_Error, "Could not find a .flver file inside \"%ws\"\n", path);

		bnd->destroy();

		delete bnd;

		return model;
	}
	catch (const std::exception& e)
	{
		g_appLog->alertMessage(MsgLevel_Error, "Failed to create FlverModel object from file %ws (error=%s)\n", path.c_str(), e.what());
		return model;
	}
}

void FlverModel::destroy()
{
	if (this->m_flver)
		delete this->m_flver;

	delete this;
}

// Gets the vertices for the FLVER mesh at index idx 
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

// Gets the normals for the FLVER mesh at index idx 
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

// Gets the tangents for the FLVER mesh at index idx 
// WARNING! This data seems to be wrong, I do not advise you to inclide these when exporting)
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

// Gets the bitangents for the FLVER mesh at index idx 
// WARNING! This data seems to be wrong, I do not advise you to inclide these when exporting)
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

// Gets the bone weights for the FLVER mesh at index idx
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

// Gets the bone influence indices for the FLVER mesh at index idx
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

std::vector<FlverModel::SkinnedVertex> FlverModel::getBindPoseSkinnedVertices(int idx)
{
	return this->m_meshVerticesBindPoseTransforms[idx];
}

void FlverModel::validateSkinnedVertexData(FlverModel::SkinnedVertex& skinnedVertex)
{
	float totalWeight = 0.f;
	for (size_t wt = 0; wt < 4; wt++)
	{
		const uint32_t morphemeBoneID = this->getMorphemeBoneIdByFlverBoneId(skinnedVertex.boneIndices[wt]);

		// Only factor in the bones that are present in the morpheme rig
		if (morphemeBoneID != -1)
			totalWeight += skinnedVertex.boneWeights[wt];
		else
		{
			//skinnedVertex.boneWeights[wt] = 0.f;
			//skinnedVertex.boneIndices[wt] = -1;
		}
	}

	if (totalWeight != 0.f)
	{
		if (totalWeight != 1.f)
		{
			for (size_t wt = 0; wt < 4; wt++)
				skinnedVertex.boneWeights[wt] /= totalWeight;
		}
	}
	else
	{
		for (size_t wt = 0; wt < 4; wt++)
		{
			const int boneID = skinnedVertex.boneIndices[wt];
			int parentID = this->m_flver->bones[boneID].previousSiblingIndex;

			while (parentID != -1)
			{
				int morphemeBoneID = this->getMorphemeBoneIdByFlverBoneId(parentID);

				if (morphemeBoneID != -1)
				{
					skinnedVertex.boneIndices[wt] = parentID;

					break;
				}

				parentID = this->m_flver->bones[parentID].previousSiblingIndex;
			}
		}

		validateSkinnedVertexData(skinnedVertex);
	}
}

// Gets all the model vertices for all the meshes and stores them into m_verts
bool FlverModel::initialise()
{
	if (this->m_flver == nullptr)
		return false;

	this->m_boneBindPoseTransforms = computeGlobalFlverRigTransforms(this->m_flver);
	this->m_boneTransforms = this->m_boneBindPoseTransforms;

	this->m_boneInverseBindPoseTransforms.reserve(this->m_boneBindPoseTransforms.size());
	for (size_t i = 0; i < this->m_boneBindPoseTransforms.size(); i++)
		this->m_boneInverseBindPoseTransforms.push_back(this->m_boneBindPoseTransforms[i].Invert());

	if (this->m_nmRig)
	{
		this->m_morphemeBoneBindPoseTransforms = computeGlobalMorphemeRigTransforms(this->m_nmRig);
		this->m_morphemeBoneTransforms = this->m_morphemeBoneBindPoseTransforms;

		this->m_morphemeInverseBoneBindPoseTransforms.reserve(this->m_morphemeBoneBindPoseTransforms.size());
		for (size_t i = 0; i < this->m_morphemeBoneBindPoseTransforms.size(); i++)
			this->m_morphemeInverseBoneBindPoseTransforms.push_back(this->m_morphemeBoneBindPoseTransforms[i].Invert());
	}

	this->m_meshVerticesTransforms.reserve(this->m_flver->header.meshCount);
	this->m_meshVerticesBindPoseTransforms.reserve(this->m_flver->header.meshCount);

	for (int i = 0; i < this->m_flver->header.meshCount; i++)
	{
		std::vector<Vector3> vertices = this->getFlverMeshVertices(i, false);
		std::vector<Vector3> normals = this->getFlverMeshNormals(i, false);
		std::vector<Vector4> boneWeights = this->getFlverMeshBoneWeights(i);
		std::vector<std::vector<int>> boneIndices = this->getFlverMeshBoneIndices(i);

		cfr::FLVER2::Mesh* mesh = &this->m_flver->meshes[i];

		int vertexCount = 0;

		for (int vbi = 0; vbi < mesh->header.vertexBufferCount; vbi++)
		{
			int vb_index = this->m_flver->meshes[i].vertexBufferIndices[vbi];
			vertexCount += this->m_flver->vertexBuffers[vb_index].header.vertexCount;
		}

		int uvCount = 0;
		int colorCount = 0;
		int tanCount = 0;

		this->m_flver->getVertexData(i, &uvCount, &colorCount, &tanCount);

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

				Vector3 pos = Vector3::Transform(Vector3(x, y, z), Matrix::CreateReflection(Plane(Vector3::Right)));

				float norm_x = mesh->vertexData->normals[(vertexIndex * 3) + 0];
				float norm_y = mesh->vertexData->normals[(vertexIndex * 3) + 2];
				float norm_z = mesh->vertexData->normals[(vertexIndex * 3) + 1];

				Vector3 normal = Vector3::Transform(Vector3(norm_x, norm_y, norm_z), Matrix::CreateReflection(Plane(Vector3::Right)));
				normal.Normalize();

				meshSkinnedVertices.push_back(SkinnedVertex(pos, normal, weights, indices));
				validateSkinnedVertexData(meshSkinnedVertices.back());
			}
		}

		this->m_meshVerticesBindPoseTransforms.push_back(meshSkinnedVertices);
	}

	this->m_meshVerticesTransforms = this->m_meshVerticesBindPoseTransforms;

	const Vector3 modelSize = this->getBoundingBoxMax() - this->getBoundingBoxMin();
	const float largestDirection = std::fmax(std::fmax(modelSize.x, modelSize.y), modelSize.z);

	this->m_scale = std::fmin(20.f / largestDirection, 1.5f);

	if (this->m_scale < 1.5f)
		this->m_scale = 1.5f;

	return true;
}

void FlverModel::update(float dt)
{
	if (this->m_flver == nullptr)
		return;

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

//Draws the character
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

		DX::DrawSphere(&prim, *this->getFlverRootBoneGlobalTransform(), 0.03f, DirectX::Colors::MediumBlue);
		DX::DrawReferenceFrame(&prim, *this->getFlverTrajectoryBoneGlobalTransform());
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

	if (this->m_settings.drawMeshes)
	{
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
}

//Looks up a bone in the flver rig by name
int FlverModel::getFlverBoneIndexByName(const char* name)
{
	for (size_t i = 0; i < this->m_flver->header.boneCount; i++)
	{
		if (this->getFlverBoneName(i).compare(name) == 0)
			return i;
	}

	return -1;
}

//Looks up a bone in the morpheme rig by name
int FlverModel::getMorphemeBoneIndexByName(const char* name)
{
	for (size_t i = 0; i < this->m_nmRig->getNumBones(); i++)
	{
		if (this->getMorphemeBoneName(i).compare(name) == 0)
			return i;
	}

	return -1;
}

//Creates an anim map from the flver rig to the morpheme rig
void FlverModel::createFlverToMorphemeBoneMap()
{	
	this->m_flverToMorphemeBoneMap.reserve(this->m_flver->header.boneCount);

	for (int i = 0; i < this->m_flver->header.boneCount; i++)
	{
		this->m_flverToMorphemeBoneMap.push_back(getMorphemeRigBoneIndexByFlverBoneIndex(this->m_nmRig, this, i));
		
		std::string boneName = this->getFlverBoneName(i);
		g_appLog->debugMessage(MsgLevel_Debug, "\tBone \"%s\": (to=%d, from=%d)\n", boneName.c_str(), this->m_flverToMorphemeBoneMap[i], i);
	}
}

//Creates an anim map from the morpheme rig to the flver rig
void FlverModel::createMorphemeToFlverBoneMap()
{
	this->m_morphemeToFlverBoneMap.reserve(this->m_nmRig->getNumBones());

	for (int idx = 0; idx < this->m_flver->header.boneCount; idx++)
	{
		this->m_morphemeToFlverBoneMap.push_back(-1);

		for (int i = 0; i < this->m_flverToMorphemeBoneMap.size(); i++)
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
	if (meshIdx > this->m_meshVerticesTransforms.size() || idx > this->m_meshVerticesTransforms[meshIdx].size())
		return nullptr;

	return &this->m_meshVerticesTransforms[meshIdx][idx];
}

FlverModel::SkinnedVertex* FlverModel::getVertexBindPose(int meshIdx, int idx)
{
	if (meshIdx > this->m_meshVerticesBindPoseTransforms.size() || idx > this->m_meshVerticesBindPoseTransforms[meshIdx].size())
		return nullptr;

	return &this->m_meshVerticesBindPoseTransforms[meshIdx][idx];
}

Matrix* FlverModel::getFlverBoneGlobalTransform(int idx)
{
	if (idx > this->m_boneTransforms.size())
		return nullptr;

	return &this->m_boneTransforms[idx];
}

Matrix* FlverModel::getFlverBoneBindPoseGlobalTransform(int idx)
{
	if (idx > this->m_boneBindPoseTransforms.size())
		return nullptr;

	return &this->m_boneBindPoseTransforms[idx];
}

Matrix* FlverModel::getFlverRootBoneGlobalTransform()
{
	int flverBoneIdx = this->getFlverBoneIndexByMorphemeBoneIndex(this->m_nmRig->getCharacterRootBoneIndex());

	return this->getFlverBoneGlobalTransform(flverBoneIdx);
}

Matrix* FlverModel::getFlverTrajectoryBoneGlobalTransform()
{
	int flverBoneIdx = this->getFlverBoneIndexByMorphemeBoneIndex(this->m_nmRig->getTrajectoryBoneIndex());

	return this->getFlverBoneGlobalTransform(flverBoneIdx);
}

Vector3 FlverModel::getBoundingBoxMin()
{
	return Vector3(this->m_flver->header.boundingBoxMin.x, this->m_flver->header.boundingBoxMin.y, this->m_flver->header.boundingBoxMin.z);
}

Vector3 FlverModel::getBoundingBoxMax()
{
	return Vector3(this->m_flver->header.boundingBoxMax.x, this->m_flver->header.boundingBoxMax.y, this->m_flver->header.boundingBoxMax.z);
}

Matrix* FlverModel::getMorphemeBoneGlobalTransform(int idx)
{
	if (idx > this->m_morphemeBoneTransforms.size())
		return nullptr;

	return &this->m_morphemeBoneBindPoseTransforms[idx];
}

Matrix* FlverModel::getMorphemeBoneBindPoseGlobalTransform(int idx)
{
	if (idx > this->m_morphemeBoneBindPoseTransforms.size())
		return nullptr;

	return &this->m_morphemeBoneBindPoseTransforms[idx];
}

Matrix* FlverModel::getMorphemeRootBoneGlobalTransform()
{
	return this->getMorphemeBoneGlobalTransform(this->m_nmRig->getCharacterRootBoneIndex());
}

Matrix* FlverModel::getMorphemeTrajectoryBoneGlobalTransform()
{
	return this->getMorphemeBoneGlobalTransform(this->m_nmRig->getTrajectoryBoneIndex());
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
	resetBoneTransformsToBindPose();

	if (animHandle)
	{
		computeAnimationTransformBuffers(animHandle);

		// Apply root motion
		this->m_position = getNmTrajectoryTransform(animHandle) * Matrix::CreateRotationY(DirectX::XM_PI);
	}

	// Compute the bones transform relative to their bind pose transform
	std::vector<Matrix> boneRelativeTransforms = computeBoneRelativeTransforms(animHandle);

	for (int meshIdx = 0; meshIdx < this->m_flver->header.meshCount; meshIdx++)
		transformMesh(meshIdx, boneRelativeTransforms);
}

void FlverModel::resetBoneTransformsToBindPose()
{
	//We initialise the final transforms to the flver bind pose so we can skip bones unhandled by morpheme in the next loop
	this->m_boneTransforms = this->m_boneBindPoseTransforms;
	this->m_morphemeBoneTransforms = this->m_morphemeBoneBindPoseTransforms;
	this->m_meshVerticesTransforms = this->m_meshVerticesBindPoseTransforms;
}

void FlverModel::computeAnimationTransformBuffers(MR::AnimationSourceHandle* animHandle)
{
	// Compute animation global transforms
	for (uint32_t i = 0; i < this->m_nmRig->getNumBones(); i++)
		this->m_morphemeBoneTransforms[i] = computeNmAnimGlobalTransform(animHandle, i);

	// Apply the morpheme rig transforms to the flver skeleton
	for (uint32_t i = 0; i < this->m_flver->header.boneCount; i++)
	{
		const int morphemeBoneID = this->m_flverToMorphemeBoneMap[i];

		if (morphemeBoneID != -1)
		{
			// Take the morpheme animation transform relative to the morpheme bind pose, align it to the flver bind pose, and then apply it to the flver bind pose.
			Matrix morphemeRelativeTransform = (this->m_morphemeInverseBoneBindPoseTransforms[morphemeBoneID] * this->m_morphemeBoneTransforms[morphemeBoneID]);

			applyTransform(this->m_boneTransforms, this->m_flver, this->m_boneBindPoseTransforms, (Matrix::CreateReflection(Plane(Vector3::Right)) * Matrix::CreateReflection(Plane(Vector3::Up)) * morphemeRelativeTransform), i);
		}
	}
}

std::vector<Matrix> FlverModel::computeBoneRelativeTransforms(MR::AnimationSourceHandle* animHandle)
{
	// Compute the bone transform relative to it's bind pose transform
	std::vector<Matrix> boneRelativeTransforms;
	boneRelativeTransforms.reserve(this->m_boneTransforms.size());

	for (int i = 0; i < this->m_boneTransforms.size(); i++)
		boneRelativeTransforms.push_back(this->m_boneInverseBindPoseTransforms[i] * this->m_boneTransforms[i]);

	return boneRelativeTransforms;
}

void FlverModel::transformMesh(int meshIdx, const std::vector<Matrix>& boneRelativeTransforms)
{
	for (int vertexIndex = 0; vertexIndex < this->m_meshVerticesBindPoseTransforms[meshIdx].size(); vertexIndex++)
		transformVertex(meshIdx, vertexIndex, boneRelativeTransforms);
}

void FlverModel::transformVertex(int meshIdx, int vertexIndex, const std::vector<Matrix>& boneRelativeTransforms)
{
	int* indices = this->m_meshVerticesBindPoseTransforms[meshIdx][vertexIndex].boneIndices;
	float* weights = this->m_meshVerticesBindPoseTransforms[meshIdx][vertexIndex].boneWeights;

	Vector3 newPos = Vector3::Zero;
	Vector3 newNorm = Vector3::Zero;
	bool hasInfluence = false;

	for (int wt = 0; wt < 4; wt++)
	{
		const int boneID = indices[wt];

		if (boneID != -1)
		{
			const int morphemeBoneID = this->getMorphemeBoneIdByFlverBoneId(boneID);
			const float weight = weights[wt];

			if ((morphemeBoneID != -1) && (weight != 0.f))
			{
				hasInfluence = true;
				newPos += Vector3::Transform(this->m_meshVerticesBindPoseTransforms[meshIdx][vertexIndex].vertexData.position, boneRelativeTransforms[boneID]) * weight;
				newNorm += Vector3::Transform(this->m_meshVerticesBindPoseTransforms[meshIdx][vertexIndex].vertexData.normal, boneRelativeTransforms[boneID]) * weight;
			}
		}
	}

	if (!hasInfluence)
	{
		g_appLog->debugMessage(MsgLevel_Debug, "Vertex %d of mesh %d has an invalid influence:\n", vertexIndex, meshIdx);
		g_appLog->debugMessage(MsgLevel_Debug, "\tIndices: (%d, %d, %d, %d)\n", indices[0], indices[1], indices[2], indices[3]);
		g_appLog->debugMessage(MsgLevel_Debug, "\tWeights: (%.3f, %.3f, %.3f, %.3f)\n", weights[0], weights[1], weights[2], weights[3]);
		g_appLog->panicMessage("Invalid bone influence data for %s (meshIdx=%d, vertexIdx=%d)\n", this->m_name.c_str(), meshIdx, vertexIndex);
	}

	this->m_meshVerticesTransforms[meshIdx][vertexIndex].vertexData.position = newPos;
	this->m_meshVerticesTransforms[meshIdx][vertexIndex].vertexData.normal = newNorm;
}