#pragma once
#include "RCore.h"

namespace db
{
	class PreviewScript
	{
		std::string m_filepath;

	public:
		PreviewScript() {};
		PreviewScript(const std::string& filepath)
			: m_filepath(filepath) {
		}

		~PreviewScript() {};

		std::string getFilepath() const { return m_filepath; };
	};
}