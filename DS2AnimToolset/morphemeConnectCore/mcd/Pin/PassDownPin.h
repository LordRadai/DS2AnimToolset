#pragma once
#include "Pin.h"

namespace mcd
{
	class Graph;
	class FunctionalPin;
	class DataPin;

	class PassDownPin : public Pin
	{
		enum class Multiplicity
		{
			kOneToOne,
			kOneToMany,

			kNumValues
		};

		std::unique_ptr<db::EnumAttribute> m_multiplicity;
	public:
		PassDownPin(db::Node* parent, const std::string& name, bool networkOut);

		virtual bool isCompatibleConnectionTarget(Pin* to) override;
		virtual bool canReceiveConnection(Pin* from) override;
		virtual bool canConnectTo(Pin* to) override;
		virtual bool breakConnectionTo(Pin* to) override;
		virtual bool canStartConnection() override;
		virtual bool canStartConnectionInGraph(mcd::Graph* graph) override;

		mcd::Graph* getChildGraph();
		mcd::Graph* getParentGraph();

		Multiplicity getMultiplicity() const;

		mcd::Pin* getUpstreamPin();
		mcd::Pin* recurseUpstreamToNonPassDownPin();
		mcd::FunctionalPin* recurseUpstreamToFunctionalPin();
		mcd::DataPin* recurseUpstreamToDataPin();

		void getDownstreamPins(std::vector<mcd::Pin*>& outPins);
		mcd::FunctionalPin* getFirstDownstreamFunctionalPin();
		mcd::DataPin* getFirstDownstreamDataPin();
		mcd::DataPin* recurseDownstreamToFirstDataPin();
	};
}