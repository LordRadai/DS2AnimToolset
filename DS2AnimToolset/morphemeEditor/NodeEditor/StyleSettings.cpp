#include "StyleSettings.h"

namespace NodeEditor
{
	StyleSettings styleSettings;

	StyleSettings::StyleSettings()
	{
		nodeMinWidth = 150.0f;
		nodeMinContentHeight = 30.0f;
		nodePinSpacing = 20.0f;
	}

	StyleSettings& getStyleSettings()
	{
		return styleSettings;
	}
}
