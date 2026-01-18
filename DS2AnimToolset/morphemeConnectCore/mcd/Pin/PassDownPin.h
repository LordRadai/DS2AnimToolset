#pragma once
#include "Pin.h"

namespace mcd
{
	class Graph;

	class PassDownPin : public Pin
	{
	public:
		PassDownPin(db::Node* parent, const std::string& name);

		virtual bool isCompatibleConnectionTarget(Pin* to) override;
		virtual bool canReceiveConnection(Pin* from) override;
		virtual bool canConnectTo(Pin* to) override;
		virtual bool breakConnectionTo(Pin* to) override;
		virtual bool canStartConnection() override;
		virtual bool canStartConnectionInGraph(mcd::Graph* graph) override;

		mcd::Graph* getChildGraph();
	};
}