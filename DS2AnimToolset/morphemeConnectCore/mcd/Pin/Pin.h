#pragma once
#include "NMDatabase/NMDatabase.h"
#include "mcd/Graph/Graph.h"

namespace mcd
{
	class Pin : public db::Node
	{
		std::unique_ptr<db::BoolAttribute> m_reference;

	public:
		Pin(db::Node* parent, const std::string& name, const std::string pinName)
			: db::Node(parent, name, pinName), 
			m_reference(std::make_unique<db::BoolAttribute>(this, "Reference", false)) {};

		virtual ~Pin() override {};
		virtual bool breakConnectionTo(Pin* to);
		virtual bool canConnectTo(Pin* to);
		virtual bool canReceiveConnection(Pin* from);
		virtual bool canStartConnection();
		virtual bool canStartConnectionInGraph(mcd::Graph* graph);

		void setReference(bool isReference);
		mcd::FlowEdge* connectTo(Pin* to);
	};
}