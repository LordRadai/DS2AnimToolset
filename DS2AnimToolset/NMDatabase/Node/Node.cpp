#include "Node.h"
#include "Attribute/CompositeAttribute/CompositeAttribute.h"
#include "Registry/Registry.h"

namespace db
{
	Node::Node(CompositeAttribute* parent, std::string name, std::string nodeName) : CompoundAttribute(parent, name, "node"),
		m_nodeName(std::make_unique<StringAttribute>(nodeName))
	{
		m_nodeID = Registry::getInstance()->getNewNodeID();
	}

	std::string Node::getEscapedName()
	{
		static std::string escapedName;
		std::string name = m_nodeName->getValue();

		for (char ch : name)
		{
			if (ch == '.' || ch - '[' < 3)
				escapedName += '\\';

			escapedName += ch;
		}

		return escapedName;
	}

	bool Node::readValueXML(int format, XMLElement* element, LoaderXML* loader)
	{
		for (size_t i = 0; i < element->getNumChildren(); i++)
		{
			XMLElement* child = element->getChild(i);
			Attribute* attr = this->findAttribute(child->getName());

			if (attr == nullptr)
			{
				std::string type = child->getAttribute("type");

				if (type == "int") attr = addIntAttribute(child->getName(), 0);
				else if (type == "bool") attr = addBoolAttribute(child->getName(), false);
				else if (type == "float") attr = addFloatAttribute(child->getName(), 0.0f);
				else if (type == "double") attr = addDoubleAttribute(child->getName(), 0.0);
				else if (type == "string") attr = addStringAttribute(child->getName(), "");
				else if (type == "enum") attr = addEnumAttribute(child->getName(), "");
				else if (type == "matrix34") attr = addMatrix34Attribute(child->getName(), NMP::Matrix34Identity());
				else if (type == "quaternion") attr = addQuaternionAttribute(child->getName(), NMP::QuatIdentity());
				else if (type == "vector3") attr = addVector3Attribute(child->getName(), NMP::Vector3Zero());
				else if (type == "boolArray") attr = addBoolArrayAttribute(child->getName(), {});
				else if (type == "floatArray") attr = addFloatArrayAttribute(child->getName(), {});
				else if (type == "doubleArray") attr = addDoubleArrayAttribute(child->getName(), {});
				else if (type == "intArray") attr = addIntArrayAttribute(child->getName(), {});
				else if (type == "stringArray") attr = addStringArrayAttribute(child->getName(), {});
				else if (type == "enumArray") attr = addEnumArrayAttribute(child->getName(), {});
				else if (type == "matrix34Array") attr = addMatrix34ArrayAttribute(child->getName(), {});
				else if (type == "quaternionArray") attr = addQuaternionArrayAttribute(child->getName(), {});
				else if (type == "vector3Array") attr = addVector3ArrayAttribute(child->getName(), {});
				else throw std::runtime_error("Node::readValueXML: Unknown attribute type '" + type + "' in XML element '" + child->getName() + "'.");
			}

			if (attr == nullptr)
				throw std::runtime_error("Node::readValueXML: Failed to find or create attribute for XML element '" + child->getName() + "' in Node '." + this->getName() + "'.");

			if (!attr->readValueXML(format, child, loader))
				return false;
		}

		return true;
	}

	bool Node::writeValueXML(int format, SaverXML* saver)
	{
		for (size_t i = 0; i < m_attributes.size(); i++)
		{
			if (!m_attributes[i]->writeXML(format, saver))
				return false;
		}

		return true;
	}

	bool Node::writeStartElementXML(int format, SaverXML* saver)
	{
		if (!m_parent)
			return false;

		m_xmlElement = m_parent->getXMLElement()->InsertNewChildElement(m_name.c_str());

		std::string nodeName = m_nodeName->getValue();

		if (nodeName != "")
			m_xmlElement->SetAttribute("name", nodeName.c_str());

		m_xmlElement->SetAttribute("type", "node");

		return true;
	}

	bool Node::reparent(db::Node* newParent)
	{
		setParentAttribute(newParent);

		return true;
	}

	BoolAttribute* Node::addBoolAttribute(const std::string& name, bool value)
	{
		BoolAttribute* attr = new BoolAttribute(this, name, value);
		insertAttribute(getAttributeCount(), attr);

		return attr;
	}

	FloatAttribute* Node::addFloatAttribute(const std::string& name, float value)
	{
		FloatAttribute* attr = new FloatAttribute(this, name, value);
		insertAttribute(getAttributeCount(), attr);
		return attr;
	}

	DoubleAttribute* Node::addDoubleAttribute(const std::string& name, double value)
	{
		DoubleAttribute* attr = new DoubleAttribute(this, name, value);
		insertAttribute(getAttributeCount(), attr);
		return attr;
	}

	IntAttribute* Node::addIntAttribute(const std::string& name, int value)
	{
		IntAttribute* attr = new IntAttribute(this, name, value);
		insertAttribute(getAttributeCount(), attr);
		return attr;
	}

	StringAttribute* Node::addStringAttribute(const std::string& name, const std::string& value)
	{
		StringAttribute* attr = new StringAttribute(this, name, value);
		insertAttribute(getAttributeCount(), attr);
		return attr;
	}

	EnumAttribute* Node::addEnumAttribute(const std::string& name, std::string value)
	{
		EnumAttribute* attr = new EnumAttribute(this, name, value);
		insertAttribute(getAttributeCount(), attr);
		return attr;
	}

	Matrix34Attribute* Node::addMatrix34Attribute(const std::string& name, const NMP::Matrix34& value)
	{
		Matrix34Attribute* attr = new Matrix34Attribute(this, name, value);
		insertAttribute(getAttributeCount(), attr);
		return attr;
	}

	QuaternionAttribute* Node::addQuaternionAttribute(const std::string& name, const NMP::Quat& value)
	{
		QuaternionAttribute* attr = new QuaternionAttribute(this, name, value);
		insertAttribute(getAttributeCount(), attr);
		return attr;
	}

	Vector3Attribute* Node::addVector3Attribute(const std::string& name, const NMP::Vector3& value)
	{
		Vector3Attribute* attr = new Vector3Attribute(this, name, value);
		insertAttribute(getAttributeCount(), attr);
		return attr;
	}

	PointerAttribute* Node::addPointerAttribute(const std::string& name, Attribute* to)
	{
		PointerAttribute* attr = new PointerAttribute(this, name, to);
		insertAttribute(getAttributeCount(), attr);
		return attr;
	}

	BoolArrayAttribute* Node::addBoolArrayAttribute(const std::string& name, const std::vector<bool>& values)
	{
		BoolArrayAttribute* attr = new BoolArrayAttribute(this, name);
		insertAttribute(getAttributeCount(), attr);
		
		attr->resize(static_cast<uint32_t>(values.size()));
		for (size_t i = 0; i < values.size(); i++)
			attr->add(values[i]);

		return attr;
	}

	FloatArrayAttribute* Node::addFloatArrayAttribute(const std::string& name, const std::vector<float>& values)
	{
		FloatArrayAttribute* attr = new FloatArrayAttribute(this, name);
		insertAttribute(getAttributeCount(), attr);
		
		attr->resize(static_cast<uint32_t>(values.size()));
		for (size_t i = 0; i < values.size(); i++)
			attr->add(values[i]);
		
		return attr;
	}

	DoubleArrayAttribute* Node::addDoubleArrayAttribute(const std::string& name, const std::vector<double>& values)
	{
		DoubleArrayAttribute* attr = new DoubleArrayAttribute(this, name);
		insertAttribute(getAttributeCount(), attr);
		
		attr->resize(static_cast<uint32_t>(values.size()));
		for (size_t i = 0; i < values.size(); i++)
			attr->add(values[i]);
		
		return attr;
	}

	IntArrayAttribute* Node::addIntArrayAttribute(const std::string& name, const std::vector<int>& values)
	{
		IntArrayAttribute* attr = new IntArrayAttribute(this, name);
		insertAttribute(getAttributeCount(), attr);
		
		attr->resize(static_cast<uint32_t>(values.size()));
		for (size_t i = 0; i < values.size(); i++)
			attr->add(values[i]);
		
		return attr;
	}

	StringArrayAttribute* Node::addStringArrayAttribute(const std::string& name, const std::vector<std::string>& values)
	{
		StringArrayAttribute* attr = new StringArrayAttribute(this, name);
		insertAttribute(getAttributeCount(), attr);
		
		attr->resize(static_cast<uint32_t>(values.size()));
		for (size_t i = 0; i < values.size(); i++)
			attr->add(values[i]);
		
		return attr;
	}

	EnumArrayAttribute* Node::addEnumArrayAttribute(const std::string& name, const std::vector<std::string>& values)
	{
		EnumArrayAttribute* attr = new EnumArrayAttribute(this, name);
		insertAttribute(getAttributeCount(), attr);
		
		attr->resize(static_cast<uint32_t>(values.size()));
		for (size_t i = 0; i < values.size(); i++)
			attr->add(values[i]);
		
		return attr;
	}

	Matrix34ArrayAttribute* Node::addMatrix34ArrayAttribute(const std::string& name, const std::vector<NMP::Matrix34>& values)
	{
		Matrix34ArrayAttribute* attr = new Matrix34ArrayAttribute(this, name);
		insertAttribute(getAttributeCount(), attr);
		
		attr->resize(static_cast<uint32_t>(values.size()));
		for (size_t i = 0; i < values.size(); i++)
			attr->add(values[i]);
		
		return attr;
	}

	QuaternionArrayAttribute* Node::addQuaternionArrayAttribute(const std::string& name, const std::vector<NMP::Quat>& values)
	{
		QuaternionArrayAttribute* attr = new QuaternionArrayAttribute(this, name);
		insertAttribute(getAttributeCount(), attr);
		
		attr->resize(static_cast<uint32_t>(values.size()));
		for (size_t i = 0; i < values.size(); i++)
			attr->add(values[i]);
		
		return attr;
	}

	Vector3ArrayAttribute* Node::addVector3ArrayAttribute(const std::string& name, const std::vector<NMP::Vector3>& values)
	{
		Vector3ArrayAttribute* attr = new Vector3ArrayAttribute(this, name);
		insertAttribute(getAttributeCount(), attr);
		
		attr->resize(static_cast<uint32_t>(values.size()));
		for (size_t i = 0; i < values.size(); i++)
			attr->add(values[i]);
		
		return attr;
	}

	Node* Node::nodeFromDatabasePath(const std::string& path, bool fromLast) const
	{
		Attribute* attr = attributeFromDatabasePath(path, fromLast);

		if (attr == nullptr)
			return nullptr;

		return attr->asNode();
	}

	bool Node::isNameUnique(const std::string& name)
	{
		if (!m_parent)
			return true;

		for (size_t i = 0; i < m_parent->getAttributeCount(); i++)
		{
			Attribute* sibling = m_parent->getAttribute(i);

			if (sibling == this)
				continue;

			if (sibling->getName() == name)
				return false;
		}
	}

	void Node::makeNameValid(std::string& name)
	{
		if (isNameUnique(name))
			return;

		while (!isNameUnique(name))
		{
			char suffix[32];
			sprintf_s(suffix, 32, "%d", m_nodeID);
			name.append(suffix);
		}
	}
}