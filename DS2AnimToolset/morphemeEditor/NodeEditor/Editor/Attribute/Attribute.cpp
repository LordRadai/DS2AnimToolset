#include <stdexcept>

#include "Attribute.h"

#include "SingleValue/BoolAttribute.h"
#include "SingleValue/FloatAttribute.h"
#include "SingleValue/IntAttribute.h"
#include "SingleValue/RefAttribute.h"
#include "SingleValue/StringAttribute.h"
#include "SingleValue/EnumAttribute.h"

#include "Array/BoolArrayAttribute.h"
#include "Array/FloatArrayAttribute.h"
#include "Array/IntArrayAttribute.h"
#include "Array/RefArrayAttribute.h"
#include "Array/StringArrayAttribute.h"

#include "ControlParameterAttribute/ControlParameterAttribute.h"
#include "RequestAttribute/RequestAttribute.h"
#include "AnimationTakeAttribute/AnimationTakeAttribute.h"

namespace NodeEditor
{
	Attribute::Attribute(Entity* owner, const std::string& name, const std::string& type) : Entity(owner->getOwnerEditor(), name),
		m_owner(owner), m_type(type)
	{
	}

	Attribute* Attribute::createAttribute(Entity* owner, const std::string& name, const std::string& type)
	{
		if (type == "int")
			return new IntAttribute(owner, name);
		else if (type == "float")
			return new FloatAttribute(owner, name);
		else if (type == "bool")
			return new BoolAttribute(owner, name);
		else if (type == "string")
			return new StringAttribute(owner, name);
		else if (type == "ref")
			return new RefAttribute(owner, name);
		else if (type == "rigChannelName")
			return new StringAttribute(owner, name);
		else if (type == "intArray")
			return new IntArrayAttribute(owner, name);
		else if (type == "floatArray")
			return new FloatArrayAttribute(owner, name);
		else if (type == "boolArray")
			return new BoolArrayAttribute(owner, name);
		else if (type == "refArray")
			return new RefArrayAttribute(owner, name);
		else if (type == "stringArray")
			return new StringArrayAttribute(owner, name);
		else if (type == "controlParameter")
			return new ControlParameterAttribute(owner, name);
		else if (type == "request")
			return new RequestAttribute(owner, name);
		else if (type == "filename")
			return new StringAttribute(owner, name);
		else if (type == "animationTake")
			return new AnimationTakeAttribute(owner, name);
		else if (type == "enum")
			return new EnumAttribute(owner, name);
		else
			throw std::runtime_error("Attribute::createAttribute: Unknown attribute type '" + type + "'");
	}
}
