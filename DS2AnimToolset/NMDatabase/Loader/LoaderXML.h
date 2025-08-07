#pragma once
#include "Loader.h"
#include "Attribute/Attribute.h"

namespace db
{
	class LoaderXML : public Loader
	{
		tinyxml2::XMLDocument* m_xmlDocument;

		bool startElementHandler(tinyxml2::XMLElement* rootElement);
		bool parse(db::Attribute* attribute);
		bool onParseSuccess(db::Attribute* attribute, tinyxml2::XMLElement* element);
		bool onParseFailed();
	public:
		LoaderXML(FILE* inputStream);
		virtual ~LoaderXML() override;

		bool load(db::Attribute* attribute);
	};
}
