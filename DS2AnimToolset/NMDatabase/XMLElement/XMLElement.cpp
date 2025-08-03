#include "XMLElement.h"

namespace db
{
	XMLElement::XMLElement(const std::string& name, db::Attribute* owner, XMLElement* parent)
		: m_dbAttribute(owner), m_parent(parent)
	{
		m_xmlElement = parent->m_xmlElement->InsertNewChildElement(name.c_str());

		m_children.reserve(m_xmlElement->ChildElementCount());

		for (tinyxml2::XMLElement* child = m_xmlElement->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) 
			m_children.push_back(new XMLElement(child->Name(), owner, this));
	}

	XMLElement* XMLElement::getChild(const int index) const
	{
		if (index < 0 || index >= static_cast<int>(m_children.size()))
			return nullptr;

		return m_children[index];
	}

	XMLElement* XMLElement::findChild(const std::string& name) const
	{
		for (XMLElement* child : m_children)
		{
			if (child->m_xmlElement->Name() == name)
				return child;
		}

		return nullptr;
	}

	bool XMLElement::getDataAsBool(bool& out) const
	{
		const char* value = m_xmlElement->GetText();

		if (value)
		{
			out = (strcmp(value, "true") == 0 || strcmp(value, "1") == 0);
			return true;
		}

		return false;
	}

	bool XMLElement::getDataAsInt(int& out) const
	{
		const char* value = m_xmlElement->GetText();
		if (value)
		{
			out = m_xmlElement->IntAttribute(value);
			return true;
		}

		return false;
	}

	bool XMLElement::getDataAsFloat(float& out) const
	{
		const char* value = m_xmlElement->GetText();
		if (value)
		{
			out = m_xmlElement->FloatAttribute(value);
			return true;
		}

		return false;
	}

	bool XMLElement::getDataAsDouble(double& out) const
	{
		const char* value = m_xmlElement->GetText();
		if (value)
		{
			out = m_xmlElement->DoubleAttribute(value);
			return true;
		}

		return false;
	}

	bool XMLElement::getDataAsString(std::string& out) const
	{
		const char* value = m_xmlElement->GetText();
		if (value)
		{
			out = value;
			return true;
		}

		return false;
	}

	bool XMLElement::getDataAsVector3(NMP::Vector3& out) const
	{
		XMLElement* x = findChild("X");
		XMLElement* y = findChild("Y");
		XMLElement* z = findChild("Z");

		if (!x->getDataAsFloat(out.x))
			return false;

		if (!y->getDataAsFloat(out.y))
			return false;

		if (!z->getDataAsFloat(out.z))
			return false;

		return true;
	}

	bool XMLElement::getDataAsQuat(NMP::Quat& out) const
	{
		XMLElement* x = findChild("X");
		XMLElement* y = findChild("Y");
		XMLElement* z = findChild("Z");
		XMLElement* w = findChild("W");

		if (!x->getDataAsFloat(out.x))
			return false;

		if (!y->getDataAsFloat(out.y))
			return false;

		if (!z->getDataAsFloat(out.z))
			return false;

		if (!w->getDataAsFloat(out.w))
			return false;

		return true;
	}

	bool XMLElement::getDataAsMatrix34(NMP::Matrix34& out) const
	{
		for (uint32_t row = 0; row < 4; row++)
		{
			char rName[256];
			sprintf(rName, "R%d", row);
			XMLElement* rowElement = findChild(rName);

			if (!rowElement)
				throw std::runtime_error("Matrix34Attribute::readValueXML: Column format is invalid. There are less than 4 columns.");

			for (uint32_t col = 0; col < 3; col++)
			{
				char cName[256];
				sprintf(cName, "C%d", col);
				XMLElement* colElement = findChild(cName);

				if (!colElement)
					throw std::runtime_error("Matrix34Attribute::readValueXML: Column format is invalid. There are less than 3 rows in one column.");

				if (!colElement->getDataAsFloat(out.r[row][col]))
					return false;
			}
		}

		return true;
	}

	bool XMLElement::getDataAsBoolArray(std::vector<bool>& out) const
	{
		out.clear();
		out.reserve(m_children.size());

		for (XMLElement* child : m_children)
		{
			bool value;
			if (child->getDataAsBool(value))
				out.push_back(value);
			else
				return false;
		}
		return true;
	}

	bool XMLElement::getDataAsIntArray(std::vector<int>& out) const
	{
		out.clear();
		out.reserve(m_children.size());
		for (XMLElement* child : m_children)
		{
			int value;
			if (child->getDataAsInt(value))
				out.push_back(value);
			else
				return false;
		}
		return true;
	}

	bool XMLElement::getDataAsFloatArray(std::vector<float>& out) const
	{
		out.clear();
		out.reserve(m_children.size());
		for (XMLElement* child : m_children)
		{
			float value;
			if (child->getDataAsFloat(value))
				out.push_back(value);
			else
				return false;
		}
		return true;
	}

	bool XMLElement::getDataAsDoubleArray(std::vector<double>& out) const
	{
		out.clear();
		out.reserve(m_children.size());
		for (XMLElement* child : m_children)
		{
			double value;
			if (child->getDataAsDouble(value))
				out.push_back(value);
			else
				return false;
		}
		return true;
	}

	bool XMLElement::getDataAsStringArray(std::vector<std::string>& out) const
	{
		out.clear();
		out.reserve(m_children.size());
		for (XMLElement* child : m_children)
		{
			std::string value;
			if (child->getDataAsString(value))
				out.push_back(value);
			else
				return false;
		}
		return true;
	}

	bool XMLElement::getDataAsVector3Array(std::vector<NMP::Vector3>& out) const
	{
		out.clear();
		out.reserve(m_children.size());
		for (XMLElement* child : m_children)
		{
			NMP::Vector3 value;
			if (child->getDataAsVector3(value))
				out.push_back(value);
			else
				return false;
		}
		return true;
	}

	bool XMLElement::getDataAsQuatArray(std::vector<NMP::Quat>& out) const
	{
		out.clear();
		out.reserve(m_children.size());
		for (XMLElement* child : m_children)
		{
			NMP::Quat value;
			if (child->getDataAsQuat(value))
				out.push_back(value);
			else
				return false;
		}
		return true;
	}

	bool XMLElement::getDataAsMatrix34Array(std::vector<NMP::Matrix34>& out) const
	{
		out.clear();
		out.reserve(m_children.size());
		for (XMLElement* child : m_children)
		{
			NMP::Matrix34 value;
			if (child->getDataAsMatrix34(value))
				out.push_back(value);
			else
				return false;
		}
		return true;
	}

	bool XMLElement::getAttributeAsBool(const std::string& name, bool& out) const
	{
		const char* value = m_xmlElement->Attribute(name.c_str());
		if (value)
		{
			out = (strcmp(value, "true") == 0 || strcmp(value, "1") == 0);
			return true;
		}
		return false;
	}

	bool XMLElement::getAttributeAsInt(const std::string& name, int& out) const
	{
		const char* value = m_xmlElement->Attribute(name.c_str());
		if (value)
		{
			out = m_xmlElement->IntAttribute(name.c_str());
			return true;
		}
		return false;
	}

	bool XMLElement::getAttributeAsFloat(const std::string& name, float& out) const
	{
		const char* value = m_xmlElement->Attribute(name.c_str());
		if (value)
		{
			out = m_xmlElement->FloatAttribute(name.c_str());
			return true;
		}
		return false;
	}

	bool XMLElement::getAttributeAsDouble(const std::string& name, double& out) const
	{
		const char* value = m_xmlElement->Attribute(name.c_str());
		if (value)
		{
			out = m_xmlElement->DoubleAttribute(name.c_str());
			return true;
		}
		return false;
	}

	bool XMLElement::getAttributeAsString(const std::string& name, std::string& out) const
	{
		const char* value = m_xmlElement->Attribute(name.c_str());
		if (value)
		{
			out = value;
			return true;
		}
		return false;
	}

	uint32_t XMLElement::getAttributeCount() const
	{
		uint32_t count = 0;
		for (const tinyxml2::XMLAttribute* attr = m_xmlElement->FirstAttribute(); attr != nullptr; attr = attr->Next())
			count++;

		return count;
	}
}