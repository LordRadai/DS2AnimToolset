#include "tinygltf/tiny_gltf.h"
#include "FlverModel/FlverModel.h"
#include "AnimObject/AnimObject.h"
#include "Character/Character.h"
#include "morpheme/mrRig.h"

namespace GLTFTranslator
{
	tinygltf::Model* createModel(MR::AnimRigDef* rig, FlverModel* model, bool includeMeshes);
	tinygltf::Mesh* createMesh(tinygltf::Model* gltf, FlverModel* model, int meshIndex);
}