#include <algorithm>
#include "GLTFTranslator.h"
#include "MorphemeEditor.h"

namespace
{
    void calculateMinMax(const std::vector<Vector3>& vectors, Vector3& outMin, Vector3& outMax) {
        if (vectors.empty()) {
            // Handle empty input case
            outMin = Vector3(0, 0, 0);
            outMax = Vector3(0, 0, 0);
            return;
        }

        // Initialize min and max with extreme values
        outMin = Vector3(FLT_MAX,
            FLT_MAX,
            FLT_MAX);

        outMax = Vector3(-FLT_MAX,
            -FLT_MAX,
            -FLT_MAX);

        // Iterate through the vector and update min and max
        for (const auto& vec : vectors) {
            outMin.x = std::fmin(outMin.x, vec.x);
            outMin.y = std::fmin(outMin.y, vec.y);
            outMin.z = std::fmin(outMin.z, vec.z);

            outMax.x = std::fmax(outMax.x, vec.x);
            outMax.y = std::fmax(outMax.y, vec.y);
            outMax.z = std::fmax(outMax.z, vec.z);
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

        // Create the root node
        tinygltf::Node rootNode;
        rootNode.name = model->getModelName();

        Quaternion rootRot = Quaternion::CreateFromAxisAngle(Vector3::Right, -DirectX::XM_PIDIV2);
        rootNode.translation = { 0.f, 0.f, 0.f };
        rootNode.rotation = { rootRot.x, rootRot.y, rootRot.z, rootRot.w }; //Z-up

        gltfModel->nodes.push_back(rootNode);

        if (includeMeshes)
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

        // Store buffer in the glTF model
        gltf->buffers.push_back(buffer);

        // Create BufferViews for positions, normals, and indices
        tinygltf::BufferView positionBufferView;
        positionBufferView.buffer = gltf->buffers.size() - 1;
        positionBufferView.byteOffset = positionOffset;
        positionBufferView.byteLength = positionDataSize;
        positionBufferView.target = TINYGLTF_TARGET_ARRAY_BUFFER;
        gltf->bufferViews.push_back(positionBufferView);

        tinygltf::BufferView normalBufferView;
        normalBufferView.buffer = gltf->buffers.size() - 1;
        normalBufferView.byteOffset = normalOffset;
        normalBufferView.byteLength = normalDataSize;
        normalBufferView.target = TINYGLTF_TARGET_ARRAY_BUFFER;
        gltf->bufferViews.push_back(normalBufferView);

        tinygltf::BufferView indexBufferView;
        indexBufferView.buffer = gltf->buffers.size() - 1;
        indexBufferView.byteOffset = indexOffset;
        indexBufferView.byteLength = indexDataSize;
        indexBufferView.target = TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER;
        gltf->bufferViews.push_back(indexBufferView);

        // Create Accessors for positions, normals, and indices
        tinygltf::Accessor positionAccessor;
        positionAccessor.bufferView = gltf->bufferViews.size() - 3; // Position buffer view
        positionAccessor.byteOffset = 0; // Starts at the beginning of its buffer view
        positionAccessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
        positionAccessor.type = TINYGLTF_TYPE_VEC3;
        positionAccessor.count = vertices.size();

        // Set min and max bounds
        positionAccessor.minValues = { minBound.x, minBound.y, minBound.z };
        positionAccessor.maxValues = { maxBound.x, maxBound.y, maxBound.z };

        gltf->accessors.push_back(positionAccessor);

        tinygltf::Accessor normalAccessor;
        normalAccessor.bufferView = gltf->bufferViews.size() - 2; // Normal buffer view
        normalAccessor.byteOffset = 0; // Starts at the beginning of its buffer view
        normalAccessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
        normalAccessor.type = TINYGLTF_TYPE_VEC3;
        normalAccessor.count = normals.size();
        gltf->accessors.push_back(normalAccessor);

        tinygltf::Accessor indexAccessor;
        indexAccessor.bufferView = gltf->bufferViews.size() - 1; // Index buffer view
        indexAccessor.byteOffset = 0;
        indexAccessor.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT;
        indexAccessor.type = TINYGLTF_TYPE_SCALAR;
        indexAccessor.count = indices.size();
        gltf->accessors.push_back(indexAccessor);

        // Create a Primitive to reference the accessors
        tinygltf::Primitive primitive;
        primitive.attributes["POSITION"] = gltf->accessors.size() - 3; // Position accessor index
        primitive.attributes["NORMAL"] = gltf->accessors.size() - 2;   // Normal accessor index
        primitive.indices = gltf->accessors.size() - 1;               // Index accessor index
        primitive.mode = TINYGLTF_MODE_TRIANGLES;

        // Create a Mesh and add the primitive
        tinygltf::Mesh mesh;
        mesh.name = "Mesh_" + std::to_string(meshIndex);
        mesh.primitives.push_back(primitive);

        // Add the mesh to the model
        gltf->meshes.push_back(mesh);

        tinygltf::Node node;
        node.name = "MeshNode_" + std::to_string(meshIndex);
        node.mesh = gltf->meshes.size() - 1; // Index of the mesh

        // Add node to the model
        gltf->nodes.push_back(node);

        // Add node as a child of the root node
        gltf->nodes[0].children.push_back(gltf->nodes.size() - 1);

        // Return a pointer to the newly added mesh
        return &gltf->meshes.back();
    }
}