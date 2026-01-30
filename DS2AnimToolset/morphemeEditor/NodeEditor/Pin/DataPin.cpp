#include "DataPin.h"

namespace NodeEditor
{
	void DataPin::draw()
	{
		drawInternal(ImNodesPinShape_QuadFilled, ImNodes::GetStyle().Colors[ImNodesCol_Pin]);
	}
}
