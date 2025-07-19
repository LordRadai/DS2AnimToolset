#pragma once
#include "RCore.h"

namespace db
{
	class AnimationLocation
	{
		std::string m_sourceDir;
		std::string m_markupDir;
		bool m_bIncludeSubDirs;

	public:
		AnimationLocation() {};
		AnimationLocation(const std::string& sourceDir, const std::string& markupDir, bool bIncludeSubDirs = false)
			: m_sourceDir(sourceDir), m_markupDir(markupDir), m_bIncludeSubDirs(bIncludeSubDirs) {
		};

		~AnimationLocation() {};

		std::string getSourceDir() const { return m_sourceDir; };
		std::string getMarkupDir() const { return m_markupDir; };
		bool isIncludeSubDirs() const { return m_bIncludeSubDirs; };
	};
}