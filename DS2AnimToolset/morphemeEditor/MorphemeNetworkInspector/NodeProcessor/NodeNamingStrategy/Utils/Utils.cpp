#include "Utils.h"

#include "RLog/RLog.h"
#include "extern.h"

#include "../../NodeProcessor.h"

namespace NodeNameStrategyUtils
{
	const std::string getStateNodeNameFromStringTable(MR::NetworkDef* netDef, MR::NodeID nodeID)
	{
		const NMP::IDMappedStringTable* nodeIDNamesTable = netDef->getNodeIDNamesTable();

		// Nodes after the number of node defs in the string table are state machine states.
		for (size_t i = netDef->getNumNodeDefs(); i < nodeIDNamesTable->getNumEntries(); i++)
		{
			uint32_t entryID = nodeIDNamesTable->getEntryID(i);

			if (entryID == nodeID)
				return getNodeNameFromFullPath(nodeIDNamesTable->getEntryString(i));
		}

		return "";
	}

	const std::string getStateNodePathFromStringTable(MR::NetworkDef* netDef, MR::NodeID nodeID)
	{
		const NMP::IDMappedStringTable* nodeIDNamesTable = netDef->getNodeIDNamesTable();

		// Nodes after the number of node defs in the string table are state machine states.
		for (size_t i = netDef->getNumNodeDefs(); i < nodeIDNamesTable->getNumEntries(); i++)
		{
			uint32_t entryID = nodeIDNamesTable->getEntryID(i);

			if (entryID == nodeID)
				return nodeIDNamesTable->getEntryString(i);
		}

		return "";
	}

	const std::string getNodeNameFromFullPath(const std::string& name)
	{
		size_t lastDivider = name.find_last_of("|");

		if (lastDivider != std::string::npos)
			return name.substr(lastDivider + 1);

		return name;
	}

	const std::string getNodeNameWithParentFromFullPath(const std::string& name)
	{
		size_t lastDivider = name.find_last_of("|");

		if (lastDivider != std::string::npos)
		{
			size_t secondLastDivider = name.find_last_of("|", lastDivider - 1);

			if (secondLastDivider != std::string::npos)
				return name.substr(secondLastDivider + 1);
			else
				return name.substr(lastDivider + 1);
		}

		return name;
	}

	const std::string getParentNodeNameFromFullPath(const std::string& name)
	{
		size_t lastDivider = name.find_last_of("|");

		if (lastDivider != std::string::npos)
			return name.substr(0, lastDivider);

		return "";
	}

	const std::string getBlendTreeNodeNameAtLayer(const std::string& name, int layerIdx)
	{
		if (layerIdx < 0)
			return "";

		std::vector<size_t> dividerPositions;

		// Collect all '|' positions
		for (size_t pos = name.find('|'); pos != std::string::npos; pos = name.find('|', pos + 1))
			dividerPositions.push_back(pos);

		if (dividerPositions.empty())
			return ""; // No layers

		// Number of layers = number of dividers - 1 (exclude root and leaf)
		int numLayers = static_cast<int>(dividerPositions.size()) - 1;

		if (layerIdx > numLayers - 1)
			return ""; // Requested layer index out of bounds

		// Find start and end for the layer segment
		size_t start = dividerPositions[dividerPositions.size() - 2 - layerIdx] + 1;
		size_t end = dividerPositions[dividerPositions.size() - 1 - layerIdx];

		return name.substr(start, end - start);
	}
}