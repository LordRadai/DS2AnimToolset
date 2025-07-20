#pragma once
#include "RCore.h"
#include "../Node/Node.h"

namespace db
{
	class AnimationLocation : public Node
	{
		std::string m_sourceDir;
		std::string m_markupDir;
		bool m_bIncludeSubDirs;

	public:
		AnimationLocation(Node* parent, const std::string& sourceDir, const std::string& markupDir, bool bIncludeSubDirs = false) :
			Node(parent, "AnimationLocation", "AnimationLocation"),
			m_sourceDir(sourceDir),
			m_markupDir(markupDir),
			m_bIncludeSubDirs(bIncludeSubDirs) {
		};

		virtual ~AnimationLocation() {};
		virtual bool isValid() const { return Node::isValid() && !m_sourceDir.empty() && !m_markupDir.empty(); };
		virtual tinyxml2::XMLElement* serialize(tinyxml2::XMLElement* parent);

		std::string getSourceDir() const { return m_sourceDir; };
		std::string getMarkupDir() const { return m_markupDir; };
		bool isIncludeSubDirs() const { return m_bIncludeSubDirs; };
	};
}