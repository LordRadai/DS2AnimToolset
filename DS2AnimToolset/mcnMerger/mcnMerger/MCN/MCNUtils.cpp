#include "MCNUtils.h"
#include <string>

namespace MCNUtils
{
	ME::NodeExportXML* getParentStateMachine(ME::NetworkDefExportXML* netDef, ME::NodeExportXML* node)
	{
		int parentIndex = node->getDownstreamParentID();

		while (parentIndex != MR::INVALID_NODE_ID)
		{
			ME::NodeExportXML* parent = static_cast<ME::NodeExportXML*>(netDef->getNode(parentIndex));

			if (parent->getTypeID() == NODE_TYPE_STATE_MACHINE)
				return parent;

			parentIndex = parent->getDownstreamParentID();
		}

		return nullptr;
	}

	std::string getNodeNameWithoutParent(ME::NodeExportXML* node)
	{
		std::string input = node->getName();
		std::string delimiter = "|";

		size_t pos = input.find(delimiter);

		if (pos != std::string::npos) {
			std::string name = input.substr(pos + delimiter.length());
			return name;
		}

		return input;
	}

	bool isNodeBlendTree(ME::NodeExportXML* nodeXml)
	{
		if ((nodeXml->getTypeID() == NODE_TYPE_STATE_MACHINE) || (nodeXml->getTypeID() == NODE_TYPE_NETWORK))
			return false;

		if (nodeXml->getNumCommonConditionSets() || nodeXml->getNumConditionSets())
			return true;

		return false;
	}

	bool isNodeControlParam(ME::NodeExportXML* nodeDef)
	{
		switch (nodeDef->getTypeID())
		{
		case NODE_TYPE_CP_BOOL:
		case NODE_TYPE_CP_INT:
		case NODE_TYPE_CP_UINT:
		case NODE_TYPE_CP_FLOAT:
		case NODE_TYPE_CP_VECTOR3:
		case NODE_TYPE_CP_VECTOR4:
			return true;
		default:
			return false;
		}

		return false;
	}

	std::string getAnimNodeName(ME::NodeExportXML* node, ME::AnimationLibraryXML* animLibrary)
	{
		assert(node->getTypeID() == NODE_TYPE_ANIM_EVENTS);

		int animIndex = 0;
		node->getDataBlock()->readAnimationId(animIndex, "AnimIndex");

		ME::AnimationEntryExportXML* anim = static_cast<ME::AnimationEntryExportXML*>(animLibrary->getAnimationSet(0)->getAnimationEntry(animIndex));
		std::string animName = RString::removeExtension(anim->getAnimationFilename());

		return animName;
	}

	std::string getNodeTypeName(ME::NodeExportXML* node)
	{
		switch (node->getTypeID())
		{
		case NODE_TYPE_NETWORK:
			return "Network";
		case NODE_TYPE_STATE_MACHINE:
			return "StateMachine";
		case NODE_TYPE_ANIMATION:
			return "Animation";
		case NODE_TYPE_ANIM_EVENTS:
			return "AnimWithEvents";
		case NODE_TYPE_FILTER_TRANSFORMS:
			return "FilterTransforms";
		case NODE_TYPE_BLEND_2:
			return "Blend2";
		case NODE_TYPE_BLEND_N:
			return "BlendN";
		case NODE_TYPE_SINGLEFRAME:
			return "SingleFrame";
		case NODE_TYPE_FREEZE:
			return "Freeze";
		case NODE_TYPE_BLEND_2X2:
			return "Blend2x2";
		case NODE_TYPE_BLEND_NXM:
			return "BlendNxM";
		case NODE_TYPE_BLEND_ALL:
			return "BlendAll";
		case NODE_TYPE_SUBTRACTIVE_BLEND:
			return "SubtrativeBlend";
		case NODE_TYPE_SCATTER_BLEND_1D:
			return "ScatterBlend1D";
		case NODE_TYPE_SCATTER_BLEND_2D:
			return "ScattedBlend2D";
		case NODE_TYPE_SCATTER_BLEND_3D:
			return "ScatterBlend3D";
		case NODE_TYPE_CP_OP_BOOLEAN:
			return "OperatorBoolean";
		case NODE_TYPE_CP_OP_FUNCTION:
			return "OperatorFunction";
		case NODE_TYPE_CP_OP_ARITHMETIC:
			return "OperatorArithmetic";
		case NODE_TYPE_CP_OP_ARITHMETIC_VECTOR3:
			return "OperatorVector3Arithmetic";
		case NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC:
			return "OperatorOneInputArithmetic";
		case NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC_VECTOR3:
			return "OperatorVector3OneInputArithmetic";
		case NODE_TYPE_CP_OP_NOISE_GEN:
			return "NoiseGen";
		case NODE_TYPE_FEATHER_BLEND_2:
			return "FeatherBlend2";
		case NODE_TYPE_APPLY_BIND_POSE:
			return "ApplyBindPose";
		case NODE_TYPE_PHYSICS_GROUPER:
			return "PhysicsGrouper";
		case NODE_TYPE_BEHAVIOUR_GROUPER:
			return "BehaviourGrouper";
		case NODE_TYPE_APPLY_GLOBAL_TIME:
			return "ApplyGlobalTime";
		case NODE_TYPE_APPLY_PHYSICS_JOINT_LIMITS:
			return "ApplyPhysicsJointLimits";
		case NODE_TYPE_SET_NON_PHYSICS_TRANSFORMS:
			return "SetNonPhysicsTransform";
		case NODE_TYPE_TWO_BONE_IK:
			return "TwoBoneIK";
		case NODE_TYPE_LOCK_FOOT:
			return "LockFoot";
		case NODE_TYPE_HEAD_LOOK:
			return "HeadLook";
		case NODE_TYPE_PHYSICS:
			return "Physics";
		case NODE_TYPE_BEHAVIOUR:
			return "Behaviour";
		case NODE_TYPE_HIPS_IK:
			return "HipsIK";
		case NODE_TYPE_PERFORMANCE_BEHAVIOUR:
			return "PerformanceBehaviour";
		case NODE_TYPE_PLAY_SPEED_MODIFIER:
			return "PlaySpeedModifier";
		case NODE_TYPE_SCALE_TO_DURATION:
			return "ScaleToDuration";
		case NODE_TYPE_EMIT_JOINT_CP_INFO:
			return "EmitJointCpInfo";
		case NODE_TYPE_EMIT_MESSAGE_ON_DISCRETE_EVENT:
			return "EmitMessageOnDiscreteEvent";
		case NODE_TYPE_CLOSEST_ANIM:
			return "ClosestAnim";
		case NODE_TYPE_SWITCH:
			return "Switch";
		case NODE_TYPE_SEQUENCE:
			return "Sequence";
		case NODE_TYPE_PASSTHROUGH:
			return "PassThrough";
		case NODE_MIRROR_TRANSFORMS_ID:
			return "MirrorTransforms";
		case NODE_TYPE_BASIC_UNEVEN_TERRAIN:
			return "BasicUnevenTerrain";
		case NODE_TYPE_PREDICTIVE_UNEVEN_TERRAIN:
			return "PredictiveUnevenTerrain";
		case NODE_TYPE_CP_OP_INT_TO_FLOAT:
			return "OperatorIntToFloat";
		case NODE_TYPE_CP_OP_RAY_CAST:
			return "RayCast";
		case NODE_TYPE_CP_OP_SMOOTH_FLOAT:
			return "SmoothFloat";
		case NODE_TYPE_CP_OP_RAMP_FLOAT:
			return "RampFloat";
		case NODE_TYPE_CP_OP_FLOATS_TO_VECTOR3:
			return "FloatsToVector3";
		case NODE_TYPE_CP_OP_FLOAT_TO_INT:
			return "FloatToInt";
		case NODE_TYPE_CP_OP_RANDOM_FLOAT:
			return "RandomFloat";
		case NODE_TYPE_CP_OP_PHYSICS_INFO:
			return "PhysicsInfo";
		case NODE_TYPE_CP_OP_CONVERT_TO_CHARACTER_SPACE:
			return "ConvertToCharacterSpace";
		case NODE_TYPE_CP_OP_VECTOR3_TO_FLOATS:
			return "Vector3ToFloat";
		case NODE_TYPE_CP_OP_VECTOR3_DOT:
			return "Vector3Dot";
		case NODE_TYPE_CP_OP_ORIENTATIONINFREEFALL:
			return "OrientationInFreeFall";
		case NODE_TYPE_CP_OP_HIT:
			return "OperatorHit";
		case NODE_TYPE_CP_OP_ROLLDOWNSTAIRS:
			return "OperatorRollDownStairs";
		case NODE_TYPE_CP_OP_APPLYIMPULSE:
			return "ApplyImpulse";
		case NODE_TYPE_CP_OP_CONTACTREPORTER:
			return "ContactReporter";
		case NODE_TYPE_CP_OP_VECTOR3_DISTANCE:
			return "Vector3Distance";
		case NODE_TYPE_CP_OP_VECTOR3_CROSSPRODUCT:
			return "Vector3CrossProduct";
		case NODE_TYPE_CP_OP_RATE_OF_CHANGE:
			return "RateOfChange";
		case NODE_TYPE_CP_OP_VECTOR3_ANGLE:
			return "Vector3Angle";
		case NODE_TYPE_CP_OP_VECTOR3_NORMALISE:
			return "Vecotr3Normalise";
		case NODE_TYPE_CP_OP_PHYSICALCONSTRAINT:
			return "PhysicalConstraint";
		case NODE_TYPE_CP_OP_FALLOVERWALL:
			return "FallOverWall";
		case NODE_TYPE_RETARGET:
			return "Retarget";
		case NODE_TYPE_SCALE_CHARACTER:
			return "ScaleCharacter";
		case NODE_TYPE_GUN_AIM_IK:
			return "GunAimIK";
		case NODE_TYPE_MODIFY_JOINT_TRANSFORM:
			return "ModifyJointTransform";
		case NODE_TYPE_MODIFY_TRAJECTORY_TRANSFORM:
			return "ModifyTrajectoryTransform";
		case NODE_TYPE_SMOOTH_TRANSFORMS:
			return "SmoothTransforms";
		case NODE_TYPE_TRAJECTORY_OVERRIDE:
			return "TrajectoryOverride";
		case NODE_TYPE_C_C_OVERRIDE:
			return "CharacterControllerOverride";
		case NODE_TYPE_LIMB_INFO:
			return "LimbInfo";
		case NODE_TYPE_EXPAND_LIMITS:
			return "ExpandLimits";
		case NODE_TYPE_TRANSIT:
			return "Transit";
		case NODE_TYPE_TRANSIT_SYNC_EVENTS:
			return "TransitSyncEvents";
		default:
			return "";
		}
	}

	std::string getNodeName(ME::NodeExportXML* node, ME::AnimationLibraryXML* animLibrary)
	{
		if (node->getTypeID() == NODE_TYPE_ANIM_EVENTS)
			return getAnimNodeName(node, animLibrary);

		std::string typeName = getNodeTypeName(node);

		char name[256];
		sprintf_s(name, "%s_%d", typeName.c_str(), node->getNodeID());

		return name;
	}

	tinyxml2::XMLElement* createBoolElement(tinyxml2::XMLElement* parent, const char* name, bool value)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);
		element->SetAttribute("type", "bool");

		element->SetText((int)value);

		return element;
	}

	tinyxml2::XMLElement* createVector3Element(tinyxml2::XMLElement* parent, std::string name, float x, float y, float z)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name.c_str());

		element->SetAttribute("type", "vector3");
		RXML::createFloatElement(element, "X", x);
		RXML::createFloatElement(element, "Y", y);
		RXML::createFloatElement(element, "Z", z);

		return element;
	}

	tinyxml2::XMLElement* createVector4Element(tinyxml2::XMLElement* parent, std::string name, float x, float y, float z, float w)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name.c_str());

		element->SetAttribute("type", "quaternion");
		RXML::createFloatElement(element, "X", x);
		RXML::createFloatElement(element, "Y", y);
		RXML::createFloatElement(element, "Z", z);
		RXML::createFloatElement(element, "W", w);

		return element;
	}

	tinyxml2::XMLElement* createQuaternionElement(tinyxml2::XMLElement* parent, std::string name, float x, float y, float z, float w)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name.c_str());

		element->SetAttribute("type", "quaternion");
		RXML::createFloatElement(element, "X", x);
		RXML::createFloatElement(element, "Y", y);
		RXML::createFloatElement(element, "Z", z);
		RXML::createFloatElement(element, "W", w);

		return element;
	}

	tinyxml2::XMLElement* createEnumElement(tinyxml2::XMLElement* parent, const char* name, const char* value)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);
		element->SetAttribute("type", "enum");

		element->SetText(value);

		return element;
	}

	tinyxml2::XMLElement* createStringElement(tinyxml2::XMLElement* parent, const char* name, const char* value)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);
		element->SetAttribute("type", "string");

		element->SetText(value);

		return element;
	}

	void addMatrixRow(tinyxml2::XMLElement* parent, NMP::Matrix34 matrix, int rowIdx)
	{
		char rowName[256];
		sprintf_s(rowName, "R%d", rowIdx);

		tinyxml2::XMLElement* row = parent->InsertNewChildElement(rowName);
		tinyxml2::XMLElement* c0 = row->InsertNewChildElement("C0");
		c0->SetText(matrix.r[rowIdx].x);

		tinyxml2::XMLElement* c1 = row->InsertNewChildElement("C1");
		c1->SetText(matrix.r[rowIdx].y);

		tinyxml2::XMLElement* c2 = row->InsertNewChildElement("C2");
		c2->SetText(matrix.r[rowIdx].z);
	}

	tinyxml2::XMLElement* createMatrix34Element(tinyxml2::XMLElement* parent, const char* name, NMP::Matrix34 value)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);
		element->SetAttribute("type", "matrix34");

		addMatrixRow(element, value, 0);
		addMatrixRow(element, value, 1);
		addMatrixRow(element, value, 2);
		addMatrixRow(element, value, 3);

		return element;
	}

	tinyxml2::XMLElement* createBlendTree(tinyxml2::XMLElement* parent, const char* name)
	{
		return createNodeElement(parent, "BlendTree", name);
	}

	tinyxml2::XMLElement* createNodeElement(tinyxml2::XMLElement* parent, const char* name, const char* nodeName)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);

		element->SetAttribute("name", nodeName);
		element->SetAttribute("type", "node");

		return element;
	}

	tinyxml2::XMLElement* createNodeContainerElement(tinyxml2::XMLElement* parent, const char* name)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);
		element->SetAttribute("type", "nodeContainer");

		return element;
	}

	tinyxml2::XMLElement* createPointerElement(tinyxml2::XMLElement* parent, const char* name, tinyxml2::XMLElement* ptrTo)
	{
		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);
		element->SetAttribute("type", "pointer");

		element->SetText(getMorphemeDBPointer(ptrTo).c_str());

		return element;
	}

	std::string getElementName(tinyxml2::XMLElement* element)
	{
		if (strcmp(element->Attribute("type"), "nodeContainer") == 0)
			return element->Name();

		return element->Attribute("name");
	}

	std::string getMorphemeDBPointer(tinyxml2::XMLElement* element)
	{
		if (element == nullptr)
			return "(null)";

		std::string pointer = getElementName(element);
		tinyxml2::XMLElement* parent = static_cast<tinyxml2::XMLElement*>(element->Parent());

		while (strcmp(parent->Name(), "MorphemeDB") != 0)
		{
			pointer = getElementName(parent) + "." + pointer;
			parent = static_cast<tinyxml2::XMLElement*>(parent->Parent());
		}

		pointer = "MorphemeDB." + pointer;

		return pointer;
	}
}