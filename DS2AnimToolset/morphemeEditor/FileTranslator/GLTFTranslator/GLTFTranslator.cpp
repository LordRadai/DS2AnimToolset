#include <algorithm>
#include "GLTFTranslator.h"
#include "MorphemeEditor.h"
#include "extern.h"
#include "RCore.h"
#include "utils/utils.h"

namespace
{
    tinygltf::BufferView* createBufferView(tinygltf::Model* gltf, std::string name, int targetBuffer, size_t byteOffset, size_t byteLength, int target)
    {
        tinygltf::BufferView bufferView;
        bufferView.name = name;
        bufferView.buffer = targetBuffer;
        bufferView.byteOffset = byteOffset;
        bufferView.byteLength = byteLength;
        bufferView.target = target;
        gltf->bufferViews.push_back(bufferView);

        return &gltf->bufferViews.back();
    }

    tinygltf::Accessor* createAccessor(tinygltf::Model* gltf, std::string name, int targetBufferView, size_t byteOffset, int componentType, int type, int count)
    {
        tinygltf::Accessor accessor;
        accessor.name = name;
        accessor.bufferView = targetBufferView;
        accessor.byteOffset = byteOffset;
        accessor.componentType = componentType;
        accessor.type = type;
        accessor.count = count;

        gltf->accessors.push_back(accessor);

        return &gltf->accessors.back();
    }

    void createSphere(tinygltf::Model* gltf, std::string name, Vector3 center, float radius, int latitudeDivisions = 50, int longitudeDivisions = 50)
    {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        std::vector<float> normals;

        // Generate the vertices and normals
        for (int lat = 0; lat <= latitudeDivisions; ++lat) {
            float theta = lat * DirectX::XM_PI / latitudeDivisions; // Latitude angle
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            for (int lon = 0; lon <= longitudeDivisions; ++lon) {
                float phi = lon * 2 * DirectX::XM_PI / longitudeDivisions; // Longitude angle
                float sinPhi = sin(phi);
                float cosPhi = cos(phi);

                // Vertex position
                float x = center.x + radius * cosPhi * sinTheta;
                float y = center.y + radius * cosTheta;
                float z = center.z + radius * sinPhi * sinTheta;

                // Normal (normalized position vector)
                float nx = cosPhi * sinTheta;
                float ny = cosTheta;
                float nz = sinPhi * sinTheta;

                // Add vertex position
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                // Add normal
                normals.push_back(nx);
                normals.push_back(ny);
                normals.push_back(nz);
            }
        }

        // Generate the indices for the sphere's triangles
        for (int lat = 0; lat < latitudeDivisions; ++lat) {
            for (int lon = 0; lon < longitudeDivisions; ++lon) {
                int first = (lat * (longitudeDivisions + 1)) + lon;
                int second = first + longitudeDivisions + 1;

                // Create two triangles for each quad
                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }

        // Create the mesh and the corresponding buffer
        tinygltf::Mesh mesh;

        // Add position attribute (vertices)
        tinygltf::Accessor positionAccessor;
        positionAccessor.bufferView = 0; // Assuming the buffer view will be added to the buffer after this step
        positionAccessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
        positionAccessor.type = TINYGLTF_TYPE_VEC3;
        positionAccessor.count = vertices.size() / 3;
        gltf->accessors.push_back(positionAccessor);

        // Add normal attribute (normals)
        tinygltf::Accessor normalAccessor;
        normalAccessor.bufferView = 1; // Assuming normal buffer view
        normalAccessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
        normalAccessor.type = TINYGLTF_TYPE_VEC3;
        normalAccessor.count = normals.size() / 3;
        gltf->accessors.push_back(normalAccessor);

        // Add index buffer
        tinygltf::Accessor indexAccessor;
        indexAccessor.bufferView = 2; // Assuming index buffer view
        indexAccessor.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT;
        indexAccessor.type = TINYGLTF_TYPE_SCALAR;
        indexAccessor.count = indices.size();
        gltf->accessors.push_back(indexAccessor);

        // Add the buffers
        tinygltf::Buffer buffer;
        buffer.data.insert(buffer.data.end(), vertices.begin(), vertices.end());
        buffer.data.insert(buffer.data.end(), normals.begin(), normals.end());
        buffer.data.insert(buffer.data.end(), indices.begin(), indices.end());
        gltf->buffers.push_back(buffer);

        // Create buffer views for position, normals, and indices
        createBufferView(gltf, "VertexBufferView", 0, 0, vertices.size() * sizeof(float), TINYGLTF_TARGET_ARRAY_BUFFER);
        createBufferView(gltf, "NormalsBufferView", 1, vertices.size() * sizeof(float), normals.size() * sizeof(float), TINYGLTF_TARGET_ARRAY_BUFFER);
        createBufferView(gltf, "IndexBufferView", 2, normals.size() * sizeof(float), indices.size() * sizeof(unsigned int), TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER);

        // Create a mesh object
        tinygltf::Primitive primitive;
        primitive.attributes["POSITION"] = 0;  // Position attribute
        primitive.attributes["NORMAL"] = 1;    // Normal attribute
        primitive.mode = TINYGLTF_MODE_TRIANGLES;
        primitive.indices = 2;                  // Index accessor

        // Create the mesh
        mesh.name = name;
        mesh.primitives.push_back(primitive);

        gltf->meshes.push_back(mesh);
    }

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

    int getGltfNodeIndexByName(tinygltf::Model* gltf, std::string name)
    {
        for (size_t i = 0; i < gltf->nodes.size(); i++)
        {
            if (gltf->nodes[i].name == name)
                return i;
        }

        return -1;
    }

    tinygltf::Node* getGltfNodeByName(tinygltf::Model* gltf, std::string name)
    {
        const int nodeIdx = getGltfNodeIndexByName(gltf, name);

        if (nodeIdx == -1)
            return nullptr;
        
        return &gltf->nodes[nodeIdx];
    }

    int addBoneInfluence(std::vector<int>& jointArray, std::vector<Matrix>& ibm, FlverModel* model, int boneID)
    {
        for (size_t i = 0; i < jointArray.size(); i++)
        {
            if (boneID == jointArray[i])
                return i;
        }

        ibm.push_back(model->getMorphemeBoneBindPoseGlobalTransform(boneID)->Invert());

        jointArray.push_back(boneID);
        return jointArray.size() - 1;
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
        for (uint32_t boneID = 1; boneID < rig->getNumBones(); ++boneID) 
        {        
            Matrix bindPoseTransform = utils::NMDX::getTransformMatrix(*rig->getBindPoseBoneQuat(boneID), *rig->getBindPoseBonePos(boneID));
            createSphere(gltf, "BoneMarker_" + std::to_string(boneID), Vector3::Transform(Vector3::Zero, bindPoseTransform), 0.3f);
        }
    }

    void getSkinnedVertices(tinygltf::Model* gltf, std::vector<Matrix>& ibm, std::vector<int>& jointArray, std::vector<Vector4>& weights, std::vector<JointIndicesVec>& indices, FlverModel* model, int meshIndex)
    {
        // Make sure the input is empty before adding anything
        ibm.clear();
        jointArray.clear();
        weights.clear();
        indices.clear();

        std::vector<std::vector<int>> jointIndices = model->getFlverMeshBoneIndices(meshIndex);
        std::vector<Vector4> jointWeights = model->getFlverMeshBoneWeights(meshIndex);

        // Normalize weights
        for (size_t i = 0; i < jointWeights.size(); i++)
        {
            const float totalWeight = jointWeights[i].x + jointWeights[i].y + jointWeights[i].z + jointWeights[i].w;

            if (totalWeight > 0.0f)
                jointWeights[i] /= totalWeight;
        }

        const MR::AnimRigDef* rig = model->getRig();

        // Convert flver bone indices to morpheme bone indices
        for (size_t i = 0; i < jointIndices.size(); i++)
        {
            Vector4 jointWeightData = Vector4::Zero;
            JointIndicesVec jointIndexData(-1, -1, -1, -1);
            for (size_t j = 0; j < 4; j++)
            {
                const int morphemeBoneID = model->getMorphemeBoneIdByFlverBoneId(jointIndices[i][j]);

                if (morphemeBoneID != -1)
                {
                    const tinygltf::Node* jointNode = getGltfNodeByName(gltf, model->getMorphemeBoneName(morphemeBoneID));

                    if (jointNode)
                    {
                        const int influenceArrayIndex = addBoneInfluence(jointArray, ibm, model, morphemeBoneID);

                        switch (j)
                        {
                        case 0:
                            jointWeightData.x = jointWeights[i].x;
                            jointIndexData.x = influenceArrayIndex;
                            break;
                        case 1:
                            jointWeightData.y = jointWeights[i].y;
                            jointIndexData.y = influenceArrayIndex;
                            break;
                        case 2:
                            jointWeightData.z = jointWeights[i].z;
                            jointIndexData.z = influenceArrayIndex;
                            break;
                        case 3:
                            jointWeightData.w = jointWeights[i].w;
                            jointIndexData.w = influenceArrayIndex;
                            break;
                        }
                    }           
                }
            }

            weights.push_back(jointWeightData);
            indices.push_back(jointIndexData);
        }
    }

    // Truncate value to specified tolerance value
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

    std::vector<Vector3> getMeshVertices(FlverModel* model, int meshIdx)
    {
        std::vector<Vector3> vertices = model->getFlverMeshVertices(meshIdx, true);

        for (size_t i = 0; i < vertices.size(); i++)
            vertices[i] = Vector3::Transform(vertices[i], /*Matrix::CreateRotationZ(DirectX::XM_PI) **/ Matrix::CreateRotationX(DirectX::XM_PI));

        return vertices;
    }

    std::vector<Vector3> getMeshNormals(FlverModel* model, int meshIdx)
    {
        std::vector<Vector3> normals = model->getFlverMeshNormals(meshIdx, true);

        for (size_t i = 0; i < normals.size(); i++)
            normals[i] = Vector3::Transform(normals[i], /*Matrix::CreateRotationZ(DirectX::XM_PI) **/ Matrix::CreateRotationX(DirectX::XM_PI));

        return normals;
    }
}

namespace GLTFTranslator
{
	tinygltf::Model* createModel(MR::AnimRigDef* rig, FlverModel* model, bool includeMeshes)
	{
		tinygltf::Model* gltfModel = new tinygltf::Model;
		gltfModel->asset.version = "2.0";
		gltfModel->asset.generator = APPNAME_A;

        addRootNode(gltfModel, "SceneRoot");

        //Add all the bones
        for (size_t i = 1; i < rig->getNumBones(); i++)
            createJoint(gltfModel, rig, i);

        //Loop back over to add all children
        for (size_t i = 1; i < rig->getNumBones(); i++)
        {
            int jointIndex = getGltfNodeIndexByName(gltfModel, rig->getBoneName(i));
            int parentIndex = getGltfNodeIndexByName(gltfModel, rig->getBoneName(rig->getParentBoneIndex(i)));

            // If the parent bone was not found, add to the root node
            if (parentIndex == -1)
                parentIndex = 0;

            gltfModel->nodes[parentIndex].children.push_back(jointIndex);
        }

        //addBoneVisuals(gltfModel, rig);

        if (includeMeshes && (model != nullptr))
        {
            for (size_t i = 0; i < model->getNumMeshes(); i++)
                createMesh(gltfModel, model, i);
        }

		return gltfModel;
	}

    tinygltf::Mesh* createMesh(tinygltf::Model* gltf, FlverModel* model, int meshIndex)
    {
        //std::vector<Vector3> vertices = getMeshVertices(model, meshIndex);
        //std::vector<Vector3> normals = getMeshNormals(model, meshIndex);

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

        std::vector<int> jointArray;
        std::vector<Vector4> jointWeights;
        std::vector<JointIndicesVec> jointIndices;
        std::vector<Matrix> inverseBindMatrices;
        getSkinnedVertices(gltf, inverseBindMatrices, jointArray, jointWeights, jointIndices, model, meshIndex);

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

        // Add inverse bind matrices to the buffer
        size_t inverseBindMatrixDataSize = inverseBindMatrices.size() * sizeof(Matrix);
        size_t inverseBindMatrixOffset = buffer.data.size();
        buffer.data.insert(buffer.data.end(),
            reinterpret_cast<const unsigned char*>(inverseBindMatrices.data()),
            reinterpret_cast<const unsigned char*>(inverseBindMatrices.data() + inverseBindMatrices.size()));

        gltf->buffers.push_back(buffer);

        // Vertices
        createBufferView(gltf, "VertexBufferView_" + std::to_string(meshIndex), gltf->buffers.size() - 1, positionOffset, positionDataSize, TINYGLTF_TARGET_ARRAY_BUFFER);
        tinygltf::Accessor* positionAccessor = createAccessor(gltf, "VertexBufAccessor_" + std::to_string(meshIndex), gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_VEC3, vertices.size());
        positionAccessor->minValues = { minBound.x, minBound.y, minBound.z };
        positionAccessor->maxValues = { maxBound.x, maxBound.y, maxBound.z };

        const int positionDataIdx = gltf->accessors.size() - 1;

        // Normals
        createBufferView(gltf, "NormalsBufferView_" + std::to_string(meshIndex), gltf->buffers.size() - 1, normalOffset, normalDataSize, TINYGLTF_TARGET_ARRAY_BUFFER);
        createAccessor(gltf, "NormalsBufAccessor_" + std::to_string(meshIndex), gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_VEC3, normals.size());

        const int normalsDataIdx = gltf->accessors.size() - 1;

        // Vertex indices
        createBufferView(gltf, "IndexBufferView_" + std::to_string(meshIndex), gltf->buffers.size() - 1, indexOffset, indexDataSize, TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER);
        createAccessor(gltf, "IndexBufAccessor_" + std::to_string(meshIndex), gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, TINYGLTF_TYPE_SCALAR, indices.size());

        const int indicesDataIdx = gltf->accessors.size() - 1;

        // Bone indices
        createBufferView(gltf, "JointIndicesBufferView_" + std::to_string(meshIndex), gltf->buffers.size() - 1, jointOffset, jointDataSize, TINYGLTF_TARGET_ARRAY_BUFFER);
        createAccessor(gltf, "JointIndicesBufAccessor_" + std::to_string(meshIndex), gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, TINYGLTF_TYPE_VEC4, jointIndices.size());

        const int boneIndicesDataIdx = gltf->accessors.size() - 1;

        // Bone weights
        createBufferView(gltf, "JointWeightBufferView_" + std::to_string(meshIndex), gltf->buffers.size() - 1, weightOffset, weightDataSize, TINYGLTF_TARGET_ARRAY_BUFFER);
        createAccessor(gltf, "JointWeightBufAccessor_" + std::to_string(meshIndex), gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_VEC4, jointWeights.size());

        const int boneWeightsDataIdx = gltf->accessors.size() - 1;

        // IBM
        createBufferView(gltf, "IbmBufferView_" + std::to_string(meshIndex), gltf->buffers.size() - 1, inverseBindMatrixOffset, inverseBindMatrixDataSize, 0);
        createAccessor(gltf, "IbmBufAccessor" + std::to_string(meshIndex), gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_MAT4, inverseBindMatrices.size());
        
        const int ibmDataIdx = gltf->accessors.size() - 1;

        tinygltf::Primitive primitive;
        primitive.indices = indicesDataIdx;
        primitive.mode = TINYGLTF_MODE_TRIANGLES;
        primitive.attributes["POSITION"] = positionDataIdx;
        primitive.attributes["NORMAL"] = normalsDataIdx;
        primitive.attributes["JOINTS_0"] = boneIndicesDataIdx;
        primitive.attributes["WEIGHTS_0"] = boneWeightsDataIdx;

        // Create a Skin
        tinygltf::Skin skin;
        skin.name = "Skin_" + std::to_string(meshIndex);
        skin.joints = jointArray;
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

    tinygltf::Animation* createAnimation(tinygltf::Model* gltf, AnimObject* animObj, const char* takeName, int fps)
    {
        const MR::AnimationSourceHandle* animHandle = animObj->getHandle();
        const MR::AnimRigDef* rig = animHandle->getRig();

        tinygltf::Buffer buffer;

        const int numKeyframes = fps * animHandle->getDuration();

        std::vector<float> timeData;
        timeData.reserve(numKeyframes);
        for (size_t i = 0; i < numKeyframes; i++)
            timeData.push_back(RMath::frameToTime(i, fps));

        tinygltf::Animation animation;
        animation.name = takeName;

        for (uint32_t channelID = 1; channelID < rig->getNumBones(); channelID++)
        {
            const int targetBoneIndex = getGltfNodeIndexByName(gltf, rig->getBoneName(channelID));
            std::vector<Vector3> translationData;
            std::vector<Quaternion> rotationData;

            for (uint32_t i = 0; i < numKeyframes; i++)
            {
                float animTime = RMath::frameToTime(i, fps);
                animObj->setAnimTime(animTime);

                Matrix transform = utils::NMDX::getTransformMatrix(animHandle->getChannelData()[channelID].m_quat, animHandle->getChannelData()[channelID].m_pos);
                Vector3 position;
                Quaternion rotation;
                Vector3 scale;
                transform.Decompose(scale, rotation, position);

                translationData.push_back(position);
                rotationData.push_back(rotation);
            }

            // Add time data to buffer
            size_t timeOffset = buffer.data.size();
            buffer.data.insert(buffer.data.end(),
                reinterpret_cast<const unsigned char*>(timeData.data()),
                reinterpret_cast<const unsigned char*>(timeData.data() + timeData.size()));

            // Add translation data to buffer
            size_t translationOffset = buffer.data.size();
            buffer.data.insert(buffer.data.end(),
                reinterpret_cast<const unsigned char*>(translationData.data()),
                reinterpret_cast<const unsigned char*>(translationData.data() + translationData.size()));

            // Add rotation data to buffer
            size_t rotationOffset = buffer.data.size();
            buffer.data.insert(buffer.data.end(),
                reinterpret_cast<const unsigned char*>(rotationData.data()),
                reinterpret_cast<const unsigned char*>(rotationData.data() + rotationData.size()));

            gltf->buffers.push_back(buffer);
            createBufferView(gltf, "TimeData_" + std::to_string(channelID), gltf->buffers.size() - 1, timeOffset, timeData.size() * sizeof(float), 0);
            createAccessor(gltf, "TimeDataAccessor_" + std::to_string(channelID), gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_SCALAR, static_cast<unsigned int>(timeData.size()));
            
            const int animTimeAccessorIdx = gltf->accessors.size() - 1;

            createBufferView(gltf, "TranslationData_" + std::to_string(channelID), gltf->buffers.size() - 1, translationOffset, translationData.size() * sizeof(Vector3), 0);
            createAccessor(gltf, "TranslationDataAccessor_" + std::to_string(channelID), gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_VEC3, static_cast<unsigned int>(translationData.size()));

            const int translationDataAccessorIdx = gltf->accessors.size() - 1;

            createBufferView(gltf, "RotationData_" + std::to_string(channelID), gltf->buffers.size() - 1, rotationOffset, rotationData.size() * sizeof(Quaternion), 0);
            createAccessor(gltf, "RotationDataAccessor_" + std::to_string(channelID), gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_VEC4, static_cast<unsigned int>(rotationData.size()));

            const int rotationDataAccessorIdx = gltf->accessors.size() - 1;

            // Create samplers for translation and rotation
            tinygltf::AnimationSampler translationSampler;
            translationSampler.input = animTimeAccessorIdx;
            translationSampler.output = translationDataAccessorIdx;
            translationSampler.interpolation = "LINEAR";

            animation.samplers.push_back(translationSampler);

            const int translationSamplerIdx = animation.samplers.size() - 1;

            tinygltf::AnimationSampler rotationSampler;
            rotationSampler.input = animTimeAccessorIdx;
            rotationSampler.output = rotationDataAccessorIdx;
            rotationSampler.interpolation = "LINEAR";

            animation.samplers.push_back(rotationSampler);

            const int rotationSamplerIdx = animation.samplers.size() - 1;

            // Create animation channels
            tinygltf::AnimationChannel translationChannel;
            translationChannel.sampler = translationSamplerIdx;
            translationChannel.target_node = targetBoneIndex;
            translationChannel.target_path = "translation";

            animation.channels.push_back(translationChannel);

            tinygltf::AnimationChannel rotationChannel;
            rotationChannel.sampler = rotationSamplerIdx;
            rotationChannel.target_node = targetBoneIndex;
            rotationChannel.target_path = "rotation";

            animation.channels.push_back(rotationChannel);
        }

        gltf->animations.push_back(animation);

        return &gltf->animations.back();
    }
}