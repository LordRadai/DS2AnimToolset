#include "ProjectControlParameter.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMVector3.h"

namespace NodeEditor
{
	namespace Project
	{
		bool ProjectControlParameter::loadFromXMLElement(tinyxml2::XMLElement* xmlElement)
		{
			if (xmlElement)
			{
				m_xmlElement = xmlElement;
				return true;
			}

			return false;
		}

		const std::string ProjectControlParameter::getType() const
		{
			if (m_xmlElement)
				return m_xmlElement->Attribute("dataType");

			return "";
		}

		void ProjectControlParameter::setType(const std::string& type)
		{
			if (m_xmlElement)
				m_xmlElement->SetAttribute("dataType", type.c_str());
		}

		int ProjectControlParameter::getControlParameterID() const
		{
			if (m_xmlElement)
				return m_xmlElement->IntAttribute("nodeID", 0);

			return 0;
		}

		void ProjectControlParameter::setControlParameterID(int id)
		{
			if (m_xmlElement)
				m_xmlElement->SetAttribute("nodeID", id);
		}

		template<>
		int ProjectControlParameter::getDefaultValue<int>() const
		{
			tinyxml2::XMLElement* xmlElem = m_xmlElement->FirstChildElement("DefaultValue");

			if (xmlElem)
				return std::stoi(xmlElem->GetText());

			return 0;
		}

		template<>
		void ProjectControlParameter::setDefaultValue<int>(const int& value)
		{
			tinyxml2::XMLElement* xmlElem = m_xmlElement->FirstChildElement("DefaultValue");
			if (!xmlElem)
			{
				xmlElem = m_xmlElement->GetDocument()->NewElement("DefaultValue");
				m_xmlElement->InsertEndChild(xmlElem);
			}
			xmlElem->SetText(std::to_string(value).c_str());
		}

		template<>
		float ProjectControlParameter::getDefaultValue<float>() const
		{
			tinyxml2::XMLElement* xmlElem = m_xmlElement->FirstChildElement("DefaultValue");

			if (xmlElem)
				return std::stof(xmlElem->GetText());

			return 0.0f;
		}

		template<>
		void ProjectControlParameter::setDefaultValue<float>(const float& value)
		{
			tinyxml2::XMLElement* xmlElem = m_xmlElement->FirstChildElement("DefaultValue");
			if (!xmlElem)
			{
				xmlElem = m_xmlElement->GetDocument()->NewElement("DefaultValue");
				m_xmlElement->InsertEndChild(xmlElem);
			}
			xmlElem->SetText(std::to_string(value).c_str());
		}

		template<>
		NMP::Vector3 ProjectControlParameter::getDefaultValue<NMP::Vector3>() const
		{
			tinyxml2::XMLElement* xmlElem = m_xmlElement->FirstChildElement("DefaultValue");

			if (xmlElem)
			{
				NMP::Vector3 vec;

				xmlElem->FirstChildElement("X")->QueryFloatText(&vec.x);
				xmlElem->FirstChildElement("Y")->QueryFloatText(&vec.y);
				xmlElem->FirstChildElement("Z")->QueryFloatText(&vec.z);

				return vec;
			}

			return NMP::Vector3Zero();
		}

		template<>
		void ProjectControlParameter::setDefaultValue<NMP::Vector3>(const NMP::Vector3& value)
		{
			tinyxml2::XMLElement* xmlElem = m_xmlElement->FirstChildElement("DefaultValue");

			if (!xmlElem)
			{
				xmlElem = m_xmlElement->GetDocument()->NewElement("DefaultValue");
				m_xmlElement->InsertEndChild(xmlElem);
			}

			tinyxml2::XMLElement* xElem = xmlElem->FirstChildElement("X");

			if (!xElem)
			{
				xElem = m_xmlElement->GetDocument()->NewElement("X");
				xmlElem->InsertEndChild(xElem);
			}

			xElem->SetText(std::to_string(value.x).c_str());
			tinyxml2::XMLElement* yElem = xmlElem->FirstChildElement("Y");

			if (!yElem)
			{
				yElem = m_xmlElement->GetDocument()->NewElement("Y");
				xmlElem->InsertEndChild(yElem);
			}

			yElem->SetText(std::to_string(value.y).c_str());
			tinyxml2::XMLElement* zElem = xmlElem->FirstChildElement("Z");

			if (!zElem)
			{
				zElem = m_xmlElement->GetDocument()->NewElement("Z");
				xmlElem->InsertEndChild(zElem);
			}

			zElem->SetText(std::to_string(value.z).c_str());
		}

		template<>
		NMP::Quat ProjectControlParameter::getDefaultValue<NMP::Quat>() const
		{
			tinyxml2::XMLElement* xmlElem = m_xmlElement->FirstChildElement("DefaultValue");
			if (xmlElem)
			{
				NMP::Quat quat;
				xmlElem->FirstChildElement("W")->QueryFloatText(&quat.w);
				xmlElem->FirstChildElement("X")->QueryFloatText(&quat.x);
				xmlElem->FirstChildElement("Y")->QueryFloatText(&quat.y);
				xmlElem->FirstChildElement("Z")->QueryFloatText(&quat.z);

				return quat;
			}

			return NMP::QuatIdentity();
		}

		template<>
		void ProjectControlParameter::setDefaultValue<NMP::Quat>(const NMP::Quat& value)
		{
			tinyxml2::XMLElement* xmlElem = m_xmlElement->FirstChildElement("DefaultValue");
			if (!xmlElem)
			{
				xmlElem = m_xmlElement->GetDocument()->NewElement("DefaultValue");
				m_xmlElement->InsertEndChild(xmlElem);
			}
			tinyxml2::XMLElement* wElem = xmlElem->FirstChildElement("W");
			if (!wElem)
			{
				wElem = m_xmlElement->GetDocument()->NewElement("W");
				xmlElem->InsertEndChild(wElem);
			}
			wElem->SetText(std::to_string(value.w).c_str());
			tinyxml2::XMLElement* xElem = xmlElem->FirstChildElement("X");
			if (!xElem)
			{
				xElem = m_xmlElement->GetDocument()->NewElement("X");
				xmlElem->InsertEndChild(xElem);
			}
			xElem->SetText(std::to_string(value.x).c_str());
			tinyxml2::XMLElement* yElem = xmlElem->FirstChildElement("Y");
			if (!yElem)
			{
				yElem = m_xmlElement->GetDocument()->NewElement("Y");
				xmlElem->InsertEndChild(yElem);
			}
			yElem->SetText(std::to_string(value.y).c_str());
			tinyxml2::XMLElement* zElem = xmlElem->FirstChildElement("Z");
			if (!zElem)
			{
				zElem = m_xmlElement->GetDocument()->NewElement("Z");
				xmlElem->InsertEndChild(zElem);
			}
			zElem->SetText(std::to_string(value.z).c_str());
		}

		template<>
		bool ProjectControlParameter::getDefaultValue<bool>() const
		{
			tinyxml2::XMLElement* xmlElem = m_xmlElement->FirstChildElement("DefaultValue");
			if (xmlElem)
			{
				std::string text = xmlElem->GetText();
				return (text == "true" || text == "1");
			}
			return false;
		}

		template<>
		void ProjectControlParameter::setDefaultValue<bool>(const bool& value)
		{
			tinyxml2::XMLElement* xmlElem = m_xmlElement->FirstChildElement("DefaultValue");
			if (!xmlElem)
			{
				xmlElem = m_xmlElement->GetDocument()->NewElement("DefaultValue");
				m_xmlElement->InsertEndChild(xmlElem);
			}
			xmlElem->SetText(value ? "true" : "false");
		}
	}
}
