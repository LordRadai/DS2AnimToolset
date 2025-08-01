#pragma once
#include "mcc/MorphemeManifest/MMManifestItem/Attribute/MMAttribute.h"
#include "DataTypes.h"
#include "mcd/Attribute/Attribute.h"
#include "NMDBExtensions/TypedNodeContainer.inl"
#include <string>

namespace mcc
{
	class AttributeInfo
	{
	public:
		enum class AttributeType
		{
			kBool,
			kFloat,
			kInt,
			kString,
			kAnimationTake,
			kRef,
			kBoolArray,
			kFloatArray,
			kIntArray,
			kRefArray,
			kControlParameter,
			kRequest,
			kNumAttributeTypes
		};

	private:
		AttributeType m_attribType;
		std::unique_ptr<mcc::MMAttribute> m_manifestAttribute;
		bool m_perAnimSet;
	public:
		AttributeInfo(MMAttribute* manifestAttribute);

		AttributeType getDataType() const { return m_attribType; }

		AttributeType getManifestDataType(std::string typeName) const;

		mcd::Attribute* createNormalDatabaseAttribute(db::TypedNodeContainer<mcd::Attribute>* parent);
		mcd::Attribute* createAnimationSetAttribute(db::TypedNodeContainer<mcd::Attribute>* parent);
		mcd::Attribute* createDatabaseAttribute(db::TypedNodeContainer<mcd::Attribute>* parent);
	};
}
