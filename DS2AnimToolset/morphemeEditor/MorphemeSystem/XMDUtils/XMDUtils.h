#include "XMD/Info.h"
#include "XMD/Bone.h"
#include "XMD/Model.h"
#include "XMD/Mesh.h"
#include "XMD/AnimationTake.h"
#include "XMD/PolygonMesh.h"
#include "XMD/SkinCluster.h"
#include "XMD/ObjectSet.h"
#include "XMD/Instance.h"
#include "XMD/Camera.h"
#include "XMD/Lambert.h"
#include "XMD/ShadingNode.h"
#include "XMD/Locator.h"
#include "XMD/PoseStream.h"
#include "XMD/SampledStream.h"

namespace XMDUtils
{
	XMD::XCamera* createCamera(XMD::XModel* xmd, const char* name, float aspectRatio, float nearZ, float farZ);
	XMD::XLambert* createLambert(XMD::XModel* xmd, const char* name);
	XMD::XShadingNode* createShadingNode(XMD::XModel* xmd, XMD::XColour color);

	XMD::XLocator* createLocator(XMD::XModel* xmd, const char* name);

	XMD::XLocator* createJointLocator(XMD::XModel* xmd, XMD::XJoint* joint);

	XMD::XLocator* createBoneLocator(XMD::XModel* xmd, XMD::XBone* bone);
}