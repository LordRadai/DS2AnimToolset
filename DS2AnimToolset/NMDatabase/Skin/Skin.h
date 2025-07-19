#pragma once
#include "RCore.h"

namespace db
{
	class Skin
	{
		std::string m_path;

	public:
		Skin() {};
		Skin(const std::string& path) : m_path(path) {}
		~Skin() {};

		std::string getPath() const { return m_path; }
	};
}