#pragma once
#include "RCore.h"
#include "Attribute/CompoundAttribute.h"
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
		std::string m_nodeName;
	public:
		Node(Attribute* parent, std::string name, std::string nodeName) : CompoundAttribute(parent, name, "node"), m_nodeName(nodeName) {}
		
		virtual ~Node() override {};
		virtual Node* asNode() const override { return const_cast<Node*>(this); }
		virtual bool isNode() const override { return true; }
		virtual bool writeValueXML(int format) override;
		virtual bool writeStartElementXML(int format) override;

		std::string getName() const { return m_nodeName; }
		void setName(const std::string& name) { m_nodeName = name; }

		void addAttribute(Attribute* attribute) { insertAttribute(getAttributeCount(), attribute); }

		void addBoolAttribute(const std::string& name, bool value) { insertAttribute(getAttributeCount(), new BoolAttribute(this, name, value)); }
		void addFloatAttribute(const std::string& name, float value) { insertAttribute(getAttributeCount(), new FloatAttribute(this, name, value)); }
		void addDoubleAttribute(const std::string& name, double value) { insertAttribute(getAttributeCount(), new DoubleAttribute(this, name, value)); }
		void addIntAttribute(const std::string& name, int value) { insertAttribute(getAttributeCount(), new IntAttribute(this, name, value)); }
		void addStringAttribute(const std::string& name, const std::string& value) { insertAttribute(getAttributeCount(), new StringAttribute(this, name, value)); }
		void addEnumAttribute(const std::string& name, std::string value) { insertAttribute(getAttributeCount(), new EnumAttribute(this, name, value)); }
		void addMatrix34Attribute(const std::string& name, const NMP::Matrix34& value) { insertAttribute(getAttributeCount(), new Matrix34Attribute(this, name, value)); }
		void addQuaternionAttribute(const std::string& name, const NMP::Quat& value) { insertAttribute(getAttributeCount(), new QuaternionAttribute(this, name, value)); }
		void addVector3Attribute(const std::string& name, const NMP::Vector3& value) { insertAttribute(getAttributeCount(), new Vector3Attribute(this, name, value)); }
		void addPointerAttribute(const std::string& name, Attribute* to) { insertAttribute(getAttributeCount(), new PointerAttribute(this, name, to)); }

		void addBoolArrayAttribute(const std::string& name, const std::vector<bool>& values);
		void addFloatArrayAttribute(const std::string& name, const std::vector<float>& values);
		void addDoubleArrayAttribute(const std::string& name, const std::vector<double>& values);
		void addIntArrayAttribute(const std::string& name, const std::vector<int>& values);
		void addStringArrayAttribute(const std::string& name, const std::vector<std::string>& values);
		void addEnumArrayAttribute(const std::string& name, const std::vector<std::string>& values);
		void addMatrix34ArrayAttribute(const std::string& name, const std::vector<NMP::Matrix34>& values);
		void addQuaternionArrayAttribute(const std::string& name, const std::vector<NMP::Quat>& values);
		void addVector3ArrayAttribute(const std::string& name, const std::vector<NMP::Vector3>& values);
	};
}