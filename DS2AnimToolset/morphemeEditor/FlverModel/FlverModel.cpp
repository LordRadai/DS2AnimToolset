#include "FlverModel.h"
#include "framework.h"
#include "extern.h"
#include "utils/NMDX/NMDX.h"
#include "RenderManager/RenderManager.h"
#include "RCore.h"
#include "morpheme/mrBlendOps.h"

#define MAX_BONE_WEIGHT_SANITIZATION_ITERATIONS 100

Matrix g_nmToYUpAdjustMatrix = Matrix::CreateRotationX(-DirectX::XM_PIDIV2);
Matrix g_flverToYUpAdjustMatrix = Matrix::CreateRotationY(DirectX::XM_PI);
Matrix g_flverBoneAdjustMatrix = g_flverToYUpAdjustMatrix * Matrix::CreateReflection(Plane(Vector3::Right));
Matrix g_flverMeshAdjustMatrix = g_flverToYUpAdjustMatrix * Matrix::CreateRotationX(DirectX::XM_PIDIV2);

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

	Matrix getAnimTrajectoryTransform(const MR::AnimationSourceHandle* animHandle)
	{
		NMP::Vector3 translation;
		NMP::Quat rotation;

		animHandle->getTrajectory(rotation, translation);

		return utils::NMDX::getTransformMatrix(rotation, translation);
	}

	Matrix getAnimTrajectoryAdjustedTransform(const MR::AnimationSourceHandle* animHandle)
	{
		Matrix trajZUp = getAnimTrajectoryTransform(animHandle);
		Matrix mConvertZUpToYUp = Matrix::CreateRotationX(DirectX::XM_PIDIV2);

		return mConvertZUpToYUp * trajZUp * mConvertZUpToYUp.Invert();
	}

	Matrix getNmRigTransform(const MR::AnimRigDef* rig, int idx)
	{
		return utils::NMDX::getTransformMatrix(*rig->getBindPoseBoneQuat(idx), *rig->getBindPoseBonePos(idx));
	}

	Matrix getNmAnimBoneTransform(const MR::AnimationSourceHandle* animHandle, int idx)
	{
		return utils::NMDX::getTransformMatrix(animHandle->getChannelData()[idx].m_quat, animHandle->getChannelData()[idx].m_pos);
	}

	Vector3 getCfrVector3(cfr::cfr_vec3& vec3)
	{
		return Vector3(vec3.x, vec3.y, vec3.z);
	}

	void accumulateNmTransforms(std::vector<Matrix>& dstChannel, const MR::AnimationSourceHandle* animHandle, bool applyRootMotion)
	{
		const MR::AnimRigDef* rig = animHandle->getRig();
		dstChannel.resize(rig->getNumBones());

		dstChannel[0] = g_nmToYUpAdjustMatrix;

		if (applyRootMotion)
			dstChannel[0] = getAnimTrajectoryTransform(animHandle) * g_nmToYUpAdjustMatrix;

		for (size_t i = 1; i < rig->getNumBones(); i++)
		{
			const int parentIndex = rig->getParentBoneIndex(i);

			if (parentIndex > i) throw std::runtime_error("Invalid bone hierarchy detected while accumulating Morpheme animation transforms.");

			dstChannel[i] = getNmAnimBoneTransform(animHandle, i) * dstChannel[parentIndex];
		}
	}

	void computeNmRigGlobalTransforms(std::vector<Matrix>& dstChannel, const MR::AnimRigDef* rig)
	{
		dstChannel.resize(rig->getNumBones());
		dstChannel[0] = getNmRigTransform(rig, 0) * g_nmToYUpAdjustMatrix;

		for (size_t i = 1; i < rig->getNumBones(); i++)
		{
			const int parentIndex = rig->getParentBoneIndex(i);

			if (parentIndex > i)
				throw std::runtime_error("Invalid bone hierarchy detected while computing Morpheme rig transforms.");

			dstChannel[i] = getNmRigTransform(rig, i) * dstChannel[parentIndex];
		}
	}

	Matrix getFlverBoneTransform(cfr::FLVER2* flver, int bone_id)
	{
		if (bone_id >= flver->header.boneCount)
			return Matrix::Identity;

		Matrix transform = Matrix::CreateScale(flver->bones[bone_id].scale.x, flver->bones[bone_id].scale.y, flver->bones[bone_id].scale.z);
		transform *= Matrix::CreateRotationX(flver->bones[bone_id].rot.x);
		transform *= Matrix::CreateRotationZ(flver->bones[bone_id].rot.z);
		transform *= Matrix::CreateRotationY(flver->bones[bone_id].rot.y);
		transform *= Matrix::CreateTranslation(flver->bones[bone_id].translation.x, flver->bones[bone_id].translation.y, flver->bones[bone_id].translation.z);

		return transform;
	}

	void computeFlvBonesGlobalTransform(std::vector<Matrix>& dstPose, FLVER2* flv)
	{
		dstPose.resize(flv->header.boneCount);

		for (size_t i = 0; i < flv->header.boneCount; i++)
		{
			FLVER2::Bone& bone = flv->bones[i];
			Matrix localTransform = getFlverBoneTransform(flv, i);
			int parentIdx = bone.parentIndex;

			while (parentIdx != -1)
			{
				Matrix parentBoneTransform = getFlverBoneTransform(flv, parentIdx);
				localTransform *= parentBoneTransform;

				parentIdx = flv->bones[parentIdx].parentIndex;
			}

			dstPose[i] = localTransform * g_flverBoneAdjustMatrix;
		}
	}

	void applyTransform(std::vector<Matrix>& buffer, FLVER2* flv, std::vector<Matrix>& bindPose, const Matrix& transform, int boneID)
	{
		// Compute this bone’s world transform relative to parent
		Matrix local = bindPose[boneID];
		Matrix world = local * transform;

		buffer[boneID] = world;

		int siblingIndex = flv->bones[boneID].nextSiblingIndex;

		while (siblingIndex != -1)
		{
			applyTransform(buffer, flv, bindPose, transform, siblingIndex);

			siblingIndex = flv->bones[siblingIndex].nextSiblingIndex;
		}

		int childIndex = flv->bones[boneID].childIndex;

		while (childIndex != -1)
		{
			applyTransform(buffer, flv, bindPose, transform, childIndex);
			childIndex = flv->bones[childIndex].nextSiblingIndex;
		}
	}

	void applyTwistTransform(std::vector<Matrix>& buffer, FLVER2* flv, std::vector<Matrix>& bindPose, const Matrix& transform, int boneID)
	{
		// Compute this bone’s world transform relative to parent
		Matrix local = bindPose[boneID];
		Matrix world = local * transform;

		buffer[boneID] = world;

		int childIndex = flv->bones[boneID].childIndex;

		while (childIndex != -1)
		{
			applyTwistTransform(buffer, flv, bindPose, transform, childIndex);

			int siblingIndex = flv->bones[childIndex].nextSiblingIndex;

			while (siblingIndex != -1)
			{
				applyTwistTransform(buffer, flv, bindPose, transform, siblingIndex);

				siblingIndex = flv->bones[siblingIndex].nextSiblingIndex;
			}

			childIndex = flv->bones[childIndex].nextSiblingIndex;
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

	this->m_settings.drawBoneInfluences = false;

	g_appLog->debugMessage(MsgLevel_Info, "Creating bone maps:\n");

	this->createFlverToMorphemeBoneMap();
	this->createMorphemeToFlverBoneMap();

	if (!this->initialise())
		INVOKE_PANIC("Flver model initialisation failed");
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
std::vector<Vector3> FlverModel::getFlverMeshVertices(int idx)
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
			float y = mesh->vertexData->positions[(vertexIndex * 3) + 1];
			float z = mesh->vertexData->positions[(vertexIndex * 3) + 2];

			vertices.push_back(Vector3::Transform(Vector3(x, y, z), g_flverMeshAdjustMatrix));
		}
	}

	return vertices;
}

// Gets the normals for the FLVER mesh at index idx 
std::vector<Vector3> FlverModel::getFlverMeshNormals(int idx)
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
			float y = mesh->vertexData->normals[(vertexIndex * 3) + 1];
			float z = mesh->vertexData->normals[(vertexIndex * 3) + 2];

			normals.push_back(Vector3::Transform(Vector3(x, y, z), g_flverMeshAdjustMatrix));
		}
	}

	return normals;
}

// Gets the tangents for the FLVER mesh at index idx 
// WARNING! This data seems to be wrong, I do not advise you to inclide these when exporting)
std::vector<Vector3> FlverModel::getFlverMeshTangents(int idx)
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
			float y = mesh->vertexData->tangents[(vertexIndex * 3) + 1];
			float z = mesh->vertexData->tangents[(vertexIndex * 3) + 2];

			tangents.push_back(Vector3::Transform(Vector3(x, y, z), g_flverMeshAdjustMatrix));
		}
	}

	return tangents;
}

// Gets the bitangents for the FLVER mesh at index idx 
// WARNING! This data seems to be wrong, I do not advise you to inclide these when exporting)
std::vector<Vector3> FlverModel::getFlverMeshBiTangents(int idx)
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
			float y = mesh->vertexData->bitangents[(vertexIndex * 3) + 1];
			float z = mesh->vertexData->bitangents[(vertexIndex * 3) + 2];

			bitangents.push_back(Vector3::Transform(Vector3(x, y, z), g_flverMeshAdjustMatrix));
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

void FlverModel::normalizeSkinVertexData(FlverModel::SkinnedVertex& skinnedVertex)
{
	float totalWeight = 0.f;
	for (size_t wt = 0; wt < 4; ++wt)
		totalWeight += skinnedVertex.boneWeights[wt];

	bool bValid = false;

	for (size_t wt = 0; wt < 4; ++wt)
	{
		skinnedVertex.boneWeights[wt] /= totalWeight;

		if (skinnedVertex.boneIndices[wt] && skinnedVertex.boneWeights[wt] > 0.f)
			bValid = true;
	}

	if (!bValid)
		g_appLog->alertMessage(MsgLevel_Error, "Warning: Vertex with no valid bone influences detected!\n");
}

// Gets all the model vertices for all the meshes and stores them into m_verts
bool FlverModel::initialise()
{
	if (this->m_flver == nullptr || this->m_nmRig == nullptr)
		return false;

	computeNmRigGlobalTransforms(this->m_nmBindPoseTransforms, this->m_nmRig);
	this->m_nmBoneTransforms = this->m_nmBindPoseTransforms;

	this->m_nmInverseBoneBindPoseTransforms.reserve(this->m_nmBindPoseTransforms.size());
	for (size_t i = 0; i < this->m_nmBindPoseTransforms.size(); i++)
		this->m_nmInverseBoneBindPoseTransforms.push_back(this->m_nmBindPoseTransforms[i].Invert());

	computeFlvBonesGlobalTransform(this->m_flverBindPoseTransforms, this->m_flver);
	this->m_flverBoneTransforms = this->m_flverBindPoseTransforms;

	this->m_flverInverseBindPoseTransforms.reserve(this->m_flverBindPoseTransforms.size());
	for (size_t i = 0; i < this->m_flverBindPoseTransforms.size(); i++)
		this->m_flverInverseBindPoseTransforms.push_back(this->m_flverBindPoseTransforms[i].Invert());

	this->m_meshVerticesTransforms.reserve(this->m_flver->header.meshCount);
	this->m_meshVerticesBindPoseTransforms.reserve(this->m_flver->header.meshCount);

	for (int i = 0; i < this->m_flver->header.meshCount; i++)
	{
		std::vector<Vector3> vertices = this->getFlverMeshVertices(i);
		std::vector<Vector3> normals = this->getFlverMeshNormals(i);
		std::vector<Vector4> boneWeights = this->getFlverMeshBoneWeights(i);
		std::vector<std::vector<int>> boneIndices = this->getFlverMeshBoneIndices(i);

		std::vector<SkinnedVertex> meshSkinnedVertices;

		for (size_t i = 0; i < vertices.size(); i++)
		{
			meshSkinnedVertices.push_back(SkinnedVertex(vertices[i], normals[i], (float*)&boneWeights[i], boneIndices[i].data()));
			normalizeSkinVertexData(meshSkinnedVertices.back());
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

	this->m_focusPoint = Vector3::Transform(Vector3::Zero, this->getWorldMatrix());

	// Apply the morpheme rig transforms to the flver skeleton
	for (uint32_t i = 0; i < this->m_flver->header.boneCount; i++)
	{
		const int morphemeBoneID = this->m_flverToMorphemeBoneMap[i];

		if (morphemeBoneID != -1)
		{
			const int parentMorphemeBoneID = this->m_nmRig->getParentBoneIndex(morphemeBoneID);
			// Take the morpheme animation transform relative to the morpheme bind pose, align it to the flver bind pose, and then apply it to the flver bind pose.
			//Matrix morphemeRelativeTransform = getNmBoneRelativeTransform(morphemeBoneID);

			applyTransform(this->m_flverBoneTransforms, this->m_flver, this->m_flverBindPoseTransforms, getNmBoneRelativeTransform(morphemeBoneID), i);
		}
	}

	// Compute the bones transform relative to their bind pose transform
	std::vector<Matrix> boneRelativeTransforms;
	computeBoneRelativeTransforms(boneRelativeTransforms);

	for (int meshIdx = 0; meshIdx < this->m_flver->header.meshCount; meshIdx++)
		transformMesh(meshIdx, boneRelativeTransforms);

	this->m_dummyPolygons.clear();
	this->m_dummyPolygons.reserve(this->m_flver->header.dummyCount);
	for (size_t i = 0; i < this->m_flver->header.dummyCount; i++)
	{
		Vector3 dummyPos(this->m_flver->dummies[i].position.x, this->m_flver->dummies[i].position.y, this->m_flver->dummies[i].position.z);
		Vector3 dummyUp(this->m_flver->dummies[i].upward.x, this->m_flver->dummies[i].upward.y, this->m_flver->dummies[i].upward.z);
		Vector3 dummyForward(this->m_flver->dummies[i].forward.x, this->m_flver->dummies[i].forward.y, this->m_flver->dummies[i].forward.z);

		Matrix dummyLocalTransform = Matrix::CreateWorld(dummyPos, dummyUp, dummyForward);

		this->m_dummyPolygons.push_back(dummyLocalTransform * this->m_flverBoneTransforms[this->m_flver->dummies[i].dummyBoneIndex]);
	}
}

//Draws the character
void FlverModel::draw(RenderManager* renderManager)
{
	Matrix world = this->getWorldMatrix();

	renderManager->applyDebugEffect(Matrix::Identity);
	renderManager->setInputLayout(kDebugLayout);

	DirectX::PrimitiveBatch<DirectX::VertexPositionColor> prim(renderManager->getDeviceContext(), UINT16_MAX * 3, UINT16_MAX);
	prim.Begin();

	int boneCount = this->m_flverBoneTransforms.size();

	int trajectoryBoneIndex = getFlverTrajectoryBoneIndex();
	int characterRootBoneIdx = getFlverRootBoneIndex();

	if (this->m_settings.selectedBone != -1)
	{
		DX::DrawReferenceFrame(&prim, getFlverBoneGlobalTransform(this->m_settings.selectedBone), 0.1f);
		renderManager->addText(RString::toNarrow(this->m_flver->bones[this->m_settings.selectedBone].name).c_str(), getFlverBoneGlobalTransform(this->m_settings.selectedBone));
	}

	if (this->m_settings.drawDummyPolygons && this->m_flver)
	{
		for (size_t i = 0; i < this->m_flver->header.dummyCount; i++)
		{
			std::string dummy_name = "Dmy_" + std::to_string(this->m_flver->dummies[i].referenceID);

			DX::DrawReferenceFrame(&prim, getDummyPolygonTransform(i), 0.1f);
			renderManager->addText(dummy_name.c_str(), getDummyPolygonTransform(i));
		}
	}
	else if (this->m_settings.selectedDummy != -1)
	{
		std::string dummy_name = "Dmy_" + std::to_string(this->m_flver->dummies[this->m_settings.selectedDummy].referenceID);

		DX::DrawReferenceFrame(&prim, getDummyPolygonTransform(this->m_settings.selectedDummy), 0.1f);
		renderManager->addText(dummy_name.c_str(), getDummyPolygonTransform(this->m_settings.selectedDummy));
	}

	if (this->m_settings.drawBones && this->m_flver)
		drawFlverBones(renderManager, prim);

	if (this->m_settings.drawMorphemeBones && this->m_nmRig)
		drawMorphemeBones(renderManager, prim);

	if (this->m_settings.drawModelPosition)
		DX::DrawReferenceFrame(&prim, world, 0.3f);

	if (this->m_settings.drawBoundingBox)
	{
		Vector3 halfExtents = (this->getBoundingBoxMax() - this->getBoundingBoxMin()) / 2;

		DX::DrawBoundingBox(&prim, world, Vector3::Zero, halfExtents, DirectX::Colors::DarkRed);
	}

	if (this->m_settings.drawBoneInfluences && this->m_settings.drawMeshes)
	{
		for (int meshIdx = 0; meshIdx < this->m_flver->header.meshCount; meshIdx++)
		{
			for (size_t vtxIdx = 0; vtxIdx < this->m_meshVerticesTransforms[meshIdx].size(); vtxIdx++)
			{
				FlverModel::SkinnedVertex* skinnedVtx = this->getVertex(meshIdx, vtxIdx);
				Vector3 vertexPos = Vector3::Transform(skinnedVtx->vertexData.position, world);
				for (size_t bi = 0; bi < 4; bi++)
				{
					int flverBoneIdx = skinnedVtx->boneIndices[bi];
					float weight = skinnedVtx->boneWeights[bi];

					if (weight > 0.f && flverBoneIdx >= 0 && flverBoneIdx < boneCount)
					{
						Vector3 bonePos = Vector3::Transform(Vector3::Zero, getFlverBoneGlobalTransform(flverBoneIdx));
						Vector4 color = Vector4(DirectX::Colors::LimeGreen);

						if (flverBoneIdx == trajectoryBoneIndex)
							color = DirectX::Colors::Yellow;
						else if (flverBoneIdx == characterRootBoneIdx)
							color = DirectX::Colors::Cyan;

						color.w *= weight;

						DX::DrawLine(&prim, vertexPos, bonePos, color);
					}
				}
			}
		}
	}

	if (this->m_settings.highlight)
	{
		renderManager->addText(this->getModelName(), world);

		if (this->m_settings.displayMode != kDispWireframe)
			DX::DrawModelWireframe(&prim, world, this, Vector4(DirectX::Colors::White));
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

			renderManager->applyPhysicalEffect(Matrix::Identity, alpha);
			renderManager->setInputLayout(kPhysicalLayout);

			primShaded.Begin();
			DX::DrawModel(&primShaded, world, this);
			primShaded.End();
		}
		else
		{
			prim.Begin();
			DX::DrawModelWireframe(&prim, world, this, Vector4(DirectX::Colors::White));
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
			return this->m_dummyPolygons[i] * this->getWorldMatrix();
	}

	g_appLog->debugMessage(MsgLevel_Error, "Could not find dummy polygon %d (%s)\n", id, this->m_name);

	return this->getWorldMatrix();
}

FlverModel::SkinnedVertex* FlverModel::getVertex(int meshIdx, int idx)
{
	if (meshIdx < 0) return nullptr;
	if (static_cast<size_t>(meshIdx) >= this->m_meshVerticesTransforms.size()) return nullptr;
	if (idx < 0) return nullptr;
	if (static_cast<size_t>(idx) >= this->m_meshVerticesTransforms[meshIdx].size()) return nullptr;

	return &this->m_meshVerticesTransforms[meshIdx][idx];
}

FlverModel::SkinnedVertex* FlverModel::getVertexBindPose(int meshIdx, int idx)
{
	if (meshIdx > this->m_meshVerticesBindPoseTransforms.size() || idx > this->m_meshVerticesBindPoseTransforms[meshIdx].size())
		return nullptr;

	return &this->m_meshVerticesBindPoseTransforms[meshIdx][idx];
}

Matrix FlverModel::getFlverBoneGlobalTransform(int idx)
{
	if (idx > this->m_flverBoneTransforms.size())
		throw std::out_of_range("FlverModel::getFlverBoneGlobalTransform: index out of range");

	return this->m_flverBoneTransforms[idx] * this->getWorldMatrix();
}

Matrix FlverModel::getFlverBoneBindPoseGlobalTransform(int idx)
{
	if (idx > this->m_flverBindPoseTransforms.size())
		throw std::out_of_range("FlverModel::getFlverBoneBindPoseGlobalTransform: index out of range");

	return this->m_flverBindPoseTransforms[idx] * this->getWorldMatrix();
}

int FlverModel::getMorphemeTrajectoryBoneIndex()
{
	return this->m_nmRig->getTrajectoryBoneIndex();
}

int FlverModel::getMorphemeRootBoneIndex()
{
	return this->m_nmRig->getCharacterRootBoneIndex();
}

int FlverModel::getFlverTrajectoryBoneIndex()
{
	return this->getFlverBoneIndexByMorphemeBoneIndex(this->m_nmRig->getTrajectoryBoneIndex());
}

int FlverModel::getFlverRootBoneIndex()
{
	return this->getFlverBoneIndexByMorphemeBoneIndex(this->m_nmRig->getCharacterRootBoneIndex());
}

Matrix FlverModel::getFlverRootBoneGlobalTransform()
{
	const int flverBoneIdx = getFlverRootBoneIndex();

	return this->getFlverBoneGlobalTransform(flverBoneIdx);
}

Matrix FlverModel::getFlverTrajectoryBoneGlobalTransform()
{
	const int flverBoneIdx = getFlverTrajectoryBoneIndex();

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

Matrix FlverModel::getMorphemeBoneGlobalTransform(int idx)
{
	if (idx > this->m_nmBoneTransforms.size())
		throw std::out_of_range("FlverModel::getMorphemeBoneGlobalTransform: index out of range");

	return this->m_nmBoneTransforms[idx] * this->getWorldMatrix();
}

Matrix FlverModel::getMorphemeBoneBindPoseGlobalTransform(int idx)
{
	if (idx > this->m_nmBindPoseTransforms.size())
		throw std::out_of_range("FlverModel::getMorphemeBoneBindPoseGlobalTransform: index out of range");

	return this->m_nmBindPoseTransforms[idx] * this->getWorldMatrix();
}

Matrix FlverModel::getMorphemeRootBoneGlobalTransform()
{
	return this->getMorphemeBoneGlobalTransform(this->m_nmRig->getCharacterRootBoneIndex());
}

Matrix FlverModel::getMorphemeTrajectoryBoneGlobalTransform()
{
	return this->getMorphemeBoneGlobalTransform(this->m_nmRig->getTrajectoryBoneIndex());
}

Matrix FlverModel::getNmBoneRelativeTransform(int idx)
{
	if (idx < this->m_nmBoneTransforms.size())
		return this->m_nmInverseBoneBindPoseTransforms[idx] * this->m_nmBoneTransforms[idx];

	return Matrix::Identity;
}

Matrix FlverModel::getFlverBoneRelativeTransform(int idx)
{
	if (idx < this->m_flverBoneTransforms.size())
		return this->m_flverInverseBindPoseTransforms[idx] * this->m_flverBoneTransforms[idx];

	return Matrix::Identity;
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

void FlverModel::animate(AnimObject* anim)
{
	if (anim == nullptr)
		return;

	MR::AnimationSourceHandle* animHandle = anim->getHandle();

	if (animHandle)
	{
		computeAnimationTransforms(animHandle);

		if (this->m_settings.enableRootMotion)
			this->m_position = getAnimTrajectoryAdjustedTransform(animHandle);
		else
			this->m_position = Matrix::Identity;
	}
}

void FlverModel::resetBoneTransforms()
{
	this->m_flverBoneTransforms = this->m_flverBindPoseTransforms;
	this->m_nmBoneTransforms = this->m_nmBindPoseTransforms;
	this->m_meshVerticesTransforms = this->m_meshVerticesBindPoseTransforms;
}

void FlverModel::computeAnimationTransforms(MR::AnimationSourceHandle* animHandle)
{
	accumulateNmTransforms(this->m_nmBoneTransforms, animHandle, false);
}

void FlverModel::computeBoneRelativeTransforms(std::vector<Matrix>& out)
{
	out.clear();
	out.reserve(this->m_flverBoneTransforms.size());

	for (int i = 0; i < this->m_flverBoneTransforms.size(); i++)
		out.push_back(getFlverBoneRelativeTransform(i));
}

void FlverModel::transformMesh(int meshIdx, const std::vector<Matrix>& boneRelativeTransforms)
{
	for (int vertexIndex = 0; vertexIndex < this->m_meshVerticesBindPoseTransforms[meshIdx].size(); vertexIndex++)
		transformVertex(meshIdx, vertexIndex, boneRelativeTransforms);
}

void FlverModel::transformVertex(int meshIdx, int vertexIndex, const std::vector<Matrix>& boneRelativeTransforms)
{
    const auto& bindVertex = this->m_meshVerticesBindPoseTransforms[meshIdx][vertexIndex];
    int constIndices[4];
    float constWeights[4];
    std::copy(std::begin(bindVertex.boneIndices), std::end(bindVertex.boneIndices), std::begin(constIndices));
    std::copy(std::begin(bindVertex.boneWeights), std::end(bindVertex.boneWeights), std::begin(constWeights));

    Vector3 newPos = Vector3::Zero;
    Vector3 newNorm = Vector3::Zero;
    bool hasInfluence = false;

    for (int wt = 0; wt < 4; ++wt)
    {
        const int boneID = constIndices[wt];
        if (boneID == -1) continue;

        const float weight = constWeights[wt];
        if (weight == 0.f) continue;

        hasInfluence = true;
        newPos += Vector3::Transform(bindVertex.vertexData.position, boneRelativeTransforms[boneID]) * weight;
        newNorm += Vector3::Transform(bindVertex.vertexData.normal, boneRelativeTransforms[boneID]) * weight;
    }

    if (!hasInfluence)
        g_appLog->debugMessage(MsgLevel_Debug, "Vertex %d of mesh %d has an invalid influence\n", vertexIndex, meshIdx);

    this->m_meshVerticesTransforms[meshIdx][vertexIndex].vertexData.position = newPos;
    this->m_meshVerticesTransforms[meshIdx][vertexIndex].vertexData.normal = newNorm;
}

void FlverModel::drawFlverBones(RenderManager* renderManager, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& prim)
{
	Matrix world = this->getWorldMatrix();

	const Vector4 boneMarkerColor = RMath::getFloatColor(IM_COL32(51, 102, 255, 255));
	const Vector4 rootBoneMarkerColor = Vector4(DirectX::Colors::Orange);
	const Vector4 trajectoryBoneMarkerColor = Vector4(DirectX::Colors::Red);

	const int trajectoryBoneIndex = this->getFlverBoneIndexByMorphemeBoneIndex(this->m_nmRig->getTrajectoryBoneIndex());
	const int characterRootBoneIdx = this->getFlverBoneIndexByMorphemeBoneIndex(this->m_nmRig->getCharacterRootBoneIndex());

	for (int boneIdx = 0; boneIdx < this->m_flver->header.boneCount; boneIdx++)
	{
		if ((boneIdx == trajectoryBoneIndex) || (boneIdx == characterRootBoneIdx))
			continue;

		int parentIndex = this->m_flver->bones[boneIdx].parentIndex;

		if (parentIndex != -1)
		{
			Vector3 boneA = Vector3::Transform(Vector3::Zero, getFlverBoneGlobalTransform(boneIdx));
			Vector3 boneB = Vector3::Transform(Vector3::Zero, getFlverBoneGlobalTransform(parentIndex));

			Vector4 jointColor = boneMarkerColor;

			if (parentIndex == getFlverRootBoneIndex())
				jointColor = rootBoneMarkerColor;

			DX::DrawJoint(&prim, Matrix::Identity, boneB, boneA, jointColor);

			if (this->m_flver->bones[boneIdx].childIndex == -1)
				DX::Draw(&prim, DirectX::BoundingSphere(boneA, 0.03f), jointColor);
		}
	}

	DX::Draw(&prim, DirectX::BoundingSphere(Vector3::Transform(Vector3::Zero, getFlverRootBoneGlobalTransform()), 0.03f), rootBoneMarkerColor);
	DX::Draw(&prim, DirectX::BoundingSphere(Vector3::Transform(Vector3::Zero, getFlverTrajectoryBoneGlobalTransform()), 0.03f), trajectoryBoneMarkerColor);
}

void FlverModel::drawMorphemeBones(RenderManager* renderManager, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& prim)
{
	Matrix world = this->getWorldMatrix();

	const Vector4 boneMarkerColor = RMath::getFloatColor(IM_COL32(251, 84, 43, 255));

	const int trajectoryBoneIndex = this->m_nmRig->getTrajectoryBoneIndex();
	const int characterRootBoneIdx = this->m_nmRig->getCharacterRootBoneIndex();

	for (int boneIdx = 0; boneIdx < this->m_nmRig->getNumBones(); boneIdx++)
	{
		if ((boneIdx == trajectoryBoneIndex) || (boneIdx == characterRootBoneIdx))
			continue;

		int parentIndex = this->m_nmRig->getParentBoneIndex(boneIdx);

		if (parentIndex != -1)
		{
			Vector3 boneA = Vector3::Transform(Vector3::Zero, getMorphemeBoneGlobalTransform(boneIdx));
			Vector3 boneB = Vector3::Transform(Vector3::Zero, getMorphemeBoneGlobalTransform(parentIndex));

			DX::DrawLine(&prim, boneB, boneA, boneMarkerColor);
		}
	}

	DX::Draw(&prim, DirectX::BoundingSphere(Vector3::Transform(Vector3::Zero, getMorphemeRootBoneGlobalTransform()), 0.03f), DirectX::Colors::Orange);
	DX::Draw(&prim, DirectX::BoundingSphere(Vector3::Transform(Vector3::Zero, getMorphemeTrajectoryBoneGlobalTransform()), 0.03f), DirectX::Colors::Red);
}