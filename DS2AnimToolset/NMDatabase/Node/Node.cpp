#include "Node.h"

namespace db
{
	bool Node::writeValueXML(int format)
	{
		for (size_t i = 0; i < m_attributes.size(); i++)
		{
			if (!m_attributes[i]->writeXML(format))
				return false;
		}

		return true;
	}

	bool Node::writeStartElementXML(int format)
	{
		if (!m_parent)
			return false;

		m_xmlElement = m_parent->getXMLElement()->InsertNewChildElement(m_name.c_str());

		if (m_nodeName != "")
			m_xmlElement->SetAttribute("name", m_nodeName.c_str());

		m_xmlElement->SetAttribute("type", "node");

		return true;
	}

	void Node::addBoolArrayAttribute(const std::string& name, const std::vector<bool>& values)
	{
		insertAttribute(getAttributeCount(), new BoolArrayAttribute(this, name));
		BoolArrayAttribute* attr = dynamic_cast<BoolArrayAttribute*>(back());
		
		attr->resize(static_cast<uint32_t>(values.size()));
		for (size_t i = 0; i < values.size(); i++)
			attr->add(values[i]);
	}

	void Node::addFloatArrayAttribute(const std::string& name, const std::vector<float>& values)
	{
		insertAttribute(getAttributeCount(), new FloatArrayAttribute(this, name));
		FloatArrayAttribute* attr = dynamic_cast<FloatArrayAttribute*>(back());

		attr->resize(static_cast<uint32_t>(values.size()));
		for (size_t i = 0; i < values.size(); i++)
			attr->add(values[i]);
	}

	void Node::addDoubleArrayAttribute(const std::string& name, const std::vector<double>& values)
	{
		insertAttribute(getAttributeCount(), new DoubleArrayAttribute(this, name));
		DoubleArrayAttribute* attr = dynamic_cast<DoubleArrayAttribute*>(back());

		attr->resize(static_cast<uint32_t>(values.size()));
		for (size_t i = 0; i < values.size(); i++)
			attr->add(values[i]);
	}

	void Node::addIntArrayAttribute(const std::string& name, const std::vector<int>& values)
	{
		insertAttribute(getAttributeCount(), new IntArrayAttribute(this, name));
		IntArrayAttribute* attr = dynamic_cast<IntArrayAttribute*>(back());

		attr->resize(static_cast<uint32_t>(values.size()));
		for (size_t i = 0; i < values.size(); i++)
			attr->add(values[i]);
	}

	void Node::addStringArrayAttribute(const std::string& name, const std::vector<std::string>& values)
	{
		insertAttribute(getAttributeCount(), new StringArrayAttribute(this, name));
		StringArrayAttribute* attr = dynamic_cast<StringArrayAttribute*>(back());

		attr->resize(static_cast<uint32_t>(values.size()));
		for (size_t i = 0; i < values.size(); i++)
			attr->add(values[i]);
	}

	void Node::addEnumArrayAttribute(const std::string& name, const std::vector<std::string>& values)
	{
		insertAttribute(getAttributeCount(), new EnumArrayAttribute(this, name));
		EnumArrayAttribute* attr = dynamic_cast<EnumArrayAttribute*>(back());

		attr->resize(static_cast<uint32_t>(values.size()));
		for (size_t i = 0; i < values.size(); i++)
			attr->add(values[i]);
	}

	void Node::addMatrix34ArrayAttribute(const std::string& name, const std::vector<NMP::Matrix34>& values)
	{
		insertAttribute(getAttributeCount(), new Matrix34ArrayAttribute(this, name));
		Matrix34ArrayAttribute* attr = dynamic_cast<Matrix34ArrayAttribute*>(back());

		attr->resize(static_cast<uint32_t>(values.size()));
		for (size_t i = 0; i < values.size(); i++)
			attr->add(values[i]);
	}

	void Node::addQuaternionArrayAttribute(const std::string& name, const std::vector<NMP::Quat>& values)
	{
		insertAttribute(getAttributeCount(), new QuaternionArrayAttribute(this, name));
		QuaternionArrayAttribute* attr = dynamic_cast<QuaternionArrayAttribute*>(back());

		attr->resize(static_cast<uint32_t>(values.size()));
		for (size_t i = 0; i < values.size(); i++)
			attr->add(values[i]);
	}

	void Node::addVector3ArrayAttribute(const std::string& name, const std::vector<NMP::Vector3>& values)
	{
		insertAttribute(getAttributeCount(), new Vector3ArrayAttribute(this, name));
		Vector3ArrayAttribute* attr = dynamic_cast<Vector3ArrayAttribute*>(back());

		attr->resize(static_cast<uint32_t>(values.size()));
		for (size_t i = 0; i < values.size(); i++)
			attr->add(values[i]);
	}
}