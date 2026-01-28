#pragma once

namespace NodeEditor
{
	struct StyleSettings
	{
		float nodeMinContentHeight;
		float nodeMinWidth;
		float nodePinSpacing;

		StyleSettings();
	};

	StyleSettings& getStyleSettings();
}
