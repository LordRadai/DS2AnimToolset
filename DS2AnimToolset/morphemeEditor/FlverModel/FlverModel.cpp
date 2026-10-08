#include <algorithm>
#include <functional>

#include "FlverModel.h"
#include "framework.h"
#include "extern.h"
#include "utils/NMDX/NMDX.h"
#include "RenderManager/RenderManager.h"

Matrix g_nmToYUpAdjustMatrix = Matrix::CreateRotationX(-DirectX::XM_PIDIV2);
Matrix g_invertedNmToYUpAdjustMatrix = g_nmToYUpAdjustMatrix.Invert();
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

			if (parentIndex > i) INVOKE_PANIC("Invalid bone hierarchy detected while accumulating Morpheme animation transforms.");

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
				INVOKE_PANIC("Invalid bone hierarchy detected while computing Morpheme rig transforms.");

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

	Matrix getRotationMatrix(Matrix transform)
	{
		Vector3 scale;
		Quaternion rotation;
		Vector3 translation;

		transform.Decompose(scale, rotation, translation);

		return Matrix::CreateFromQuaternion(rotation);
	}

	// Scales a rotation by a factor. If oneAxis is set, only the twist around the given (unit) axis is kept.
	Matrix scaleTwistRotation(const Matrix& rotation, float scale, bool oneAxis, const Vector3& twistAxis)
	{
		Quaternion q = Quaternion::CreateFromRotationMatrix(rotation);

		// Take the shortest arc so the scaled rotation does not flip around
		if (q.w < 0.f)
			q = -q;

		if (oneAxis)
		{
			// Swing-twist decomposition: the twist is the quaternion's vector part projected on the axis, together with w
			const float twistAngle = 2.f * std::atan2(Vector3(q.x, q.y, q.z).Dot(twistAxis), q.w);

			return Matrix::CreateFromAxisAngle(twistAxis, twistAngle * scale);
		}

		return Matrix::CreateFromQuaternion(Quaternion::Slerp(Quaternion::Identity, q, scale));
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

FlverModel::FlverModel(UMEM* umem, MR::AnimRigDef* rig, const ChrModelExFormat::ChrModelExFormat* exFormat)
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
	this->createFlverToMorphemeSkinningBoneMap();

	if (!this->initialise())
		INVOKE_PANIC("Flver model initialisation failed");

	// Needs the flver bind pose computed by initialise()
	this->createFlverTwistBones(exFormat);
	this->createFlverBoneEvaluationOrder();
}

FlverModel::FlverModel(MR::AnimRigDef* rig)
{
	this->m_loaded = false;
	this->m_meshVerticesTransforms.clear();
	this->m_position = Matrix::Identity;
	this->m_flver = nullptr;
	this->m_nmRig = rig;
	this->m_settings.drawBoneInfluences = false;

	if (!this->initialise())
		INVOKE_PANIC("Flver model initialisation failed");
}

FlverModel* FlverModel::createFromBnd(std::wstring path, MR::AnimRigDef* rig)
{
	FlverModel* model = nullptr;

	BND4::Bnd4* bnd = BND4::Bnd4::loadFromFile(path);

	if (bnd == nullptr)
		return nullptr;

	BND4::BndFile* flverFile = bnd->getFirstFileWithExtension(".flv");

	if (flverFile)
	{
		g_appLog->debugMessage(MsgLevel_Debug, "Loading model \"%ws\"\n", path.c_str());

		UMEM* umem = uopenMem((char*)flverFile->data, flverFile->uncompressedSize);

		ChrModelExFormat::ChrModelExFormat* exFormat = nullptr;
		BND4::BndFile* exFormatFile = bnd->getFirstFileWithExtension(".flvpwv");

		if (exFormatFile && exFormatFile->data)
			exFormat = ChrModelExFormat::ChrModelExFormat::createFromResource(reinterpret_cast<ChrModelExFormat::FLVPWV::Header*>(exFormatFile->data));
		else
			g_appLog->debugMessage(MsgLevel_Warn, "No twist bone file in \"%ws\", twist bones will follow their parent\n", path.c_str());

		model = new FlverModel(umem, rig, exFormat);

		if (exFormat)
			exFormat->destroy();

		model->m_name = std::filesystem::path(path).filename().replace_extension("").string();
		model->m_fileOrigin = path + L"\\" + RString::toWide(flverFile->name.c_str());
	}
	else
		g_appLog->debugMessage(MsgLevel_Error, "Could not find a .flver file inside \"%ws\"\n", path);

	bnd->destroy();

	delete bnd;

	return model;
}

FlverModel* FlverModel::createFromAnimRig(MR::AnimRigDef* rig)
{
	return new FlverModel(rig);
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

	// No weights at all: the vertex is bound rigidly to its first bone. Dividing by the zero total would write NaN
	// weights, which make the exported XMD unusable (morphemeConnect's anim.createRig rejects it).
	if (!(totalWeight > 0.f))
	{
		for (size_t wt = 0; wt < 4; ++wt)
			skinnedVertex.boneWeights[wt] = (wt == 0) ? 1.f : 0.f;

		totalWeight = 1.f;
	}

	bool bValid = false;

	for (size_t wt = 0; wt < 4; ++wt)
	{
		skinnedVertex.boneWeights[wt] /= totalWeight;

		if (skinnedVertex.boneIndices[wt] && skinnedVertex.boneWeights[wt] > 0.f)
			bValid = true;
	}

	if (!bValid)
		g_appLog->debugMessage(MsgLevel_Warn, "Warning: Vertex with no valid bone influences detected!\n");
}

// Gets all the model vertices for all the meshes and stores them into m_verts
bool FlverModel::initialise()
{
	if (this->m_nmRig == nullptr)
		return false;

	computeNmRigGlobalTransforms(this->m_nmBindPoseTransforms, this->m_nmRig);
	this->m_nmBoneTransforms = this->m_nmBindPoseTransforms;

	this->m_nmInverseBoneBindPoseTransforms.reserve(this->m_nmBindPoseTransforms.size());
	for (size_t i = 0; i < this->m_nmBindPoseTransforms.size(); i++)
		this->m_nmInverseBoneBindPoseTransforms.push_back(this->m_nmBindPoseTransforms[i].Invert());

	if (this->m_flver)
	{
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
	}

	if (this->m_scale < 1.5f)
		this->m_scale = 1.5f;

	return true;
}

void FlverModel::update(float dt)
{
	if (this->m_flver == nullptr)
		return;

	this->m_focusPoint = Vector3::Transform(Vector3::Zero, this->getWorldMatrix());

	// Apply the morpheme rig transforms to the flver skeleton. Parents are always evaluated before their children.
	for (const int i : this->m_flverBoneEvaluationOrder)
	{
		const int morphemeBoneID = this->m_flverToMorphemeBoneMap[i];

		if (morphemeBoneID != -1)
		{
			// Take the morpheme animation transform relative to the morpheme bind pose and apply it to the flver bind pose.
			this->m_flverBoneTransforms[i] = this->m_flverBindPoseTransforms[i] * getNmBoneRelativeTransform(morphemeBoneID);
		}
		else if (this->isFlverTwistBone(i))
		{
			// Twist bones are driven by the FLVPWV settings: they follow their base bone and add part of the rotation of another bone
			this->m_flverBoneTransforms[i] = this->computeFlverTwistBoneTransform(i);
		}
		else
		{
			// Other bones morpheme does not animate keep their flver bind pose local transform and follow their parent.
			const int parentIdx = this->getFlverBoneParentIndex(i);

			if (parentIdx != -1)
				this->m_flverBoneTransforms[i] = this->m_flverBindPoseTransforms[i] * this->m_flverInverseBindPoseTransforms[parentIdx] * this->m_flverBoneTransforms[parentIdx];
			else
				this->m_flverBoneTransforms[i] = this->m_flverBindPoseTransforms[i];
		}
	}

	if (this->m_settings.drawMeshes)
	{
		// Compute the bones transform relative to their bind pose transform
		std::vector<Matrix> boneRelativeTransforms;
		computeBoneRelativeTransforms(boneRelativeTransforms);

		for (int meshIdx = 0; meshIdx < this->m_flver->header.meshCount; meshIdx++)
			transformMesh(meshIdx, boneRelativeTransforms);
	}

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

void FlverModel::setTransforms(NMP::DataBuffer* transforms)
{
	Matrix mConvertZUpToYUp = g_nmToYUpAdjustMatrix;
	Matrix mInvertZUpToYUp = g_invertedNmToYUpAdjustMatrix;

	for (size_t i = 0; i < this->m_nmBoneTransforms.size(); i++)
	{
		NMP::Vector3 translation = *transforms->getChannelPos(i);
		NMP::Quat rotation = *transforms->getChannelQuat(i);

		Matrix transform = utils::NMDX::getTransformMatrix(rotation, translation);

		this->m_nmBoneTransforms[i] = transform * mConvertZUpToYUp;
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
		DX::DrawReferenceFrame(&prim, Matrix::Identity, 0.3f);

	if (this->m_settings.drawBoundingBox)
	{
		Vector3 halfExtents = (this->getBoundingBoxMax() - this->getBoundingBoxMin()) / 2;

		DX::DrawBoundingBox(&prim, Matrix::Identity, Vector3::Zero, halfExtents, DirectX::Colors::DarkRed);
	}

	if (this->m_settings.drawBoneInfluences && this->m_settings.drawMeshes)
	{
		for (int meshIdx = 0; meshIdx < this->m_flver->header.meshCount; meshIdx++)
		{
			for (size_t vtxIdx = 0; vtxIdx < this->m_meshVerticesTransforms[meshIdx].size(); vtxIdx++)
			{
				FlverModel::SkinnedVertex* skinnedVtx = this->getVertex(meshIdx, vtxIdx);
				Vector3 vertexPos = Vector3::Transform(skinnedVtx->vertexData.position, Matrix::Identity);
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
		renderManager->addText(this->getModelName(), Matrix::Identity);

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

	for (int idx = 0; idx < this->m_nmRig->getNumBones(); idx++)
	{
		this->m_morphemeToFlverBoneMap.push_back(-1);

		for (int i = 0; i < this->m_flverToMorphemeBoneMap.size(); i++)
		{
			if (this->m_flverToMorphemeBoneMap[i] == idx)
				this->m_morphemeToFlverBoneMap.back() = i;
		}
	}
}

// Returns the flver parent of the given bone, or -1 if it has none or the index is invalid
int FlverModel::getFlverBoneParentIndex(int idx)
{
	if ((idx < 0) || (idx >= this->m_flver->header.boneCount))
		return -1;

	const int parentIdx = this->m_flver->bones[idx].parentIndex;

	if ((parentIdx < 0) || (parentIdx >= this->m_flver->header.boneCount))
		return -1;

	return parentIdx;
}

//Maps each flver bone to the morpheme bone that drives it: the bone with the same name, or the one of its nearest flver ancestor 
//that exists in the morpheme rig. Used to move the weights of bones morpheme does not animate (twist bones, etc.) to a bone it does.
void FlverModel::createFlverToMorphemeSkinningBoneMap()
{
	const int boneCount = this->m_flver->header.boneCount;

	this->m_flverToMorphemeSkinningBoneMap.assign(boneCount, -1);

	for (int i = 0; i < boneCount; i++)
	{
		int boneIdx = i;

		// Bounded walk so a malformed hierarchy cannot loop forever
		for (int depth = 0; (boneIdx != -1) && (depth < boneCount); depth++)
		{
			if (this->m_flverToMorphemeBoneMap[boneIdx] != -1)
			{
				this->m_flverToMorphemeSkinningBoneMap[i] = this->m_flverToMorphemeBoneMap[boneIdx];
				break;
			}

			boneIdx = this->getFlverBoneParentIndex(boneIdx);
		}

		if (this->m_flverToMorphemeBoneMap[i] == -1)
		{
			std::string boneName = this->getFlverBoneName(i);
			g_appLog->debugMessage(MsgLevel_Debug, "\tBone \"%s\" is not in the morpheme rig, skinning it to \"%s\"\n", boneName.c_str(), this->getMorphemeBoneName(this->m_flverToMorphemeSkinningBoneMap[i]).c_str());
		}
	}
}

//Reads the twist bone settings of every flver bone from the FLVPWV file. Entries are matched to flver bones by name.
void FlverModel::createFlverTwistBones(const ChrModelExFormat::ChrModelExFormat* exFormat)
{
	const int boneCount = this->m_flver->header.boneCount;

	this->m_flverTwistBones.assign(boneCount, TwistBone());

	if (exFormat == nullptr)
		return;

	for (int i = 0; i < boneCount; i++)
	{
		if (this->m_flver->bones[i].name == nullptr)
			continue;

		const ChrModelExFormat::Bone* exBone = exFormat->getBone(this->m_flver->bones[i].name);

		if ((exBone == nullptr) || !exBone->isTwistBone())
			continue;

		const int baseBone = exBone->getBaseBone();
		const int rotationAdditionBone = exBone->getRotationAdditionBone();

		if ((baseBone < 0) || (baseBone >= boneCount) || (rotationAdditionBone < 0) || (rotationAdditionBone >= boneCount) || (baseBone == i) || (rotationAdditionBone == i))
		{
			g_appLog->debugMessage(MsgLevel_Warn, "Twist bone \"%s\" has invalid base (%d) or rotation addition (%d) bone, ignoring it\n", this->getFlverBoneName(i).c_str(), baseBone, rotationAdditionBone);
			continue;
		}

		TwistBone& twistBone = this->m_flverTwistBones[i];
		twistBone.baseBone = baseBone;
		twistBone.rotationAdditionBone = rotationAdditionBone;
		twistBone.rotationScale = exBone->getRotationScale();
		twistBone.threeAxis = (exBone->getType() == ChrModelExFormat::FLVPWV::Bone::TWIST_BONE_THREE_AXIS);
		twistBone.twistAxis = this->computeFlverBoneDirection(rotationAdditionBone);

		g_appLog->debugMessage(MsgLevel_Debug, "\tTwist bone \"%s\": base=\"%s\", rotationAddition=\"%s\", scale=%.3f, %s\n", this->getFlverBoneName(i).c_str(), this->getFlverBoneName(baseBone).c_str(), this->getFlverBoneName(rotationAdditionBone).c_str(), twistBone.rotationScale, twistBone.threeAxis ? "three axis" : "one axis");
	}
}

//Gets the direction a flver bone points to in its own bind pose frame: towards its farthest child, so a twist bone sitting on top of it 
//does not count. Falls back to local X for leaf bones.
Vector3 FlverModel::computeFlverBoneDirection(int idx)
{
	const int boneCount = this->m_flver->header.boneCount;

	Vector3 direction = Vector3::Zero;
	int childIdx = this->m_flver->bones[idx].childIndex;

	for (int i = 0; (childIdx >= 0) && (childIdx < boneCount) && (i < boneCount); i++)
	{
		// Both bind transforms carry the axis adjust matrix, which cancels out here
		const Vector3 childOffset = (this->m_flverBindPoseTransforms[childIdx] * this->m_flverInverseBindPoseTransforms[idx]).Translation();

		if (childOffset.LengthSquared() > direction.LengthSquared())
			direction = childOffset;

		childIdx = this->m_flver->bones[childIdx].nextSiblingIndex;
	}

	if (direction.LengthSquared() > 1e-8f)
	{
		direction.Normalize();
		return direction;
	}

	return Vector3::UnitX;
}

bool FlverModel::isFlverTwistBone(int idx) const
{
	if ((idx < 0) || (idx >= this->m_flverTwistBones.size()))
		return false;

	return this->m_flverTwistBones[idx].baseBone != -1;
}

//Orders the flver bones so that each bone comes after its parent, and twist bones after their base and rotation addition bones
void FlverModel::createFlverBoneEvaluationOrder()
{
	const int boneCount = this->m_flver->header.boneCount;

	enum VisitState : uint8_t { kNotVisited, kVisiting, kVisited };
	std::vector<VisitState> state(boneCount, kNotVisited);

	this->m_flverBoneEvaluationOrder.clear();
	this->m_flverBoneEvaluationOrder.reserve(boneCount);

	std::function<void(int)> visit = [&](int idx)
	{
		// A bone that is still being visited is part of a dependency cycle: skip that dependency instead of looping
		if ((idx == -1) || (state[idx] != kNotVisited))
			return;

		state[idx] = kVisiting;

		visit(this->getFlverBoneParentIndex(idx));

		if (this->isFlverTwistBone(idx))
		{
			visit(this->m_flverTwistBones[idx].baseBone);
			visit(this->m_flverTwistBones[idx].rotationAdditionBone);
		}

		state[idx] = kVisited;
		this->m_flverBoneEvaluationOrder.push_back(idx);
	};

	for (int i = 0; i < boneCount; i++)
		visit(i);
}

//Computes the current transform of a twist bone: its bind pose relative to its base bone, plus the rotation of its rotation addition bone 
//away from that bone's bind pose, scaled by the rotation scale. One axis twist bones only take the roll around the rotation addition bone's direction.
Matrix FlverModel::computeFlverTwistBoneTransform(int idx)
{
	const TwistBone& twistBone = this->m_flverTwistBones[idx];
	const int additionBone = twistBone.rotationAdditionBone;
	const int additionParent = this->getFlverBoneParentIndex(additionBone);

	// Local transforms of the rotation addition bone. Root bones are taken relative to the axis adjust matrix, so neither carries its reflection.
	const Matrix additionParentTransform = (additionParent != -1) ? this->m_flverBoneTransforms[additionParent] : g_flverBoneAdjustMatrix;
	const Matrix additionParentBindInverse = (additionParent != -1) ? this->m_flverInverseBindPoseTransforms[additionParent] : g_flverBoneAdjustMatrix.Invert();

	const Matrix additionLocal = getRotationMatrix(this->m_flverBoneTransforms[additionBone] * additionParentTransform.Invert());
	const Matrix additionBindLocal = getRotationMatrix(this->m_flverBindPoseTransforms[additionBone] * additionParentBindInverse);

	// Rotation away from the bind pose, expressed in the rotation addition bone's own frame (local = delta * bind)
	const Matrix additionDelta = additionLocal * additionBindLocal.Transpose();
	const Matrix scaledDelta = scaleTwistRotation(additionDelta, twistBone.rotationScale, !twistBone.threeAxis, twistBone.twistAxis);

	// Move the rotation from the rotation addition bone's frame to the twist bone's frame
	const Matrix additionToTwist = getRotationMatrix(this->m_flverBindPoseTransforms[idx] * this->m_flverInverseBindPoseTransforms[additionBone]);
	const Matrix twistRotation = additionToTwist * scaledDelta * additionToTwist.Transpose();

	const int baseBone = twistBone.baseBone;

	return twistRotation * this->m_flverBindPoseTransforms[idx] * this->m_flverInverseBindPoseTransforms[baseBone] * this->m_flverBoneTransforms[baseBone];
}

int FlverModel::getMorphemeSkinningBoneIdByFlverBoneId(int idx)
{
	if ((idx >= 0) && (idx < this->m_flverToMorphemeSkinningBoneMap.size()))
		return this->m_flverToMorphemeSkinningBoneMap[idx];

	return -1;
}

FlverModel::MorphemeSkinInfluences FlverModel::getMorphemeSkinInfluences(const SkinnedVertex& vertex)
{
	// Twist bones split their weight in two, so a vertex can reach up to 8 morpheme bones before trimming
	constexpr int kMaxCandidates = 8;

	int candidateBones[kMaxCandidates];
	float candidateWeights[kMaxCandidates];
	int numCandidates = 0;

	// Several flver bones can collapse onto the same morpheme bone (e.g. an upper arm and its twist bone), merge them
	auto addInfluence = [&](int flverBoneID, float weight)
	{
		const int morphemeBoneID = this->getMorphemeSkinningBoneIdByFlverBoneId(flverBoneID);

		if ((morphemeBoneID == -1) || !(weight > 0.f))
			return;

		int slot = 0;
		while ((slot < numCandidates) && (candidateBones[slot] != morphemeBoneID))
			slot++;

		if (slot == numCandidates)
		{
			candidateBones[slot] = morphemeBoneID;
			candidateWeights[slot] = 0.f;
			numCandidates++;
		}

		candidateWeights[slot] += weight;
	};

	for (int wt = 0; wt < 4; wt++)
	{
		const int flverBoneID = vertex.boneIndices[wt];
		const float weight = vertex.boneWeights[wt];

		if (!(weight > 0.f))
			continue;

		// Twist bones the morpheme rig does not have: split the weight between the base bone and the rotation addition bone by the rotation 
		// scale, so the exported mesh picks up part of the twist the preview computes.
		if ((this->getMorphemeBoneIdByFlverBoneId(flverBoneID) == -1) && this->isFlverTwistBone(flverBoneID))
		{
			const TwistBone& twistBone = this->m_flverTwistBones[flverBoneID];
			const float rotationScale = std::clamp(twistBone.rotationScale, 0.f, 1.f);

			addInfluence(twistBone.baseBone, weight * (1.f - rotationScale));
			addInfluence(twistBone.rotationAdditionBone, weight * rotationScale);
		}
		else
			addInfluence(flverBoneID, weight);
	}

	// Keep the 4 largest influences
	MorphemeSkinInfluences influences;

	for (int i = 0; (i < numCandidates) && (influences.numInfluences < 4); i++)
	{
		int largest = i;
		for (int j = i + 1; j < numCandidates; j++)
		{
			if (candidateWeights[j] > candidateWeights[largest])
				largest = j;
		}

		std::swap(candidateBones[i], candidateBones[largest]);
		std::swap(candidateWeights[i], candidateWeights[largest]);

		influences.boneIndices[influences.numInfluences] = candidateBones[i];
		influences.boneWeights[influences.numInfluences] = candidateWeights[i];
		influences.numInfluences++;
	}

	float totalWeight = 0.f;
	for (int i = 0; i < influences.numInfluences; i++)
		totalWeight += influences.boneWeights[i];

	if (totalWeight > 0.f)
	{
		for (int i = 0; i < influences.numInfluences; i++)
			influences.boneWeights[i] /= totalWeight;
	}

	return influences;
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
		INVOKE_PANIC("FlverModel::getFlverBoneGlobalTransform: index out of range");

	return this->m_flverBoneTransforms[idx] * this->getWorldMatrix();
}

Matrix FlverModel::getFlverBoneBindPoseGlobalTransform(int idx)
{
	if (idx > this->m_flverBindPoseTransforms.size())
		INVOKE_PANIC("FlverModel::getFlverBoneBindPoseGlobalTransform: index out of range");

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
	if (this->m_flver)
		return Vector3(this->m_flver->header.boundingBoxMin.x, this->m_flver->header.boundingBoxMin.y, this->m_flver->header.boundingBoxMin.z);

	return Vector3(FLT_MIN, FLT_MIN, FLT_MIN);
}

Vector3 FlverModel::getBoundingBoxMax()
{
	if (this->m_flver)
		return Vector3(this->m_flver->header.boundingBoxMax.x, this->m_flver->header.boundingBoxMax.y, this->m_flver->header.boundingBoxMax.z);

	return Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
}

Matrix FlverModel::getMorphemeBoneGlobalTransform(int idx)
{
	if (idx > this->m_nmBoneTransforms.size())
		INVOKE_PANIC("FlverModel::getMorphemeBoneGlobalTransform: index out of range");

	return this->m_nmBoneTransforms[idx] * this->getWorldMatrix();
}

Matrix FlverModel::getMorphemeBoneBindPoseGlobalTransform(int idx)
{
	if (idx > this->m_nmBindPoseTransforms.size())
		INVOKE_PANIC("FlverModel::getMorphemeBoneBindPoseGlobalTransform: index out of range");

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
		computeAnimationTransforms(animHandle);
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

		if (boneID >= boneRelativeTransforms.size()) continue;

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

			DX::DrawJoint(&prim, Matrix::Identity, boneB, boneA, boneMarkerColor);
		}
	}

	DX::Draw(&prim, DirectX::BoundingSphere(Vector3::Transform(Vector3::Zero, getMorphemeRootBoneGlobalTransform()), 0.03f), DirectX::Colors::Orange);
	DX::Draw(&prim, DirectX::BoundingSphere(Vector3::Transform(Vector3::Zero, getMorphemeTrajectoryBoneGlobalTransform()), 0.03f), DirectX::Colors::Red);
}