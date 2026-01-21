#pragma once
#include "Attribute/SingleValue/BoolAttribute.h"
#include "Attribute/SingleValue/IntAttribute.h"
#include "Attribute/SingleValue/FloatAttribute.h"
#include "Attribute/SingleValue/DoubleAttribute.h"
#include "Attribute/SingleValue/StringAttribute.h"
#include "Attribute/SingleValue/EnumAttribute.h"
#include "Attribute/SingleValue/Vector3Attribute.h"
#include "Attribute/SingleValue/Matrix34Attribute.h"
#include "Attribute/SingleValue/PointerAttribute.h"
#include "Attribute/SingleValue/QuaternionAttribute.h"

#include "Attribute/Array/BoolArrayAttribute.h"
#include "Attribute/Array/IntArrayAttribute.h"
#include "Attribute/Array/FloatArrayAttribute.h"
#include "Attribute/Array/DoubleArrayAttribute.h"
#include "Attribute/Array/StringArrayAttribute.h"
#include "Attribute/Array/EnumArrayAttribute.h"
#include "Attribute/Array/AttributeArray.h"
#include "Attribute/Array/Vector3ArrayAttribute.h"
#include "Attribute/Array/Matrix34ArrayAttribute.h"
#include "Attribute/Array/QuaternionArrayAttribute.h"
#include "Attribute/Array/PointerArrayAttribute.h"

#include "Node/Node.h"
#include "NodeContainer/NodeContainer.h"

namespace db
{
	static class AttributeFactory
	{
	public:
		/**
		 * \brief Create an attribute based on the type string.
		 * Used to load from XML.
		 * 
		 * \param typeName The name of the attribute type to create.
		 * \return A pointer to the created attribute, or nullptr if the type is unknown.
		 */
		static Attribute* makeAttributeFromTypeString(const std::string& typeName);
	};
}