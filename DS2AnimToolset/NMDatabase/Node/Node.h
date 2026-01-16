#pragma once
#include "RCore.h"
#include "Attribute/CompoundAttribute/CompoundAttribute.h"
#include "Attribute/SingleValue/BoolAttribute.h"
#include "Attribute/SingleValue/FloatAttribute.h"
#include "Attribute/SingleValue/DoubleAttribute.h"
#include "Attribute/SingleValue/IntAttribute.h"
#include "Attribute/SingleValue/StringAttribute.h"
#include "Attribute/SingleValue/EnumAttribute.h"
#include "Attribute/SingleValue/Matrix34Attribute.h"
#include "Attribute/SingleValue/QuaternionAttribute.h"
#include "Attribute/SingleValue/Vector3Attribute.h"
#include "Attribute/SingleValue/PointerAttribute.h"

#include "Attribute/Array/BoolArrayAttribute.h"
#include "Attribute/Array/FloatArrayAttribute.h"
#include "Attribute/Array/DoubleArrayAttribute.h"
#include "Attribute/Array/IntArrayAttribute.h"
#include "Attribute/Array/StringArrayAttribute.h"
#include "Attribute/Array/EnumArrayAttribute.h"
#include "Attribute/Array/Matrix34ArrayAttribute.h"
#include "Attribute/Array/QuaternionArrayAttribute.h"
#include "Attribute/Array/Vector3ArrayAttribute.h"

namespace db
{
	class Node : public CompoundAttribute
	{
	protected:
		std::unique_ptr<StringAttribute> m_nodeName;
	public:
		Node(Attribute* parent, std::string name, std::string nodeName) : CompoundAttribute(parent, name, "node"), m_nodeName(std::make_unique<StringAttribute>(nodeName)) {}
		
		virtual ~Node() override {};
		virtual Node* asNode() const override { return const_cast<Node*>(this); }
		virtual bool isNode() const override { return true; }
		virtual bool readValueXML(int format, XMLElement* element, LoaderXML* loader) override;
		virtual bool writeValueXML(int format, SaverXML* saver) override;
		virtual bool writeStartElementXML(int format, SaverXML* saver) override;
		virtual std::string getEscapedName() const override;
		virtual std::string getName() const override { return m_nodeName->getValue(); }
		virtual void setName(const std::string& name) override { m_nodeName->setValue(name); }

		bool reparent(db::Node* newParent);

		Attribute* addAttribute(Attribute* attribute) { insertAttribute(getAttributeCount(), attribute); return m_attributes.back(); }

		BoolAttribute* addBoolAttribute(const std::string& name, bool value);
		FloatAttribute* addFloatAttribute(const std::string& name, float value);
		DoubleAttribute* addDoubleAttribute(const std::string& name, double value);
		IntAttribute* addIntAttribute(const std::string& name, int value);
		StringAttribute* addStringAttribute(const std::string& name, const std::string& value);
		EnumAttribute* addEnumAttribute(const std::string& name, std::string value);
		Matrix34Attribute* addMatrix34Attribute(const std::string& name, const NMP::Matrix34& value);
		QuaternionAttribute* addQuaternionAttribute(const std::string& name, const NMP::Quat& value);
		Vector3Attribute* addVector3Attribute(const std::string& name, const NMP::Vector3& value);
		PointerAttribute* addPointerAttribute(const std::string& name, Attribute* to);

		BoolArrayAttribute* addBoolArrayAttribute(const std::string& name, const std::vector<bool>& values);
		FloatArrayAttribute* addFloatArrayAttribute(const std::string& name, const std::vector<float>& values);
		DoubleArrayAttribute* addDoubleArrayAttribute(const std::string& name, const std::vector<double>& values);
		IntArrayAttribute* addIntArrayAttribute(const std::string& name, const std::vector<int>& values);
		StringArrayAttribute* addStringArrayAttribute(const std::string& name, const std::vector<std::string>& values);
		EnumArrayAttribute* addEnumArrayAttribute(const std::string& name, const std::vector<std::string>& values);
		Matrix34ArrayAttribute* addMatrix34ArrayAttribute(const std::string& name, const std::vector<NMP::Matrix34>& values);
		QuaternionArrayAttribute* addQuaternionArrayAttribute(const std::string& name, const std::vector<NMP::Quat>& values);
		Vector3ArrayAttribute* addVector3ArrayAttribute(const std::string& name, const std::vector<NMP::Vector3>& values);

		Node* nodeFromDatabasePath(const std::string& path, bool fromLast) const;
	};
}