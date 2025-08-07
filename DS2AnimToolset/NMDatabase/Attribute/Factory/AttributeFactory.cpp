#include "AttributeFactory.h"

namespace db
{
	Attribute* AttributeFactory::makeAttributeFromTypeString(const std::string& typeName)
	{
		if (typeName == "bool") return new BoolAttribute(nullptr, "", false);
		if (typeName == "int") return new IntAttribute(nullptr, "", 0);
		if (typeName == "float") return new FloatAttribute(nullptr, "", 0.f);
		if (typeName == "double") return new DoubleAttribute(nullptr, "", 0.0);
		if (typeName == "string") return new StringAttribute(nullptr, "", "");
		if (typeName == "enum") return new EnumAttribute(nullptr, "", "");
		if (typeName == "vector3") return new Vector3Attribute(nullptr, "", NMP::Vector3Zero());
		if (typeName == "matrix34") return new Matrix34Attribute(nullptr, "", NMP::Matrix34Zero());
		if (typeName == "pointer") return new PointerAttribute(nullptr, "", nullptr);
		if (typeName == "quaternion") return new QuaternionAttribute(nullptr, "", NMP::QuatIdentity());
		if (typeName == "boolArray") return new BoolArrayAttribute(nullptr, "");
		if (typeName == "intArray") return new IntArrayAttribute(nullptr, "");
		if (typeName == "floatArray") return new FloatArrayAttribute(nullptr, "");
		if (typeName == "doubleArray") return new DoubleArrayAttribute(nullptr, "");
		if (typeName == "stringArray") return new StringArrayAttribute(nullptr, "");
		if (typeName == "enumArray") return new EnumArrayAttribute(nullptr, "");
		if (typeName == "attributeArray") return new AttributeArray(nullptr, "");
		if (typeName == "vector3Array") return new Vector3ArrayAttribute(nullptr, "");
		if (typeName == "matrix34Array") return new Matrix34ArrayAttribute(nullptr, "");
		//if (typeName == "pointerArray") return new PointerArrayAttribute(nullptr, "");
		if (typeName == "quaternionArray") return new QuaternionArrayAttribute(nullptr, "");

		return nullptr; // Unknown type
	}
}
