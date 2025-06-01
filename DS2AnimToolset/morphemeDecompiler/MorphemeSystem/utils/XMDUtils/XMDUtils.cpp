#include "XMDUtils.h"

namespace XMDUtils
{
	XMD::XCamera* createCamera(XMD::XModel* xmd, const char* name, float aspectRatio, float nearZ, float farZ)
	{
		XMD::XCamera* camera = static_cast<XMD::XCamera*>(xmd->CreateNode(XMD::XFn::Camera));

		camera->SetName(name);
		camera->SetAspect(aspectRatio);
		camera->SetNear(nearZ);
		camera->SetFar(farZ);
		camera->SetHorizontalAspect(aspectRatio * 0.63333f);
		camera->SetVerticalAspect(aspectRatio * 0.44f);

		return camera;
	}

	XMD::XLambert* createLambert(XMD::XModel* xmd, const char* name)
	{
		XMD::XLambert* lambert = static_cast<XMD::XLambert*>(xmd->CreateNode(XMD::XFn::Lambert));
		lambert->SetName(name);

		lambert->SetDiffuse(XMDUtils::createShadingNode(xmd, XMD::XColour(0.5f, 0.5f, 0.5f, 1.f)));
		lambert->SetAmbient(XMDUtils::createShadingNode(xmd, XMD::XColour(0.f, 0.f, 0.f, 1.f)));
		lambert->SetTransparency(XMDUtils::createShadingNode(xmd, XMD::XColour(0.f, 0.f, 0.f, 1.f)));

		return lambert;
	}

	XMD::XShadingNode* createShadingNode(XMD::XModel* xmd, XMD::XColour color)
	{
		XMD::XShadingNode* shading = static_cast<XMD::XShadingNode*>(xmd->CreateNode(XMD::XFn::ShadingNode));

		shading->SetProperty(0, color);

		return shading;
	}

	XMD::XLocator* createLocator(XMD::XModel* xmd, const char* name)
	{
		XMD::XLocator* locator = static_cast<XMD::XLocator*>(xmd->CreateNode(XMD::XFn::Locator));

		locator->SetName(name);

		return locator;
	}

	XMD::XLocator* createJointLocator(XMD::XModel* xmd, XMD::XJoint* joint)
	{
		char locatorName[256];
		sprintf_s(locatorName, "%sshape", joint->GetName());

		return XMDUtils::createLocator(xmd, locatorName);
	}

	XMD::XLocator* createBoneLocator(XMD::XModel* xmd, XMD::XBone* bone)
	{
		char locatorName[256];
		sprintf_s(locatorName, "%sshape", bone->GetName());

		return XMDUtils::createLocator(xmd, locatorName);
	}
}