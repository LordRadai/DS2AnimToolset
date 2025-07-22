#include "PreviewScript.h"
#include "mcnSerializer/mcnSerializer.h"

namespace db
{
	tinyxml2::XMLElement* PreviewScript::serialize(tinyxml2::XMLElement* parent)
	{
		std::ifstream scriptFile(m_filepath);

		if (!scriptFile.is_open())
			throw std::runtime_error("Failed to open script file " + m_filepath);

		std::stringstream scriptContent;
		scriptContent << scriptFile.rdbuf();

		tinyxml2::XMLElement* element = Node::serialize(parent);

		tinyxml2::XMLElement* scriptData = mcnSerializer::createStringElement(element, "ScriptData", "");

		tinyxml2::XMLText* textNode = parent->GetDocument()->NewText(scriptContent.str().c_str());
		textNode->SetCData(true);

		scriptData->InsertFirstChild(textNode);
	}
}