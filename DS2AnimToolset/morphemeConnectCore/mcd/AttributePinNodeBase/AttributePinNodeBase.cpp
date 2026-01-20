#include "AttributePinNodeBase.h"
#include "mcu/Log.h"
#include "mcd/MorphemeDB/MorphemeDB.h"
#include "mcd/Network/Network.h"

namespace mcd
{
	std::vector<db::Node> AttributePinNodeBase::getConnectedObjects(bool param_1, bool param_2)
	{
		std::vector<db::Node> connectedNodes;
		LOG_NOT_IMPLEMENTED();
		return connectedNodes;
	}

	std::vector<db::Node> AttributePinNodeBase::getDirectlyConnectedObjects(bool param_1, bool param_2)
	{
		std::vector<db::Node> connectedNodes;

		LOG_NOT_IMPLEMENTED();

		return connectedNodes;
	}

	bool AttributePinNodeBase::getOutputFunctionalPin(FunctionalPin** out)
	{
		if (out)
			*out = nullptr;

		return false;
	}

	mcd::MorphemeDB* AttributePinNodeBase::getMorphemeDatabase()
	{
		db::Database* database = getDatabase();

		if (database && database->isOfType<mcd::MorphemeDB>())
			return dynamic_cast<mcd::MorphemeDB*>(database);

		return nullptr;
	}

	mcd::Network* AttributePinNodeBase::getMorphemeNetwork()
	{
		mcd::MorphemeDB* morphemeDB = getMorphemeDatabase();

		if (morphemeDB)
			return morphemeDB->getNetwork();

		return nullptr;
	}
}
