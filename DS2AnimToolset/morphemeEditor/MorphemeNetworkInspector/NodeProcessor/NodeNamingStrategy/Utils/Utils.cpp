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

	const std::string getBlendTreeNodeName(const std::string& name)
	{
		size_t lastDivider = name.find_last_of("|");

		if (lastDivider != std::string::npos)
		{
			size_t secondLastDivider = name.find_last_of("|", lastDivider - 1);
			size_t count = lastDivider - secondLastDivider - 1;

			if (secondLastDivider != std::string::npos)
				return name.substr(secondLastDivider + 1, count);
			else
				return name.substr(0, lastDivider);
		}

		return "";
	}
}