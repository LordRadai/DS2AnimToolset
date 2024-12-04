#include <algorithm>
#include "GLTFTranslator.h"
#include "MorphemeEditor.h"
#include "extern.h"
#include "RCore.h"
#include "utils/utils.h"

namespace
{
    void addRootNode(tinygltf::Model* gltf, std::string name, Quaternion rotation = Quaternion::Identity, Vector3 translation = Vector3::Zero, Vector3 scale = Vector3::One)
    {
        tinygltf::Node rootNode;
        rootNode.name = name;

        rootNode.translation = { translation.x, translation.y, translation.z };
        rootNode.rotation = { rotation.x, rotation.y, rotation.z, rotation.w };
        rootNode.scale = { scale.x, scale.y, scale.z };

        gltf->nodes.push_back(rootNode);
    }

    void calculateMinMax(const std::vector<Vector3>& vectors, Vector3& outMin, Vector3& outMax) 
    {
        if (vectors.empty()) 
        {
            // Handle empty input case
            outMin = Vector3(0, 0, 0);
            outMax = Vector3(0, 0, 0);
            return;
        }

        // Initialize min and max with extreme values
        outMin = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
        outMax = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

        // Iterate through the vector and update min and max
        for (const auto& vec : vectors) 
        {
            outMin.x = std::fmin(outMin.x, vec.x);
            outMin.y = std::fmin(outMin.y, vec.y);
            outMin.z = std::fmin(outMin.z, vec.z);

            outMax.x = std::fmax(outMax.x, vec.x);
            outMax.y = std::fmax(outMax.y, vec.y);
            outMax.z = std::fmax(outMax.z, vec.z);
        }
    }

    int getGltfNodeByName(tinygltf::Model* gltf, std::string name)
    {
        for (size_t i = 0; i < gltf->nodes.size(); i++)
        {
            if (gltf->nodes[i].name == name)
                return i;
        }

        return -1;
    }

    // This doesn't work, I don't know why, but it's all wrong
    void addBoneVisuals(tinygltf::Model* gltf, MR::AnimRigDef* rig) 
    {
        // Debug bone view material
        tinygltf::Material lineMaterial;
        lineMaterial.name = "DebugBoneMaterial";
        lineMaterial.pbrMetallicRoughness.baseColorFactor = { 0.0, 0.0, 1.0, 1.0 };
        lineMaterial.pbrMetallicRoughness.metallicFactor = 0.0;
        lineMaterial.pbrMetallicRoughness.roughnessFactor = 1.0;
        gltf->materials.push_back(lineMaterial);
        int lineMaterialIndex = gltf->materials.size() - 1;

        // Iterate through all joints
        for (size_t i = 1; i < rig->getNumBones(); ++i) 
        {
            int parentIndex = rig->getParentBoneIndex(i);
            if (parentIndex == -1)
                continue;

            // Get the positions of the bone and its parent            
            Vector3 start = Vector3::Transform(Vector3::Zero, utils::NMDX::getTransformMatrix(*rig->getBindPoseBoneQuat(parentIndex), *rig->getBindPoseBonePos(parentIndex)));
            Vector3 end = Vector3::Transform(Vector3::Zero, utils::NMDX::getTransformMatrix(*rig->getBindPoseBoneQuat(i), *rig->getBindPoseBonePos(i)));

            // Create a line mesh for the bone
            std::vector<Vector3> vertices = { start, end };
            std::vector<uint16_t> indices = { 0, 1 };

            // Create a buffer for the line data
            tinygltf::Buffer buffer;
            buffer.data.insert(buffer.data.end(),
                reinterpret_cast<const unsigned char*>(vertices.data()),
                reinterpret_cast<const unsigned char*>(vertices.data() + vertices.size()));

            buffer.data.insert(buffer.data.end(),
                reinterpret_cast<const unsigned char*>(indices.data()),
                reinterpret_cast<const unsigned char*>(indices.data() + indices.size()));

            gltf->buffers.push_back(buffer);

            size_t positionDataSize = vertices.size() * sizeof(Vector3);

            // Create a BufferView for the line data
            tinygltf::BufferView bufferView;
            bufferView.buffer = gltf->buffers.size() - 1;
            bufferView.byteOffset = 0;
            bufferView.byteLength = positionDataSize;
            bufferView.target = TINYGLTF_TARGET_ARRAY_BUFFER;
            gltf->bufferViews.push_back(bufferView);

            // Create an Accessor for the line positions
            tinygltf::Accessor positionAccessor;
            positionAccessor.bufferView = gltf->bufferViews.size() - 1;
            positionAccessor.byteOffset = 0;
            positionAccessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
            positionAccessor.type = TINYGLTF_TYPE_VEC3;
            positionAccessor.count = vertices.size();
            gltf->accessors.push_back(positionAccessor);

            // Create another BufferView and Accessor for the indices
            tinygltf::BufferView indexBufferView;
            indexBufferView.buffer = gltf->buffers.size() - 1;
            indexBufferView.byteOffset = positionDataSize;
            indexBufferView.byteLength = indices.size() * sizeof(uint16_t);
            indexBufferView.target = TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER;
            gltf->bufferViews.push_back(indexBufferView);

            tinygltf::Accessor indexAccessor;
            indexAccessor.bufferView = gltf->bufferViews.size() - 1;
            indexAccessor.byteOffset = 0;
            indexAccessor.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT;
            indexAccessor.type = TINYGLTF_TYPE_SCALAR;
            indexAccessor.count = indices.size();
            gltf->accessors.push_back(indexAccessor);

            // Create a Primitive for the line
            tinygltf::Primitive primitive;
            primitive.attributes["POSITION"] = gltf->accessors.size() - 2; // Position accessor index
            primitive.indices = gltf->accessors.size() - 1;               // Index accessor index
            primitive.mode = TINYGLTF_MODE_LINE;
            primitive.material = lineMaterialIndex;                            // Assign material

            // Create a Mesh for the line
            tinygltf::Mesh lineMesh;
            lineMesh.name = "BoneDebugMesh_" + std::to_string(i);
            lineMesh.primitives.push_back(primitive);
            gltf->meshes.push_back(lineMesh);

            // Create a Node for the line and add it to the model
            tinygltf::Node lineNode;
            lineNode.mesh = gltf->meshes.size() - 1;
            gltf->nodes.push_back(lineNode);

            // Add the debug node to the root node or appropriate parent
            gltf->nodes[parentIndex].children.push_back(gltf->nodes.size() - 1);
        }
    }

    void getSkinnedVertices(std::vector<Vector4>& weights, std::vector<JointIndicesVec>& indices, FlverModel* model, int meshIndex)
    {
        std::vector<std::vector<int>> jointIndices = model->getFlverMeshBoneIndices(meshIndex);
        std::vector<Vector4> jointWeights = model->getFlverMeshBoneWeights(meshIndex);

        // Normalize weights
        for (size_t i = 0; i < jointWeights.size(); i++)
        {
            float totalWeight = jointWeights[i].x + jointWeights[i].y + jointWeights[i].z + jointWeights[i].w;

            if (totalWeight > 0.0f)
                jointWeights[i] /= totalWeight;
        }

        // Convert flver bone indices to morpheme bone indices
        for (size_t i = 0; i < jointIndices.size(); i++)
        {
            Vector4 jointWeightData = Vector4::Zero;
            JointIndicesVec jointIndexData(-1, -1, -1, -1);
            for (size_t j = 0; j < 4; j++)
            {
                const int morphemeBoneIdx = model->getMorphemeBoneIdByFlverBoneId(jointIndices[i][j]);

                if (morphemeBoneIdx != -1)
                {
                    switch (j)
                    {
                    case 0:
                        jointWeightData.x = jointWeights[i].x;
                        jointIndexData.x = morphemeBoneIdx;
                        break;
                    case 1:
                        jointWeightData.y = jointWeights[i].y;
                        jointIndexData.y = morphemeBoneIdx;
                        break;
                    case 2:
                        jointWeightData.z = jointWeights[i].z;
                        jointIndexData.z = morphemeBoneIdx;
                        break;
                    case 3:
                        jointWeightData.w = jointWeights[i].w;
                        jointIndexData.w = morphemeBoneIdx;
                        break;
                    }
                }
            }

            weights.push_back(jointWeightData);
            indices.push_back(jointIndexData);
        }
    }

    tinygltf::BufferView* createBufferView(tinygltf::Model* gltf, int targetBuffer, size_t byteOffset, size_t byteLength, int target)
    {
        tinygltf::BufferView bufferView;
        bufferView.buffer = targetBuffer;
        bufferView.byteOffset = byteOffset;
        bufferView.byteLength = byteLength;
        bufferView.target = target;
        gltf->bufferViews.push_back(bufferView);

        return &gltf->bufferViews.back();
    }

    tinygltf::Accessor* createBufferViewAccessor(tinygltf::Model* gltf, int targetBufferView, size_t byteOffset, int componentType, int type, int count)
    {
        tinygltf::Accessor accessor;
        accessor.bufferView = targetBufferView;
        accessor.byteOffset = byteOffset;
        accessor.componentType = componentType;
        accessor.type = type;
        accessor.count = count;

        gltf->accessors.push_back(accessor);

        return &gltf->accessors.back();
    }

    float clampValue(float value, float tolerance = 1e-4f) 
    {
        if (std::abs(value - 1.0f) < tolerance) 
            return 1.0f;
        if (std::abs(value) < tolerance) 
            return 0.0f;

        return value;
    }

    void clampMatrix(Matrix& matrix)
    {
        for (size_t i = 0; i < 4; i++)
        {
            for (size_t j = 0; j < 4; j++)
                matrix.m[i][j] = clampValue(matrix.m[i][j]);
        }
    }
}

namespace GLTFTranslator
{
	tinygltf::Model* createModel(MR::AnimRigDef* rig, FlverModel* model, bool includeMeshes)
	{
		tinygltf::Model* gltfModel = new tinygltf::Model;
		gltfModel->asset.version = "2.0";
		gltfModel->asset.generator = APPNAME_A;

        addRootNode(gltfModel, "SceneRoot"/*, Quaternion::CreateFromAxisAngle(Vector3::Right, -DirectX::XM_PIDIV2)*/);

        //Add all the bones
        for (size_t i = 1; i < rig->getNumBones(); i++)
            createJoint(gltfModel, rig, i);

        //Loop back over to add all children
        for (size_t i = 1; i < rig->getNumBones(); i++)
        {
            int jointIndex = getGltfNodeByName(gltfModel, rig->getBoneName(i));
            int parentIndex = getGltfNodeByName(gltfModel, rig->getBoneName(rig->getParentBoneIndex(i)));

            //If the parent bone was not found, add to the root node
            if (parentIndex == -1)
                parentIndex = 0;

            gltfModel->nodes[parentIndex].children.push_back(jointIndex);
        }

        addBoneVisuals(gltfModel, rig);

        if (includeMeshes && (model != nullptr))
        {
            for (size_t i = 0; i < model->getNumMeshes(); i++)
                createMesh(gltfModel, model, i);
        }

		return gltfModel;
	}

    tinygltf::Mesh* createMesh(tinygltf::Model* gltf, FlverModel* model, int meshIndex)
    {
        std::vector<Vector3> vertices = model->getFlverMeshVertices(meshIndex, true);
        std::vector<Vector3> normals = model->getFlverMeshNormals(meshIndex, true);

        Vector3 minBound, maxBound;

        calculateMinMax(vertices, minBound, maxBound);

        // Normalize the normals, or gltf will complain about the small errors
        for (size_t i = 0; i < normals.size(); i++)
            normals[i].Normalize();

        std::vector<uint16_t> indices;
        indices.reserve(vertices.size());
        for (size_t i = 0; i < vertices.size(); i++)
            indices.push_back(i);

        // Create a buffer to hold all data (positions, normals, indices)
        tinygltf::Buffer buffer;
        buffer.name = "MeshBuffer_" + std::to_string(meshIndex);

        // Add vertex positions to the buffer
        size_t positionDataSize = vertices.size() * sizeof(Vector3);
        size_t positionOffset = buffer.data.size();
        buffer.data.insert(buffer.data.end(),
            reinterpret_cast<const unsigned char*>(vertices.data()),
            reinterpret_cast<const unsigned char*>(vertices.data() + vertices.size()));

        // Add vertex normals to the buffer
        size_t normalDataSize = normals.size() * sizeof(Vector3);
        size_t normalOffset = buffer.data.size();
        buffer.data.insert(buffer.data.end(),
            reinterpret_cast<const unsigned char*>(normals.data()),
            reinterpret_cast<const unsigned char*>(normals.data() + normals.size()));

        // Add index data to the buffer
        size_t indexDataSize = indices.size() * sizeof(uint16_t);
        size_t indexOffset = buffer.data.size();
        buffer.data.insert(buffer.data.end(),
            reinterpret_cast<const unsigned char*>(indices.data()),
            reinterpret_cast<const unsigned char*>(indices.data() + indices.size()));

        std::vector<Vector4> jointWeights;
        std::vector<JointIndicesVec> jointIndices;
        getSkinnedVertices(jointWeights, jointIndices, model, meshIndex);

        // Add joint indices to the buffer
        size_t jointDataSize = jointIndices.size() * sizeof(Vector4);
        size_t jointOffset = buffer.data.size();
        buffer.data.insert(buffer.data.end(),
            reinterpret_cast<const unsigned char*>(jointIndices.data()),
            reinterpret_cast<const unsigned char*>(jointIndices.data() + jointIndices.size()));

        // Add weights to the buffer
        size_t weightDataSize = jointWeights.size() * sizeof(Vector4);
        size_t weightOffset = buffer.data.size();
        buffer.data.insert(buffer.data.end(),
            reinterpret_cast<const unsigned char*>(jointWeights.data()),
            reinterpret_cast<const unsigned char*>(jointWeights.data() + jointWeights.size()));

        // Prepare joints and inverse bind matrices
        std::vector<int> jointNodeIndices;
        std::vector<Matrix> inverseBindMatrices; // Replace with your matrix type

        const MR::AnimRigDef* rig = model->getRig();

        for (size_t i = 1; i < rig->getNumBones(); i++)
        {
            Matrix bindPoseTransform = utils::NMDX::getTransformMatrix(*rig->getBindPoseBoneQuat(i), *rig->getBindPoseBonePos(i));
            clampMatrix(bindPoseTransform);
            inverseBindMatrices.push_back(bindPoseTransform.Invert());
        }

        // Add inverse bind matrices to the buffer
        size_t inverseBindMatrixDataSize = inverseBindMatrices.size() * sizeof(Matrix);
        size_t inverseBindMatrixOffset = buffer.data.size();
        buffer.data.insert(buffer.data.end(),
            reinterpret_cast<const unsigned char*>(inverseBindMatrices.data()),
            reinterpret_cast<const unsigned char*>(inverseBindMatrices.data() + inverseBindMatrices.size()));

        gltf->buffers.push_back(buffer);

        // Vertices
        createBufferView(gltf, gltf->buffers.size() - 1, positionOffset, positionDataSize, TINYGLTF_TARGET_ARRAY_BUFFER);
        tinygltf::Accessor* positionAccessor = createBufferViewAccessor(gltf, gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_VEC3, vertices.size());
        positionAccessor->minValues = { minBound.x, minBound.y, minBound.z };
        positionAccessor->maxValues = { maxBound.x, maxBound.y, maxBound.z };

        const int positionDataIdx = gltf->accessors.size() - 1;

        // Normals
        createBufferView(gltf, gltf->buffers.size() - 1, normalOffset, normalDataSize, TINYGLTF_TARGET_ARRAY_BUFFER);
        createBufferViewAccessor(gltf, gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_VEC3, normals.size());

        const int normalsDataIdx = gltf->accessors.size() - 1;

        // Vertex indices
        createBufferView(gltf, gltf->buffers.size() - 1, indexOffset, indexDataSize, TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER);
        createBufferViewAccessor(gltf, gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, TINYGLTF_TYPE_SCALAR, indices.size());

        const int indicesDataIdx = gltf->accessors.size() - 1;

        // Bone indices
        createBufferView(gltf, gltf->buffers.size() - 1, jointOffset, jointDataSize, TINYGLTF_TARGET_ARRAY_BUFFER);
        createBufferViewAccessor(gltf, gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, TINYGLTF_TYPE_VEC4, jointIndices.size());

        const int boneIndicesDataIdx = gltf->accessors.size() - 1;

        // Bone weights
        createBufferView(gltf, gltf->buffers.size() - 1, weightOffset, weightDataSize, TINYGLTF_TARGET_ARRAY_BUFFER);
        createBufferViewAccessor(gltf, gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_VEC4, jointWeights.size());

        const int boneWeightsDataIdx = gltf->accessors.size() - 1;

        // IBM
        createBufferView(gltf, gltf->buffers.size() - 1, inverseBindMatrixOffset, inverseBindMatrixDataSize, 0);
        createBufferViewAccessor(gltf, gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_MAT4, inverseBindMatrices.size());
        
        const int ibmDataIdx = gltf->accessors.size() - 1;

        tinygltf::Primitive primitive;
        primitive.indices = indicesDataIdx;
        primitive.mode = TINYGLTF_MODE_TRIANGLES;
        primitive.attributes["POSITION"] = positionDataIdx;
        primitive.attributes["NORMAL"] = normalsDataIdx;
        primitive.attributes["JOINTS_0"] = boneIndicesDataIdx;
        primitive.attributes["WEIGHTS_0"] = boneWeightsDataIdx;

        for (int boneIndex = 0; boneIndex < rig->getNumBones(); ++boneIndex) 
        {
            int jointNodeIndex = getGltfNodeByName(gltf, rig->getBoneName(boneIndex));

            if (jointNodeIndex != -1)
                jointNodeIndices.push_back(jointNodeIndex);
        }

        // Create a Skin
        tinygltf::Skin skin;
        skin.name = "Skin_" + std::to_string(meshIndex);
        skin.joints = jointNodeIndices;
        skin.inverseBindMatrices = ibmDataIdx;

        // Add the skin to the model
        gltf->skins.push_back(skin);

        // Create a Mesh and add the primitive
        tinygltf::Mesh mesh;
        mesh.name = "Mesh_" + std::to_string(meshIndex);
        mesh.primitives.push_back(primitive);

        // Add the mesh to the model
        gltf->meshes.push_back(mesh);

        tinygltf::Node node;
        node.name = "MeshNode_" + std::to_string(meshIndex);
        node.mesh = gltf->meshes.size() - 1; // Index of the mesh
        node.skin = gltf->skins.size() - 1;

        // Add node to the model
        gltf->nodes.push_back(node);

        // Add node as a child of the root node
        gltf->nodes[0].children.push_back(gltf->nodes.size() - 1);

        // Return a pointer to the newly added mesh
        return &gltf->meshes.back();
    }

    tinygltf::Node* createJoint(tinygltf::Model* gltf, const MR::AnimRigDef* rig, const int boneID) 
    {
        const NMP::Vector3* translation = rig->getBindPoseBonePos(boneID);
        const NMP::Quat* rotation = rig->getBindPoseBoneQuat(boneID);

        tinygltf::Node jointNode;
        jointNode.name = rig->getBoneName(boneID);
        jointNode.translation = { translation->x, translation->y, translation->z };
        jointNode.rotation = { rotation->x, rotation->y, rotation->z, rotation->w };
        jointNode.scale = { 1.0f, 1.0f, 1.0f };

        gltf->nodes.push_back(jointNode);

        return &gltf->nodes.back();
    }
}