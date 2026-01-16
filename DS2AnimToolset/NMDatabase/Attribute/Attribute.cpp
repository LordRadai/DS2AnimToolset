#include "Attribute.h"
#include "Node/Node.h"
#include "Attribute/Array/AttributeArray.h"
#include "Attribute/CompositeAttribute/CompositeAttribute.h"
#include "CompoundAttribute/CompoundAttribute.h"
#include "Database/Database.h"

namespace db
{
    std::string Attribute::getEscapedName()
    {
        static std::string escapedName;

        if (m_parent == nullptr || !m_parent->isAttributeArray())
        {
            for (char ch : m_name)
            {
                if (ch == '.' || ch - '[' < 3)
                    escapedName += '\\';

                escapedName += ch;
			}
        }
        else
        {
			int index = getIndex();

			escapedName = "[" + std::to_string(index) + "]";
        }

        return escapedName;
	}

	Database* Attribute::getDatabase() const
	{
		Attribute* parent = m_parent;

		while (parent)
		{
			if (parent == nullptr) return dynamic_cast<Database*>(parent);

			parent = parent->getParentAttribute();
		}
	}

    int Attribute::getIndex()
    {
        if (m_parent && m_index != -1)
        {
			CompositeAttribute* parentComp = m_parent->asComposite();

			m_index = parentComp->getAttributeIndex(const_cast<Attribute*>(this));
        }

        return m_index;
	}

	bool Attribute::writeStartElementXML(int format, SaverXML* saver)
	{
		if (!m_parent)
			return false;

		m_xmlElement = m_parent->getXMLElement()->InsertNewChildElement(m_name.c_str());
		m_xmlElement->SetAttribute("type", m_type.c_str());

		return true;
	}

	bool Attribute::writeXML(int format, SaverXML* saver)
	{
		bool bWroteStartElem = writeStartElementXML(format, saver);

		if (!bWroteStartElem)
			return false;

		return writeValueXML(format, saver);
	}

	std::string Attribute::toDatabasePath() const
	{
		if (m_parent == nullptr)
			return m_name;

		return m_parent->toDatabasePath() + "." + m_name;
	}

    Attribute* Attribute::attributeFromDatabasePath(const std::string& path, bool resolveLast) const 
    {
        if (path.empty())
            return nullptr;

        // Tokenize the path
        std::vector<std::string> tokens;
        std::string current;
        bool escape = false;

        for (wchar_t ch : path) 
        {
            if (escape) 
            {
                current += ch;
                escape = false;
            }
            else if (ch == L'\\') 
            {
                escape = true;
            }
            else if (ch == L'.' || ch == L'[' || ch == L']') 
            {
                if (!current.empty()) 
                {
                    tokens.push_back(current);
                    current.clear();
                }
            }
            else 
            {
                current += ch;
            }
        }

        if (!current.empty())
            tokens.push_back(current);

        if (tokens.empty())
            return nullptr;

        Attribute* currentAttr = const_cast<Attribute*>(this);
        size_t end = resolveLast ? tokens.size() : tokens.size() - 1;

        for (size_t i = 0; i < end; ++i) 
        {
            const std::string& token = tokens[i];

            if (token.empty())
                return nullptr;

            if (std::all_of(token.begin(), token.end(), iswdigit)) 
            {
                // Token is a numeric index
                int index = std::stoi(token);
                if (!currentAttr->isAttributeArray()) return nullptr;

                auto* attributeArray = currentAttr->asAttributeArray();
                if (!attributeArray || index < 0 || index >= attributeArray->size()) return nullptr;

                currentAttr = attributeArray->operator[](index);
            }
            else 
            {
                // Token is an attribute name
                if (!currentAttr->isComposite()) return nullptr;

                auto* composite = currentAttr->asComposite();
                if (!composite) return nullptr;

                currentAttr = composite->findAttribute(token);
                if (!currentAttr) return nullptr;
            }
        }

        return currentAttr;
    }
}
