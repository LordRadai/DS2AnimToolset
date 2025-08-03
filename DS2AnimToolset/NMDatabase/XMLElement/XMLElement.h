#pragma once
#include "RCore.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMMatrix34.h"

namespace db
{
	class Attribute;

	class XMLElement
	{
		tinyxml2::XMLElement* m_xmlElement = nullptr;
		XMLElement* m_parent = nullptr;
		db::Attribute* m_dbAttribute;
		std::vector<XMLElement*> m_children;

	public:
		XMLElement(const std::string& name, db::Attribute* owner, XMLElement* parent);

		tinyxml2::XMLElement* getXmlElement() const { return m_xmlElement; }
		XMLElement* getParent() const { return m_parent; }
		db::Attribute* getDBAttribute() const { return m_dbAttribute; }

		std::string getName() const { return m_xmlElement->Name(); }
		const char* getAttribute(const std::string& name) const { return m_xmlElement->Attribute(name.c_str()); }

		XMLElement* getChild(const int index) const;
		XMLElement* findChild(const std::string& name) const;
		uint32_t getChildCount() const { return static_cast<uint32_t>(m_children.size()); }

		bool hasAttribute(const std::string& name) const { return m_xmlElement->Attribute(name.c_str()) != nullptr; }
		uint32_t getAttributeCount() const;

		bool getDataAsBool(bool& out) const;
		bool getDataAsInt(int& out) const;
		bool getDataAsFloat(float& out) const;
		bool getDataAsDouble(double& out) const;
		bool getDataAsString(std::string& out) const;
		bool getDataAsVector3(NMP::Vector3& out) const;
		bool getDataAsQuat(NMP::Quat& out) const;
		bool getDataAsMatrix34(NMP::Matrix34& out) const;

		bool getDataAsBoolArray(std::vector<bool>& out) const;
		bool getDataAsIntArray(std::vector<int>& out) const;
		bool getDataAsFloatArray(std::vector<float>& out) const;
		bool getDataAsDoubleArray(std::vector<double>& out) const;
		bool getDataAsStringArray(std::vector<std::string>& out) const;
		bool getDataAsVector3Array(std::vector<NMP::Vector3>& out) const;
		bool getDataAsQuatArray(std::vector<NMP::Quat>& out) const;
		bool getDataAsMatrix34Array(std::vector<NMP::Matrix34>& out) const;

		bool getAttributeAsBool(const std::string& name, bool& out) const;
		bool getAttributeAsInt(const std::string& name, int& out) const;
		bool getAttributeAsFloat(const std::string& name, float& out) const;
		bool getAttributeAsDouble(const std::string& name, double& out) const;
		bool getAttributeAsString(const std::string& name, std::string& out) const;
	};
}
