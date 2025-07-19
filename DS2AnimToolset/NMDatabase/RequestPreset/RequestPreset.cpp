#include "RequestPreset.h"

namespace db
{
	std::string RequestPreset::getManfiestPreset(int index)
	{
		if (index < 0 || index >= m_addedManifestPresets.size())
			throw std::out_of_range("Index out of range");

		return m_addedManifestPresets[index];
	}
}