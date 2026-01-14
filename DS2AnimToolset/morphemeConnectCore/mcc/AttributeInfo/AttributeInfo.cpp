#include "AttributeInfo.h"
#include "mcd/Attribute/SingleValue/FloatAttribute.h"
#include "mcd/Attribute/SingleValue/BoolAttribute.h"
#include "mcd/Attribute/SingleValue/IntAttribute.h"
#include "mcd/Attribute/SingleValue/StringAttribute.h"
#include "mcd/Attribute/AnimationSet/AnimationSetAttribute.h"
#include "mcd/Attribute/AnimationTake/AnimationTakeAttribute.h"
#include "mcd/Attribute/Array/BoolArrayAttribute.h"
#include "mcd/Attribute/Array/FloatArrayAttribute.h"
#include "mcd/Attribute/Array/IntArrayAttribute.h"
#include "mcd/Attribute/RigChannelName/RigChannelNameAttribute.h"

#include "mcc/MorphemeDocument/MorphemeDocument.h"
#include "extern.h"

namespace mcc
{
	AttributeInfo::AttributeInfo(MMAttribute* manifestAttribute) : m_manifestAttribute(manifestAttribute)
	{
		m_attribType = getManifestDataType(manifestAttribute->getType());
		m_perAnimSet = manifestAttribute->isPerAnimSet();

		if (m_attribType == AttributeType::kAnimationTake)
			m_perAnimSet = true;

		m_syncWithRigChannels = manifestAttribute->isSyncWithRigChannels();
	}

	std::string AttributeInfo::getManifestDataTypeName(AttributeType type)
	{
		switch (type)
		{
		case AttributeType::kFloat:				return "float";
		case AttributeType::kBool:				return "bool";
		case AttributeType::kInt:				return "int";
		case AttributeType::kString:			return "string";
		case AttributeType::kAnimationTake:		return "animationTake";
		case AttributeType::kRef:				return "ref";
		case AttributeType::kBoolArray:			return "boolArray";
		case AttributeType::kFloatArray:		return "floatArray";
		case AttributeType::kIntArray:			return "intArray";
		case AttributeType::kRefArray:			return "refArray";
		case AttributeType::kControlParameter:	return "controlParameter";
		case AttributeType::kRequest:			return "request";
		case AttributeType::kRigChannelNames:	return "rigChannelName";
		default:
			throw std::runtime_error(
				"AttributeInfo::getManifestDataTypeName() - Unhandled attribute type " +
				std::to_string(static_cast<uint32_t>(type))
			);
		}
	}

	AttributeInfo::AttributeType AttributeInfo::getManifestDataType(std::string typeName)
	{
		if (typeName == "float")
			return AttributeType::kFloat;
		else if (typeName == "bool")
			return AttributeType::kBool;
		else if (typeName == "int")
			return AttributeType::kInt;
		else if (typeName == "string")
			return AttributeType::kString;
		else if (typeName == "animationTake")
			return AttributeType::kAnimationTake;
		else if (typeName == "boolArray")
			return AttributeType::kBoolArray;
		else if (typeName == "floatArray")
			return AttributeType::kFloatArray;
		else if (typeName == "intArray")
			return AttributeType::kIntArray;
		else if (typeName == "rigChannelName")
			return AttributeType::kRigChannelNames;
		else if (typeName == "request")
			return AttributeType::kRequest;

		throw std::runtime_error("AttributeInfo::getManifestDataType() - Unhandled attribute type: " + typeName);
	}

	mcd::Attribute* AttributeInfo::createNormalDatabaseAttribute(db::TypedNodeContainer<mcd::Attribute>* parent)
	{
		mcd::Attribute* attribute = nullptr;

		switch (m_attribType)
		{
		case mcc::AttributeInfo::AttributeType::kBool:
			attribute = new mcd::BoolAttribute(parent, m_manifestAttribute->getName(), m_manifestAttribute->getBoolValue());
			break;
		case mcc::AttributeInfo::AttributeType::kFloat:
			attribute = new mcd::FloatAttribute(parent, m_manifestAttribute->getName(), m_manifestAttribute->getFloatValue());
			break;
		case mcc::AttributeInfo::AttributeType::kInt:
			attribute = new mcd::IntAttribute(parent, m_manifestAttribute->getName(), m_manifestAttribute->getIntValue());
			break;
		case mcc::AttributeInfo::AttributeType::kString:
			attribute = new mcd::StringAttribute(parent, m_manifestAttribute->getName(), m_manifestAttribute->getStringValue());
			break;
		case mcc::AttributeInfo::AttributeType::kAnimationTake:
			attribute = new mcd::AnimationTakeAttribute(parent, m_manifestAttribute->getName());
			break;
		case mcc::AttributeInfo::AttributeType::kBoolArray:
			attribute = new mcd::BoolArrayAttribute(parent, m_manifestAttribute->getName());

			for (int i = 0; i < m_manifestAttribute->size(); i++)
				dynamic_cast<mcd::BoolArrayAttribute*>(attribute)->addElement(m_manifestAttribute->getBoolValue(i));
			break;
		case mcc::AttributeInfo::AttributeType::kFloatArray:
			attribute = new mcd::FloatArrayAttribute(parent, m_manifestAttribute->getName());

			for (int i = 0; i < m_manifestAttribute->size(); i++)
				dynamic_cast<mcd::FloatArrayAttribute*>(attribute)->addElement(m_manifestAttribute->getFloatValue(i));
			break;
		case mcc::AttributeInfo::AttributeType::kIntArray:
			attribute = new mcd::IntArrayAttribute(parent, m_manifestAttribute->getName());

			for (int i = 0; i < m_manifestAttribute->size(); i++)
				dynamic_cast<mcd::IntArrayAttribute*>(attribute)->addElement(m_manifestAttribute->getIntValue(i));
			break;
		case mcc::AttributeInfo::AttributeType::kRigChannelNames:
			attribute = new mcd::RigChannelNameAttribute(parent, m_manifestAttribute->getName());
			break;
		case mcc::AttributeInfo::AttributeType::kRequest:
			throw std::runtime_error("Request attributes are not yet implemented");
			break;
		default:
			throw std::runtime_error("Unhandled attribute type: " + getManifestDataTypeName(m_attribType));
			break;
		}

		if (m_syncWithRigChannels)
		{
			attribute->addAttribute(new db::BoolAttribute(attribute, "SyncWithRigChannels", true));
		}

		if (m_perAnimSet)
		{
			parent->remove(attribute);
			return new mcd::AnimationSetAttribute(parent, m_manifestAttribute->getName(), g_doc->getActiveAnimationSetName(), attribute);
		}

		return attribute;
	}

	mcd::Attribute* AttributeInfo::createAnimationSetAttribute(db::TypedNodeContainer<mcd::Attribute>* parent)
	{
		return new mcd::AnimationSetAttribute(parent, m_manifestAttribute->getName(), g_doc->getActiveAnimationSetName(), createNormalDatabaseAttribute(parent));
	}

	mcd::Attribute* AttributeInfo::createDatabaseAttribute(db::TypedNodeContainer<mcd::Attribute>* parent)
	{
		mcd::Attribute* attribute = nullptr;

		if (m_perAnimSet)
			attribute = createAnimationSetAttribute(parent);

		attribute = createNormalDatabaseAttribute(parent);

		parent->add(attribute);

		return attribute;
	}
}
