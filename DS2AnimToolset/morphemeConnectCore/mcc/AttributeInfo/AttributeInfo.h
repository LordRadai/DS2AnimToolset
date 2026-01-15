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
			kRigChannelNames,

			kNumAttributeTypes
		};

	private:
		AttributeType m_attribType;
		std::unique_ptr<mcc::MMAttribute> m_manifestAttribute;
		bool m_perAnimSet;
		bool m_syncWithRigChannels;
	public:
		AttributeInfo(MMAttribute* manifestAttribute);

		AttributeType getDataType() const { return m_attribType; }

		static std::string getManifestDataTypeName(AttributeType type);
		static AttributeType getManifestDataType(std::string typeName);

		mcd::Attribute* createNormalDatabaseAttribute(db::TypedNodeContainer<mcd::Attribute>* parent);
		mcd::Attribute* createAnimationSetAttribute(db::TypedNodeContainer<mcd::Attribute>* parent);
		mcd::Attribute* createDatabaseAttribute(db::TypedNodeContainer<mcd::Attribute>* parent);
	};
}
