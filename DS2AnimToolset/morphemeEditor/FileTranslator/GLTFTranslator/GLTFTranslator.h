#include "tinygltf/tiny_gltf.h"
#include "FlverModel/FlverModel.h"
#include "AnimObject/AnimObject.h"
#include "Character/Character.h"
#include "morpheme/mrRig.h"

struct JointIndicesVec
{
	uint16_t x = -1;
	uint16_t y = -1;
	uint16_t z = -1;
	uint16_t w = -1;

	JointIndicesVec() {}
	JointIndicesVec(uint16_t x, uint16_t y, uint16_t z, uint16_t w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
};

namespace GLTFTranslator
{
	tinygltf::Model* createModel(MR::AnimRigDef* rig, FlverModel* model, bool includeMeshes);
	tinygltf::Mesh* createMesh(tinygltf::Model* gltf, FlverModel* model, int meshIndex);
	tinygltf::Node* createJoint(tinygltf::Model* gltf, const MR::AnimRigDef* rig, const int boneID);
	tinygltf::Animation* createAnimation(tinygltf::Model* gltf, AnimObject* animObj, const char* takeName, int fps);
}