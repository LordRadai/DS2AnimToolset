#include <algorithm>
#include "GLTFTranslator.h"
#include "MorphemeEditor.h"
#include "extern.h"
#include "RCore.h"
#include "utils/utils.h"

namespace
{
    std::vector<Vector3> getVertices(FlverModel* model, int meshIdx)
    {
        std::vector<FlverModel::SkinnedVertex> skinnedVertices = model->getBindPoseSkinnedVertices(meshIdx);
        std::vector<Vector3> pointList;
        pointList.reserve(skinnedVertices.size());

        for (size_t i = 0; i < skinnedVertices.size(); i++)
            pointList.push_back(Vector3::Transform(skinnedVertices[i].vertexData.position, Matrix::CreateRotationZ(DirectX::XM_PI)));

        return pointList;
    }

    std::vector<Vector3> getNormals(FlverModel* model, int meshIdx)
    {
        std::vector<FlverModel::SkinnedVertex> skinnedVertices = model->getBindPoseSkinnedVertices(meshIdx);

        std::vector<Vector3> normalList;
        normalList.reserve(skinnedVertices.size());

        for (size_t i = 0; i < skinnedVertices.size(); i++)
            normalList.push_back(Vector3::TransformNormal(skinnedVertices[i].vertexData.normal, Matrix::CreateRotationZ(DirectX::XM_PI)));

        return normalList;
    }

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

    void createSphere(tinygltf::Model* gltf, std::string name, Vector3 center, float radius, int latitudeDivisions = 50, int longitudeDivisions = 50, DirectX::XMVECTORF32 color = DirectX::Colors::Blue)
    {
        std::vector<Vector3> vertices;
        std::vector<Vector3> normals;
        std::vector<uint16_t> indices;

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
                vertices.push_back(Vector3(x, y, z));

                // Add normal
                normals.push_back(Vector3(nx, ny, nz));
            }
        }

        // Generate the indices for the sphere's triangles
        for (uint16_t lat = 0; lat < latitudeDivisions; ++lat) {
            for (uint16_t lon = 0; lon < longitudeDivisions; ++lon) {
                uint16_t first = (lat * (longitudeDivisions + 1)) + lon;
                uint16_t second = first + longitudeDivisions + 1;

                // Create two triangles for each quad
                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }

        // Add the buffers
        tinygltf::Buffer buffer;
        buffer.name = "Buffer_" + name;
        buffer.data.insert(buffer.data.end(), reinterpret_cast<const unsigned char*>(vertices.data()), reinterpret_cast<const unsigned char*>(vertices.data() + vertices.size()));
        buffer.data.insert(buffer.data.end(), reinterpret_cast<const unsigned char*>(normals.data()), reinterpret_cast<const unsigned char*>(normals.data() + normals.size()));
        buffer.data.insert(buffer.data.end(), reinterpret_cast<const unsigned char*>(indices.data()), reinterpret_cast<const unsigned char*>(indices.data() + indices.size()));
        gltf->buffers.push_back(buffer);

        // Create the mesh and the corresponding buffer
        tinygltf::Mesh mesh;

        ptrdiff_t positionOffset = 0;
        ptrdiff_t normalsOffset = vertices.size() * sizeof(Vector3);
        ptrdiff_t indicesOffset = normalsOffset + normals.size() * sizeof(Vector3);

        // Create buffer views for position, normals, and indices
        createBufferView(gltf, "VertexBufferView_" + name, gltf->buffers.size() - 1, positionOffset, vertices.size() * sizeof(Vector3), TINYGLTF_TARGET_ARRAY_BUFFER);
        createAccessor(gltf, "VertexAccessor_" + name, gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_VEC3, vertices.size());

        const int vertexIdx = gltf->accessors.size() - 1;

        createBufferView(gltf, "NormalsBufferView_" + name, gltf->buffers.size() - 1, normalsOffset, normals.size() * sizeof(Vector3), TINYGLTF_TARGET_ARRAY_BUFFER);
        createAccessor(gltf, "NormalsAccessor_" + name, gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_VEC3, normals.size());

        const int normalIdx = gltf->accessors.size() - 1;

        createBufferView(gltf, "IndexBufferView_" + name, gltf->buffers.size() - 1, indicesOffset, indices.size() * sizeof(uint16_t), TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER);
        createAccessor(gltf, "IndexAccessor_" + name, gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, TINYGLTF_TYPE_SCALAR, indices.size());

        const int indicesIdx = gltf->accessors.size() - 1;

        tinygltf::Material material;

        material.name = "DebugMaterial";
        material.pbrMetallicRoughness.baseColorFactor = { color[0], color[1], color[2], color[3] };

        gltf->materials.push_back(material);
        const int materialIndex = gltf->materials.size() - 1;

        // Create a mesh object
        tinygltf::Primitive primitive;
        primitive.attributes["POSITION"] = vertexIdx;
        primitive.attributes["NORMAL"] = normalIdx;
        primitive.mode = TINYGLTF_MODE_TRIANGLES;
        primitive.indices = indicesIdx;
        primitive.material = materialIndex;

        // Create the mesh
        mesh.name = name;
        mesh.primitives.push_back(primitive);

        gltf->meshes.push_back(mesh);
    }

    void createLine(tinygltf::Model* gltf, std::string name, Vector3 pointA, Vector3 pointB, DirectX::XMVECTORF32 color = DirectX::Colors::Blue)
    {
        Vector3 vertices[2] = { pointA, pointB };
        uint16_t indices[2] = { 0, 1 };

        // Add the buffers
        tinygltf::Buffer buffer;
        buffer.name = "Buffer_" + name;
        buffer.data.insert(buffer.data.end(), reinterpret_cast<const unsigned char*>(vertices), reinterpret_cast<const unsigned char*>(vertices + 2));
        buffer.data.insert(buffer.data.end(), reinterpret_cast<const unsigned char*>(indices), reinterpret_cast<const unsigned char*>(indices + 2));
        gltf->buffers.push_back(buffer);

        // Create the mesh and the corresponding buffer
        tinygltf::Mesh mesh;

        ptrdiff_t positionOffset = 0;
        ptrdiff_t indicesOffset = 2 * sizeof(Vector3);

        // Create buffer views for position, normals, and indices
        createBufferView(gltf, "VertexBufferView_" + name, gltf->buffers.size() - 1, positionOffset, 2 * sizeof(Vector3), TINYGLTF_TARGET_ARRAY_BUFFER);
        createAccessor(gltf, "VertexAccessor_" + name, gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_VEC3, 2);

        const int vertexIdx = gltf->accessors.size() - 1;

        createBufferView(gltf, "IndexBufferView_" + name, gltf->buffers.size() - 1, indicesOffset, 2 * sizeof(uint16_t), TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER);
        createAccessor(gltf, "IndexAccessor_" + name, gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, TINYGLTF_TYPE_SCALAR, 2);

        const int indicesIdx = gltf->accessors.size() - 1;

        tinygltf::Material material;

        material.name = "DebugMaterial";
        material.pbrMetallicRoughness.baseColorFactor = { color[0], color[1], color[2], color[3] };

        gltf->materials.push_back(material);
        const int materialIndex = gltf->materials.size() - 1;

        // Create a mesh object
        tinygltf::Primitive primitive;
        primitive.attributes["POSITION"] = vertexIdx;
        primitive.mode = TINYGLTF_MODE_LINE;
        primitive.indices = indicesIdx;
        primitive.material = materialIndex;

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

    // Adds a debug visualizer to all bones
    void addBoneVisuals(tinygltf::Model* gltf, MR::AnimRigDef* rig) 
    {
        // Iterate through all joints
        for (uint32_t boneID = 1; boneID < rig->getNumBones(); ++boneID) 
        {                
            tinygltf::Node* joint = getGltfNodeByName(gltf, rig->getBoneName(boneID));

            if (joint)
            {
                /*
                const int parentID = rig->getParentBoneIndex(boneID);

                if (parentID != -1)
                {
                    Matrix boneTransform = utils::NMDX::getTransformMatrix(*rig->getBindPoseBoneQuat(boneID), *rig->getBindPoseBonePos(boneID));
                    Matrix parentTransform = utils::NMDX::getTransformMatrix(*rig->getBindPoseBoneQuat(parentID), *rig->getBindPoseBonePos(parentID));

                    createLine(gltf, "BoneMarker_" + std::to_string(boneID), Vector3::Transform(Vector3::Zero, parentTransform.Invert()), Vector3::Transform(Vector3::Zero, boneTransform * parentTransform.Invert()));
                }
                else
                {
                    createSphere(gltf, "BoneMarker_" + std::to_string(boneID), Vector3::Zero, 0.05f);
                }
                */

                createSphere(gltf, "BoneMarker_" + std::to_string(boneID), Vector3::Zero, 0.01f);

                joint->mesh = gltf->meshes.size() - 1;
            }
        }
    }

    // Retrieve the data necessary for the skinning process from the flver model
    void getSkinningData(tinygltf::Model* gltf, std::vector<Matrix>& ibm, std::vector<int>& jointArray, std::vector<Vector4>& weights, std::vector<JointIndicesVec>& indices, FlverModel* model, int meshIndex)
    {
        // Make sure the input is empty before adding anything
        ibm.clear();
        jointArray.clear();
        weights.clear();
        indices.clear();

        std::vector<FlverModel::SkinnedVertex> skinnedVertices = model->getBindPoseSkinnedVertices(meshIndex);

        const MR::AnimRigDef* rig = model->getRig();

        // Convert flver bone indices to morpheme bone indices
        for (size_t i = 0; i < skinnedVertices.size(); i++)
        {
            Vector4 jointWeightData = Vector4::Zero;
            JointIndicesVec jointIndexData(-1, -1, -1, -1);
            for (size_t j = 0; j < 4; j++)
            {
                const int morphemeBoneID = model->getMorphemeBoneIdByFlverBoneId(skinnedVertices[i].boneIndices[j]);

                if (morphemeBoneID != -1)
                {
                    const tinygltf::Node* jointNode = getGltfNodeByName(gltf, model->getMorphemeBoneName(morphemeBoneID));

                    if (jointNode)
                    {
                        const int influenceArrayIndex = addBoneInfluence(jointArray, ibm, model, morphemeBoneID);

                        switch (j)
                        {
                        case 0:
                            jointWeightData.x = skinnedVertices[i].boneWeights[j];
                            jointIndexData.x = influenceArrayIndex;
                            break;
                        case 1:
                            jointWeightData.y = skinnedVertices[i].boneWeights[j];
                            jointIndexData.y = influenceArrayIndex;
                            break;
                        case 2:
                            jointWeightData.z = skinnedVertices[i].boneWeights[j];
                            jointIndexData.z = influenceArrayIndex;
                            break;
                        case 3:
                            jointWeightData.w = skinnedVertices[i].boneWeights[j];
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

    std::vector<FlverModel::SkinnedVertex> getMeshSkinnedVertices(FlverModel* model, int meshIdx)
    {
        std::vector<FlverModel::SkinnedVertex> skinnedVertices = model->getBindPoseSkinnedVertices(meshIdx);

        for (size_t i = 0; i < skinnedVertices.size(); i++)
        {
            skinnedVertices[i].vertexData.position = Vector3::Transform(skinnedVertices[i].vertexData.position, Matrix::CreateRotationZ(DirectX::XM_PI) * Matrix::CreateRotationX(DirectX::XM_PIDIV2));
            skinnedVertices[i].vertexData.normal = Vector3::Transform(skinnedVertices[i].vertexData.normal, Matrix::CreateRotationZ(DirectX::XM_PI) * Matrix::CreateRotationX(DirectX::XM_PIDIV2));
        }

        return skinnedVertices;
    }
}

namespace FT
{
    tinygltf::Model* GltfFileTranslator::createModel(MR::AnimRigDef* rig, FlverModel* model, bool includeMeshes)
    {
        tinygltf::Model* gltfModel = new tinygltf::Model;
        gltfModel->asset.version = "2.0";
        gltfModel->asset.generator = APPNAME_A;

        addRootNode(gltfModel, "SceneRoot");

        //Add all the bones
        for (uint32_t i = 1; i < rig->getNumBones(); i++)
            createJoint(gltfModel, rig, i);

        //Loop back over to add all children
        for (uint32_t i = 1; i < rig->getNumBones(); i++)
        {
            int jointIndex = getGltfNodeIndexByName(gltfModel, rig->getBoneName(i));
            int parentIndex = getGltfNodeIndexByName(gltfModel, rig->getBoneName(rig->getParentBoneIndex(i)));

            // If the parent bone was not found, add to the root node
            if (parentIndex == -1)
                parentIndex = 0;

            gltfModel->nodes[parentIndex].children.push_back(jointIndex);
        }

#ifdef _DEBUG
        addBoneVisuals(gltfModel, rig);
#endif

        if (includeMeshes && (model != nullptr))
        {
            for (uint32_t i = 0; i < model->getNumMeshes(); i++)
                createMesh(gltfModel, model, i);
        }

        return gltfModel;
    }

    tinygltf::Mesh* GltfFileTranslator::createMesh(tinygltf::Model* gltf, FlverModel* model, int meshIndex)
    {
        std::vector<FlverModel::SkinnedVertex> skinnedVertices = getMeshSkinnedVertices(model, meshIndex);

        Vector3 minBound, maxBound;

        std::vector<Vector3> verts = getVertices(model, meshIndex);

        calculateMinMax(verts, minBound, maxBound);

        std::vector<Vector3> normals = getNormals(model, meshIndex);

        std::vector<uint16_t> indices;
        indices.reserve(skinnedVertices.size());
        for (size_t i = 0; i < skinnedVertices.size(); i++)
            indices.push_back(i);

        // Create a buffer to hold all data (positions, normals, indices)
        tinygltf::Buffer buffer;
        buffer.name = "MeshBuffer_" + std::to_string(meshIndex);

        // Add vertex positions to the buffer
        size_t positionDataSize = verts.size() * sizeof(Vector3);
        size_t positionOffset = buffer.data.size();
        buffer.data.insert(buffer.data.end(),
            reinterpret_cast<const unsigned char*>(verts.data()),
            reinterpret_cast<const unsigned char*>(verts.data() + verts.size()));

        // Add vertex normals to the buffer
        size_t normalDataSize = normals.size() * sizeof(Vector3);
        size_t normalOffset = buffer.data.size();
        buffer.data.insert(buffer.data.end(),
            reinterpret_cast<const unsigned char*>(normals.data()),
            reinterpret_cast<const unsigned char*>(normals.data() + normals.size()));

        std::vector<int> jointArray;
        std::vector<Vector4> jointWeights;
        std::vector<JointIndicesVec> jointIndices;
        std::vector<Matrix> inverseBindMatrices;
        getSkinningData(gltf, inverseBindMatrices, jointArray, jointWeights, jointIndices, model, meshIndex);

        // Add joint indices to the buffer
        size_t jointDataSize = jointIndices.size() * sizeof(JointIndicesVec);
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

        // Add index data to the buffer
        size_t indexDataSize = indices.size() * sizeof(uint16_t);
        size_t indexOffset = buffer.data.size();
        buffer.data.insert(buffer.data.end(),
            reinterpret_cast<const unsigned char*>(indices.data()),
            reinterpret_cast<const unsigned char*>(indices.data() + indices.size()));

        gltf->buffers.push_back(buffer);

        // Vertices
        createBufferView(gltf, "VertexBufferView_" + std::to_string(meshIndex), gltf->buffers.size() - 1, positionOffset, positionDataSize, TINYGLTF_TARGET_ARRAY_BUFFER);
        tinygltf::Accessor* positionAccessor = createAccessor(gltf, "VertexBufAccessor_" + std::to_string(meshIndex), gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_VEC3, verts.size());
        positionAccessor->minValues = { minBound.x, minBound.y, minBound.z };
        positionAccessor->maxValues = { maxBound.x, maxBound.y, maxBound.z };

        const int positionDataIdx = gltf->accessors.size() - 1;

        // Normals
        createBufferView(gltf, "NormalsBufferView_" + std::to_string(meshIndex), gltf->buffers.size() - 1, normalOffset, normalDataSize, TINYGLTF_TARGET_ARRAY_BUFFER);
        createAccessor(gltf, "NormalsBufAccessor_" + std::to_string(meshIndex), gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_VEC3, normals.size());

        const int normalsDataIdx = gltf->accessors.size() - 1;

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

        // Vertex indices
        createBufferView(gltf, "IndexBufferView_" + std::to_string(meshIndex), gltf->buffers.size() - 1, indexOffset, indexDataSize, TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER);
        createAccessor(gltf, "IndexBufAccessor_" + std::to_string(meshIndex), gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, TINYGLTF_TYPE_SCALAR, indices.size());

        const int indicesDataIdx = gltf->accessors.size() - 1;

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

    tinygltf::Node* GltfFileTranslator::createJoint(tinygltf::Model* gltf, const MR::AnimRigDef* rig, const int boneID)
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

    tinygltf::Animation* GltfFileTranslator::createAnimation(tinygltf::Model* gltf, AnimObject* animObj, const char* takeName, int fps)
    {
        const MR::AnimationSourceHandle* animHandle = animObj->getHandle();

        if (animHandle == nullptr)
            return nullptr;

        const MR::AnimRigDef* rig = animHandle->getRig();

        tinygltf::Buffer buffer;

        const int numKeyframes = fps * animHandle->getDuration();

        std::vector<float> timeData;
        timeData.reserve(numKeyframes);
        for (size_t i = 0; i < numKeyframes; i++)
            timeData.push_back(RMath::frameToTime(i, fps));

        tinygltf::Animation animation;
        animation.name = takeName;

        const int rootBoneID = rig->getCharacterRootBoneIndex();
        const int trajecotryBoneID = rig->getTrajectoryBoneIndex();

        for (uint32_t channelID = 1; channelID < rig->getNumBones(); channelID++)
        {
            const int targetBoneIndex = getGltfNodeIndexByName(gltf, rig->getBoneName(channelID));
            std::vector<Vector3> translationData;
            std::vector<Quaternion> rotationData;

            for (uint32_t i = 0; i < numKeyframes; i++)
            {
                float animTime = RMath::frameToTime(i, fps);
                animObj->setAnimTime(animTime);

                Matrix transform = animObj->getTransformAtTime(animTime, channelID);

                if (channelID == rootBoneID)
                {
                    const int parentID = rig->getParentBoneIndex(rootBoneID);
                    transform = animObj->getTransformAtTime(animTime, channelID) * animObj->getTransformAtTime(animTime, parentID) * animObj->getTransformAtTime(animTime, trajecotryBoneID);
                }

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
            tinygltf::Accessor* timeAccessor = createAccessor(gltf, "TimeDataAccessor_" + std::to_string(channelID), gltf->bufferViews.size() - 1, 0, TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_SCALAR, static_cast<unsigned int>(timeData.size()));
            timeAccessor->minValues = { timeData[0] };
            timeAccessor->maxValues = { timeData.back() };

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

    bool GltfFileTranslator::exportModel(Character* character)
    {
        FlverModel* model = character->getCharacterModelCtrl()->getModel();
        MR::AnimRigDef* rig = character->getRig(0);

        tinygltf::Model* gltfModel = createModel(rig, model, true);

        tinygltf::Scene scene;
        scene.nodes.push_back(0); // Root node of the model

        gltfModel->scenes.push_back(scene);
        gltfModel->defaultScene = 0;

        std::string modelOutPath = RString::toNarrow(character->getCharacterName()) + ".gltf";

        tinygltf::TinyGLTF gltfWriter;
        bool success = gltfWriter.WriteGltfSceneToFile(gltfModel, modelOutPath, false, true, true, false);

        delete gltfModel;

        return success;
    }

    bool GltfFileTranslator::exportAnimation(Character* character, std::wstring path, int animSetIdx, int animIdx, int fps, bool includeModel)
    {
        MorphemeCharacterDef* characterDef = character->getMorphemeCharacterDef();
        AnimObject* anim = characterDef->getAnimation(animSetIdx, animIdx);
        int animId = anim->getAnimID();

        g_appLog->debugMessage(MsgLevel_Info, "\tExporting animation \"%s\" to GLTF (%ws)\n", anim->getAnimName(), character->getCharacterName().c_str());

        FlverModel* model = character->getCharacterModelCtrl()->getModel();
        MR::AnimRigDef* rig = character->getRig(0);

        tinygltf::Model* gltfModel = createModel(rig, model, includeModel);
        createAnimation(gltfModel, anim, characterDef->getAnimFileLookUp()->getTakeName(animId), fps);

        tinygltf::Scene scene;
        scene.nodes.push_back(0); // Root node of the model

        gltfModel->scenes.push_back(scene);
        gltfModel->defaultScene = 0;

        std::string outPath = RString::toNarrow(path) + RString::removeExtension(characterDef->getAnimFileLookUp()->getSourceFilename(animId)) + ".gltf";

        tinygltf::TinyGLTF gltfWriter;
        bool success = gltfWriter.WriteGltfSceneToFile(gltfModel, outPath, false, true, true, false);

        delete gltfModel;

        return success;
    }
}